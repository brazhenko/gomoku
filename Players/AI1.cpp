//
// Created by Lommy Reznak on 2/3/21.
//

#include "AI1.h"
#include "../Engine.h"
#include <map>
#include <string>
#include <set>

void Gomoku::AI1::YourTurn(int row, int col, const std::unordered_set<std::pair<int, int>, pairhash> &availableMoves)
{
	myMove = true;
	availableMoves_ = availableMoves;

	if (this->currentBoard.StoneCount() == 0 && (availableMoves.find(Board::StringToMove("j10")) != availableMoves.end()))
	{
		this->nextMove = {Board::StringToMove("j10").first, Board::StringToMove("j10").second};
		return;
	}


	if (!FindNext())
	{
		tree = std::make_unique<CalcNode>(currentBoard);
	}


	int bestMeasure;
	if (this->side_ == Board::Side::White)
		bestMeasure= -100;
	else
		bestMeasure= +100;

	int c = 0;

	std::cout << currentBoard << std::endl;

	for (const auto &move: availableMoves)
	{
        if (!currentBoard.IsCellHasStoneNearby(move.first, move.second, 3)) continue;


		auto copy = this->currentBoard;

		copy.MakeMove(move.first, move.second);

		auto val = Gomoku::Engine::StaticPositionAnalize(copy);

		if (score1BetterThenScore2(val, bestMeasure))
		{
			bestMeasure = val;
			this->nextMove = {move.first, move.second};
		}

		if (val != 0)
		{
			std::cerr << "///////////////////" << Gomoku::Board::MoveToString(move) << ": " << val << "\n";
			c ++;
		}


		tree->children.emplace(std::make_unique<CalcNode>(std::move(copy)));
	}

	std::cout << "BEST MOVE: " << Gomoku::Board::MoveToString(this->nextMove) << ", avail:" << c << std::endl;
//	for (auto &a : s)
//		std::cout << a << std::endl;
	std::cout << "///////////////////" << std::endl;
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
