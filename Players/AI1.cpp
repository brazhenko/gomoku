//
// Created by Lommy Reznak on 2/3/21.
//

#include "AI1.h"
#include "../Engine.h"
#include <map>
#include <string>
#include <set>
#include <future>

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


	auto ll = [this](std::vector<std::pair<int, int>>::const_iterator left,
					 std::vector<std::pair<int, int>>::const_iterator right
	) -> std::pair<std::pair<int, int> /* move */, int /* score */>
	{
		int bestMeasure;
		if (this->side_ == Board::Side::White) bestMeasure= -100; else bestMeasure= +100;
		std::pair<int, int> retMove;

		for (; left != right; left++)
		{
			const auto &move = *left;

			if (!currentBoard.IsCellHasStoneNearby(move.first, move.second, 3)) continue;

			auto copy = this->currentBoard;

			copy.MakeMove(move.first, move.second);

			auto val = Gomoku::Engine::StaticPositionAnalize(copy);

			if (score1BetterThenScore2(val, bestMeasure))
			{
				bestMeasure = val;
				retMove = {move.first, move.second};
			}
		}

		return { retMove, bestMeasure };
	};

	int countOfThreads = 0;

	std::vector<std::future<std::pair<std::pair<int, int> /* move */, int /* score */>>> futures;

	int tmp = availableMoves.size();
	const int countInThread = 10;
	if (tmp < 10)
		countOfThreads = 1;
	else
		countOfThreads = tmp / countInThread + (tmp % countInThread != 0);

	std::cout << currentBoard << std::endl;


	futures.reserve(countOfThreads);
	for (int i = 0; i < countOfThreads; i++)
	{
		futures.emplace_back(std::move(std::async(
			ll, availableMoves.begin() + i * countInThread,
			std::min(availableMoves.begin() + (i+1) * countInThread, availableMoves.end())
		)));
	}

	int bestMeasure;
	if (this->side_ == Board::Side::White) bestMeasure= -100; else bestMeasure= +100;
	for (int i = 0; i < countOfThreads; i++)
	{
		auto tmp2 = futures[i].get();

		std::cout << "best " << i << " :" << Gomoku::Board::MoveToString(tmp2.first) << ", val: " << tmp2.second << std::endl;
		if (score1BetterThenScore2(tmp2.second, bestMeasure))
		{
			this->nextMove = tmp2.first;
			bestMeasure = tmp2.second;
		}

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
			tree = std::move(tree->children.extract(it).value());
			return true;
		}
	}

	return false;
}
