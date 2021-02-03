//
// Created by Lommy Reznak on 2/3/21.
//

#ifndef GOMOKU_AI1_H
#define GOMOKU_AI1_H

#include "IPlayer.h"

namespace Gomoku
{
	class AI1 : public IPlayer
	{
		std::unordered_set<std::pair<int, int>, pairhash> availableMoves_;

	public:
		explicit AI1(BoardState::Side side, MakeMove_t MakeMove)
				: IPlayer(side, std::move(MakeMove))
		{}

		struct CalcNode
		{
			CalcNode() = delete;
			explicit CalcNode(const BoardState& bs)
			{
				state_ = bs;
			}

			int positionScore = 0;
			BoardState state_;
			std::vector<std::unique_ptr<CalcNode>> children;
		};

		std::unique_ptr<CalcNode> tree = std::make_unique<CalcNode>(BoardState{});
		BoardState currentBoard{};

		bool FindNext()
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

		std::pair<float, float> meanStone;
		int stoneCount = 0;

		void YourTurn(int row, int col, const std::unordered_set<std::pair<int, int>, pairhash>& availableMoves) override
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
				currentBoard.MakeMove(move.first, move.second);
				tree->children.emplace_back(std::make_unique<CalcNode>(std::move(copy)));
			}

		}

		BoardState::MoveResult Ping() override
		{
			if (!myMove) return {};

			if (!availableMoves_.empty())
			{
				myMove = false;
				return MakeMove_(availableMoves_.begin()->first, availableMoves_.begin()->second);
			}

			return {};
		}
	};
}




#endif //GOMOKU_AI1_H
