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

		bool FindNext();

		std::pair<float, float> meanStone;
		int stoneCount = 0;

		void YourTurn(int row, int col, const std::unordered_set<std::pair<int, int>, pairhash>& availableMoves) override;

		BoardState::MoveResult Ping() override;
	};
}




#endif //GOMOKU_AI1_H
