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
		std::vector<std::pair<int, int>> availableMoves_;

	public:
		explicit AI1(Board::Side side, MakeMove_t MakeMove, const Gomoku::Board &realBoard)
				: IPlayer(side, std::move(MakeMove), realBoard)
		{}

		struct CalcNode
		{
			CalcNode() = delete;
			explicit CalcNode(const Board& bs)
			{
				state_ = bs;
			}

			int positionScore = 0;
			Board state_;
			std::unordered_set<std::unique_ptr<CalcNode>> children;
		};

		std::unique_ptr<CalcNode> tree = std::make_unique<CalcNode>(Board{});

		bool FindNext();

		std::pair<float, float> meanStone;
		std::pair<int, int> nextMove;

		int stoneCount = 0;

		void YourTurn(int row, int col, const std::vector<std::pair<int, int>>& availableMoves) override;

		Board::MoveResult Ping() override;
	};
}




#endif //GOMOKU_AI1_H
