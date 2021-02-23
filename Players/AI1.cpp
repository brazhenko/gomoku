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
    startThinking = std::chrono::system_clock::now();
	myMove = true;

	std::cout << "Started click" << std::endl;

	this->nextMove = currentBoard.GetAvailableMoves().front();
	need_clean = true;

    jobs_cv_.notify_one();

//	if (this->currentBoard.GetStoneCount() == 0
//		&& (std::find(currentBoard.GetAvailableMoves().begin(), currentBoard.GetAvailableMoves().end(),
//		std::make_pair(Board::StringToMove("j10").first, Board::StringToMove("j10").second))
//				!= currentBoard.GetAvailableMoves().end()))
//	{
//		this->nextMove = std::make_pair(Board::StringToMove("j10").first, Board::StringToMove("j10").second);
//		return;
//	}
}

Gomoku::Board::MoveResult Gomoku::AI1::Ping()
{
	if (!myMove) return Board::MoveResult::NotMyMove;

	// Time to think ended up, returning best move calculated !
	if (std::chrono::system_clock::now() >= startThinking + maxTimeToThink)
	{
		myMove = false;

		need_clean = true;
		jobs_cv_.notify_one();

		return MakeMove_(nextMove.first, nextMove.second);
	}

	return Board::MoveResult::NotReadyToMove;
}

bool Gomoku::AI1::FindNext()
{
	for (auto it = tree->children_.begin(); it != tree->children_.end(); it++)
	{
		std::cout << Board::MoveToString((*it)->state_.GetMovesList().back()) << " ";
		if ((*it)->state_ == currentBoard)
		{
			tree = *it;
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
                                       const std::shared_ptr<CalcNode>& node,
                                       bool max = true)
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

            std::cout << "log-" << std::endl;
        }

        std::partial_sort(pm.begin(), std::min(pm.begin() + countOfBestCandididates, pm.end()), pm.end(),
		[this, &node, max] (const std::pair<Board, int> &left, const std::pair<Board, int> &right) {
			std::cout << "log±" << std::endl;
			if (max)
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
			if (need_clean)
			{
				// cleaning
				need_clean = false;
				// Change tree root
				if (!FindNext())
				{
					std::cout << "Not found, generating new" << std::endl;
					tree = std::make_shared<CalcNode>(currentBoard, std::weak_ptr<CalcNode>(), true, 0);
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
							/*std::move(const_cast<Gomoku::Board&>( */
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

        // Lock the worker thread until some job appeared
        std::unique_lock<std::mutex> lk(jobs_cv_mtx_);
        jobs_cv_.wait(lk);

        std::cout << "Getting out of lock!" << std::endl;
    }
}
