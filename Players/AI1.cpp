//
// Created by Lommy Reznak on 2/3/21.
//

#include "AI1.h"

void Gomoku::AI1::YourTurn(int row, int col, const std::unordered_set<std::pair<int, int>, pairhash> &availableMoves)
{
	myMove = true;
	availableMoves_ = availableMoves;

	if (row != -1)
		currentBoard.MakeMove(row, col);

	// Add move to
	meanStone = {(meanStone.first * float(stoneCount) + float(row)) / float(stoneCount + 1),
				 (meanStone.first * float(stoneCount) + float(row)) / float(stoneCount + 1)};
	stoneCount++;


	if (!FindNext())
		tree = std::make_unique<CalcNode>(currentBoard);

	for (const auto &move: availableMoves)
	{
		auto copy = currentBoard;
		copy.MakeMove(move.first, move.second);
		tree->children.emplace_back(std::make_unique<CalcNode>(std::move(copy)));
	}

	int  i = 0;
	for (const auto &node: tree->children)
	{
		for (const auto &move: node->state_.GetMovesList())
		{
			decltype(node->state_) copy;
			copy = node->state_;
			copy.MakeMove(move.first, move.second);
			tree->children.emplace_back(std::make_unique<CalcNode>(std::move(copy)));
		}
		i++;
		if (i == 77)
			break;
	}
}

Gomoku::BoardState::MoveResult Gomoku::AI1::Ping()
{
	if (!myMove) return {};

	if (!availableMoves_.empty())
	{
		myMove = false;
		return MakeMove_(availableMoves_.begin()->first, availableMoves_.begin()->second);
	}

	return {};
}

bool Gomoku::AI1::FindNext()
{
	for (int i = 0; i < tree->children.size(); i++)
	{
		if (tree->children[i]->state_ == currentBoard)
		{
			tree = std::move(tree->children[i]);
			return true;
		}
	}
	return false;
}
