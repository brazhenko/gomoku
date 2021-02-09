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
	{
		tree = std::make_unique<CalcNode>(currentBoard);
	}


//	for (const auto &move: availableMoves)
//	{
//		auto copy = currentBoard;
//		copy.MakeMove(move.first, move.second);
//		std::cout << copy.GetAvailableMoves().size() << std::endl;
//
//		tree->children.emplace(std::make_unique<CalcNode>(std::move(copy)));
//	}

//	std::cout << tree->children.size() << std::endl;


	int  i = 0;
	int maxEval = -1000;

//	for (const auto &node: tree->children)
//	{
//		for (const auto &move: node->state_.GetAvailableMoves())
//		{
//
//			decltype(node->state_) copy;
//			copy = node->state_;
//			copy.MakeMove(move.first, move.second);
//			node->children.emplace(std::make_unique<CalcNode>(std::move(copy)));
//
////			i++;
//		}
//
//	}
	std::cout << i << std::endl;
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
