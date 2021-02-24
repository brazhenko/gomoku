//
// Created by Lommy Reznak on 2/3/21.
//

#include "AI1.h"
#include "../Engine.h"
#include <map>
#include <future>
#include <queue>

void Gomoku::AI1::YourTurn()
{
	myMove = true;
    startThinking = std::chrono::system_clock::now();

	std::cout << "Your turn!" << std::endl;

	// setting first available move as default
	if (currentBoard.GetAvailableMoves().empty())
        return;

    this->nextMove = currentBoard.GetAvailableMoves().front();

	// reload tree root in worker
    need_reload = true;
    jobs_cv_.notify_one();
}

Gomoku::Board::MoveResult Gomoku::AI1::Ping()
{
	if (!myMove) return Board::MoveResult::NotMyMove;

	if (std::chrono::system_clock::now() >= startThinking + maxTimeToThink)
	{
        // Time to think ended up, returning best move calculated !

        std::cout << "Making Move" << std::endl;
		auto ret = MakeMove_(nextMove.first, nextMove.second);

        myMove = false;
        need_reload = true;
        jobs_cv_.notify_one();

        return ret;
	}

	return Board::MoveResult::NotReadyToMove;
}

bool Gomoku::AI1::FindNext()
{
    std::cout << std::boolalpha << tree->maximize_ << " ";

    for (auto it = tree->children_.begin(); it != tree->children_.end(); it++)
	{
		std::cout << Board::MoveToString((*it)->state_.GetMovesList().back()) << "~" << (*it)->positionScore_ << " ";
		if ((*it)->state_ == currentBoard)
		{
			tree = *it;
			tree->parent_.reset();
			return true;
		}
	}

	return false;
}

void Gomoku::AI1::Worker()
{
    constexpr int countOfBestCandididates = 3;
    constexpr int localityRadius = 2;

    auto findPerspectiveMoves = [this](std::vector<Board::pcell>::const_iterator left,
                                       std::vector<Board::pcell>::const_iterator right,
                                       const std::shared_ptr<CalcNode>& node)
    {
        std::vector<std::pair<Board, int>> pm;
        pm.reserve(right - left);

        for (; left != right; left++)
        {
            if (!node->state_.IsCellHasStoneNearby(*left, localityRadius))
                continue;

            auto copy = node->state_;
            copy.MakeMove(*left);
            auto val = Gomoku::Engine::StaticPositionAnalize(copy);
            pm.emplace_back(std::move(copy), val);

        }

        std::partial_sort(pm.begin(), std::min(pm.begin() + countOfBestCandididates, pm.end()), pm.end(),
		[this, &node] (const std::pair<Board, int> &left, const std::pair<Board, int> &right) {
			if (node->maximize_)
				return score1BetterThenScore2(left.second, right.second);
			return score1WorseThenScore2(left.second, right.second);
		});

        return pm;
    };

    std::shared_ptr<CalcNode> var;

	while (work_.load())
	{
	    std::cout << "// Work start! //" << std::endl;
        auto t1 = std::chrono::high_resolution_clock::now();

        while (true)
        {
            if (!work_) return;

			if (need_reload)
			{
                std::lock_guard lg(jobs_mtx_);
				// cleaning
				need_reload = false;
				// Change tree root
				if (!FindNext())
				{
					std::cout << "Not found, generating new" << std::endl;
					tree = std::make_shared<CalcNode>(currentBoard, std::weak_ptr<CalcNode>(), true, 0);
//					std::cout << "l0" << std::endl;
				}
				else
					std::cout << "found child, replacing" << std::endl;

				// Empty queue
				jobs_ = {};
				jobs_.emplace(tree);
				best_ = Min;
			}

            {   // Pop out a new job of break!
                std::lock_guard lg(jobs_mtx_);

                if (jobs_.empty() || jobs_.front()->Depth() > depth_)
					break;


//				std::cout << "Depth: " << jobs_.front()->Depth() << " size: " << jobs_.size() << std::endl;
                var = jobs_.front();
                jobs_.pop();

                // costyl
				if (var->Depth() == 2 && score1BetterThenScore2(var->positionScore_, this->best_))
				{
					this->nextMove = var->state_.GetMovesList().back();
					this->best_ = var->positionScore_;
				}

                if (!var->children_.empty())
				{
                	for (auto &child : var->children_)
                		jobs_.push(child);
					continue;
				}
            }


            // vector of job split in to pieces
            std::vector<std::future<std::vector<std::pair<Board, int>>>> futures;
            const auto &avlMoves = var->state_.GetAvailableMoves();

            int tmp = avlMoves.size();
            int countOfThreads;
            constexpr int countInThread = 30;
            if (tmp < 10)
                countOfThreads = 1;
            else
                countOfThreads = tmp / countInThread + (tmp % countInThread != 0);

            // generate job pieces
            futures.reserve(countOfThreads);
            for (int i = 0; i < countOfThreads; i++)
                futures.emplace_back(std::move(std::async(
                        findPerspectiveMoves, avlMoves.begin() + i * countInThread,
                        std::min(avlMoves.begin() + (i+1) * countInThread, avlMoves.end()),
                        var)));

            moves_pq pq([this, &var](const std::pair<Board, int> &left, const std::pair<Board, int> &right) {
				if (var->maximize_)
					return score1WorseThenScore2(left.second, right.second);
				return score1BetterThenScore2(left.second, right.second);
            });


            // get countOfBestCandididates * countOfThreads best moves
            for (int i = 0; i < countOfThreads; i++)
            {
                auto result = futures[i].get();
                for (int j = 0; j < countOfBestCandididates && j < result.size(); j++)
                    pq.emplace((result[j]));
            }

            // put countOfBestCandididates best moves in jobs queue and calculation tree
			{
				std::lock_guard lg(jobs_mtx_);

				for (int i = 0; i < countOfBestCandididates && !pq.empty(); i++)
				{
					// costyl
					if (var->Depth() == 2 && score1BetterThenScore2(var->positionScore_, this->best_))
					{
						this->nextMove = var->state_.GetMovesList().back();
						this->best_ = var->positionScore_;
					}

					var->children_.emplace_back(std::make_shared<CalcNode>(
							/* std::move(const_cast<Gomoku::Board&>() */
							pq.top().first,
							var,
							var->maximize_ ^ true,
							pq.top().second));
					pq.pop();

					jobs_.emplace(var->children_.back());
				}
			}

        }

        auto t2 = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count();

        std::cout << "// Work done! Duration: " << duration << " | Jobs: " << jobs_.size() << " //" << std::endl;

        if (!work_) return;

        // Lock the worker thread until some job appeared
        std::unique_lock lk(jobs_cv_mtx_);
        jobs_cv_.wait(lk);

        std::cout << "Getting out of lock!" << std::endl;
    }
}
