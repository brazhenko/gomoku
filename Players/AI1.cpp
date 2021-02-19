//
// Created by Lommy Reznak on 2/3/21.
//

#include "AI1.h"
#include "../Engine.h"
#include <map>
#include <future>
#include <queue>

void Gomoku::AI1::YourTurn(int row, int col, const std::vector<std::pair<int, int>> &availableMoves)
{
	auto t1 = std::chrono::high_resolution_clock::now();

	myMove = true;
	availableMoves_ = availableMoves;

	if (this->currentBoard.GetStoneCount() == 0
		&& (std::find(availableMoves.begin(), availableMoves.end(),
		std::make_pair(Board::StringToMove("j10").first, Board::StringToMove("j10").second))
				!= availableMoves.end()))
	{
		this->nextMove = std::make_pair(Board::StringToMove("j10").first, Board::StringToMove("j10").second);
		return;
	}

	if (!FindNext())
	{
		tree = std::make_unique<CalcNode>(currentBoard);
	}

	using moves_pq = std::priority_queue<
					std::pair<Board, int>,
					std::vector<std::pair<Board, int>>,
					std::function<bool(const std::pair<Board, int> &left, const std::pair<Board, int> &right)>>;


	auto findPerspectiveMoves = [this](std::vector<Board::pcell>::const_iterator left,
									   std::vector<Board::pcell>::const_iterator right,
									   bool max = true)
	{
		moves_pq perspectiveMoves([this, max]
		        (const std::pair<Board, int> &left, const std::pair<Board, int> &right)
        {
                return score1WorseThenScore2(left.second, right.second);
//				return score1BetterThenScore2(left.second, right.second);
		});


		for (; left != right; left++)
		{
			const auto &move = *left;
			if (!currentBoard.IsCellHasStoneNearby(move, 3)) continue;

			auto copy = this->currentBoard;
			copy.MakeMove(move);
			auto val = Gomoku::Engine::StaticPositionAnalize(copy);

			perspectiveMoves.emplace(std::move(copy), val);
		}

        return perspectiveMoves;
	};

	int countOfThreads = 0;

	std::vector<std::future<moves_pq>> futures;

	int tmp = availableMoves.size();
	const int countInThread = 30;
	if (tmp < 10)
		countOfThreads = 1;
	else
		countOfThreads = tmp / countInThread + (tmp % countInThread != 0);

	futures.reserve(countOfThreads);
	for (int i = 0; i < countOfThreads; i++)
	{
		futures.emplace_back(std::move(std::async(
				findPerspectiveMoves, availableMoves.begin() + i * countInThread,
				std::min(availableMoves.begin() + (i+1) * countInThread, availableMoves.end())
		)));
	}

	const int depth = 5;
	const int countOfBestCandididates = 3;


    moves_pq pq([this](const std::pair<Board, int> &left, const std::pair<Board, int> &right) {
			return score1WorseThenScore2(left.second, right.second);
	});

    for (int i = 0; i < countOfThreads; i++)
	{
		auto tmp2 = futures[i].get();

		for (int j = 0; j < countOfBestCandididates && !tmp2.empty(); j++)
		{
			pq.emplace(tmp2.top());

			tmp2.pop();
		}
	}


	if (!pq.empty() && !pq.top().first.GetMovesList().empty())
    {
        this->nextMove = pq.top().first.GetMovesList().back();
        std::cout << "MOVE:" << Board::MoveToString(pq.top().first.GetMovesList().back()) << std::endl;
    }
	else
    {
	    std::cout << "NOT FOUND" << std::endl;
    }


	for (int i = 0; i < countOfBestCandididates && !pq.empty(); i++)
	{
		tree->children.emplace_back(
		        std::make_unique<CalcNode>(
		                std::move(const_cast<Gomoku::Board&>(pq.top().first))
		                )
		                );

		pq.pop();
	}



	auto t2 = std::chrono::high_resolution_clock::now();
	auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count();
    std::cout << "BEST MOVE: " << Gomoku::Board::MoveToString(this->nextMove) << ", ms elapsed: " << duration << std::endl;
}

Gomoku::Board::MoveResult Gomoku::AI1::Ping()
{
	if (!myMove) return {};

	if (!availableMoves_.empty())
	{
		myMove = false;
		return MakeMove_(nextMove.first, nextMove.second);
	}

	return {};
}

bool Gomoku::AI1::FindNext()
{
	for (auto it = tree->children.begin(); it != tree->children.end(); it++)
	{
		if ((*it)->state_ == currentBoard)
		{
//			tree = std::move(tree->children.extract(it).value());
			return true;
		}
	}

	return false;
}
