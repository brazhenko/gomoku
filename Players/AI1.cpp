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
	this->nextMove = currentBoard.GetAvailableMoves().front();

	if (!FindNext())
	{
		tree = std::make_shared<CalcNode>(currentBoard, std::weak_ptr<CalcNode>(), true);
	}

	{
		std::lock_guard<std::mutex> lg(jobs_mtx_);
		// Empty queue
		std::queue<std::pair<int /* depth */, std::shared_ptr<CalcNode>>> empty;
		std::swap(empty, jobs_);
		jobs_.emplace(0, tree);
	}
    jobs_cv_.notify_one();

	if (this->currentBoard.GetStoneCount() == 0
		&& (std::find(currentBoard.GetAvailableMoves().begin(), currentBoard.GetAvailableMoves().end(),
		std::make_pair(Board::StringToMove("j10").first, Board::StringToMove("j10").second))
				!= currentBoard.GetAvailableMoves().end()))
	{
		this->nextMove = std::make_pair(Board::StringToMove("j10").first, Board::StringToMove("j10").second);
		return;
	}


	const int countOfBestCandididates = 3;

	auto findPerspectiveMoves = [this](std::vector<Board::pcell>::const_iterator left,
									   std::vector<Board::pcell>::const_iterator right,
									   const std::shared_ptr<CalcNode>& node,
									   bool max = true)
	{
		std::vector<std::pair<Board, int>> pm;
		pm.reserve(right - left);

		for (; left != right; left++)
		{
			const auto &move = *left;
			if (!node->state_.IsCellHasStoneNearby(move, 2)) continue;

			auto copy = node->state_;
			copy.MakeMove(move);
			auto val = Gomoku::Engine::StaticPositionAnalize(copy);

			pm.emplace_back(std::move(copy), val);
		}

		std::partial_sort(pm.begin(), std::min(pm.begin() + countOfBestCandididates, pm.end()), pm.end(),
        [this, &node] (const std::pair<Board, int> &left, const std::pair<Board, int> &right) {
				return score1BetterThenScore2(left.second, right.second);
        });

        return pm;
	};


	int countOfThreads = 0;
	std::vector<std::future<std::vector<std::pair<Board, int>>>> futures;

    auto t1 = std::chrono::high_resolution_clock::now();


    int tmp = currentBoard.GetAvailableMoves().size();
	const int countInThread = 15;

	if (tmp < 10) countOfThreads = 1;
	else countOfThreads = tmp / countInThread + (tmp % countInThread != 0);

	futures.reserve(countOfThreads);
	for (int i = 0; i < countOfThreads; i++)
	{
		futures.emplace_back(std::move(std::async(
				findPerspectiveMoves, currentBoard.GetAvailableMoves().begin() + i * countInThread,
				std::min(currentBoard.GetAvailableMoves().begin() + (i+1) * countInThread, currentBoard.GetAvailableMoves().end()),
				tree
		)));
	}

    moves_pq pq([this](const std::pair<Board, int> &left, const std::pair<Board, int> &right) {
			return score1WorseThenScore2(left.second, right.second);
	});

    for (int i = 0; i < countOfThreads; i++)
	{
		auto tmp2 = futures[i].get();

		for (int j = 0; j < countOfBestCandididates && j < tmp2.size(); j++)
		{
			pq.emplace(tmp2[j]);
		}
	}

	if (!pq.empty() && !pq.top().first.GetMovesList().empty())
    {
        this->nextMove = pq.top().first.GetMovesList().back();
    }
	else
    {
	    std::cout << "NOT FOUND" << std::endl;
    }


	auto t2 = std::chrono::high_resolution_clock::now();
	auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count();
    std::cout << "BEST MOVE: " << Gomoku::Board::MoveToString(this->nextMove) << ", ms elapsed: " << duration << std::endl;
}

Gomoku::Board::MoveResult Gomoku::AI1::Ping()
{
	if (!myMove) return Board::MoveResult::NotMyMove;

	// Time to think ended up, returning best move calculated !
	if (std::chrono::system_clock::now() >= startThinking + maxTimeToThink)
	{
		myMove = false;
		return MakeMove_(nextMove.first, nextMove.second);
	}

	return Board::MoveResult::NotReadyToMove;
}

bool Gomoku::AI1::FindNext()
{
	for (auto it = tree->children_.begin(); it != tree->children_.end(); it++)
	{
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
        }

        std::partial_sort(pm.begin(), std::min(pm.begin() + countOfBestCandididates, pm.end()), pm.end(),
                          [this, &node]
                          (const std::pair<Board, int> &left, const std::pair<Board, int> &right){
                              return score1BetterThenScore2(left.second, right.second);
                          });

        return pm;
    };

    std::pair<int /* depth */, std::shared_ptr<CalcNode>> var;

	while (work_.load())
	{
	    std::cout << "// Work start! //" << std::endl;
        auto t1 = std::chrono::high_resolution_clock::now();

        while (std::lock_guard(jobs_mtx_), !jobs_.empty() && jobs_.front().first < depth_)
        {
            {   // Pop out a new job (node) to work on
                std::lock_guard<std::mutex> lg(jobs_mtx_);
                var = jobs_.front();
                jobs_.pop();
            }

            // vector of job split in to pieces
            std::vector<std::future<std::vector<std::pair<Board, int>>>> futures;
            const auto &avlMoves = var.second->state_.GetAvailableMoves();

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
                        var.second)));

            moves_pq pq([this](const std::pair<Board, int> &left, const std::pair<Board, int> &right) {
                return score1WorseThenScore2(left.second, right.second);
            });

            // get countOfBestCandididates * countOfThreads best moves
            for (int i = 0; i < countOfThreads; i++)
            {
                auto result = futures[i].get();
                for (int j = 0; j < countOfBestCandididates && j < result.size(); j++)
                    pq.emplace((result[j]));
            }

            // put countOfBestCandididates best moves in jobs queue and calculation tree
            for (int i = 0; i < countOfBestCandididates && !pq.empty(); i++)
            {
                var.second->children_.emplace_back(std::make_shared<CalcNode>(
                        std::move(const_cast<Gomoku::Board&>(pq.top().first)),
                        var.second,
						var.second->maximize_ ^ true));
                pq.pop();

                std::lock_guard lg(jobs_mtx_);
                jobs_.emplace(var.first + 1, var.second->children_.back());
            }

        }

        auto t2 = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count();

        std::cout << "// Work done! Duration: " << duration << " | Jobs: " << jobs_.size() << " //" << std::endl;

        // Lock the worker thread until some job appeared
        std::unique_lock<std::mutex> lk(jobs_cv_mtx_);
        jobs_cv_.wait(lk, [this]{
            std::lock_guard<std::mutex> lg(jobs_mtx_);
            return (!jobs_.empty() && jobs_.front().first < depth_) || !work_;
        });
    }
}
