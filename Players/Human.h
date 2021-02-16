//
// Created by Lommy Reznak on 2/4/21.
//

#ifndef GOMOKU_HUMAN_H
#define GOMOKU_HUMAN_H

#include <unordered_set>
#include <memory>
#include "../Board.h"
#include <functional>
#include <utility>
#include "IPlayer.h"

namespace Gomoku
{
	class Human : public IPlayer
	{
		std::unordered_set<std::pair<int, int>, pairhash> availableMoves_;
	public:
		explicit Human(Board::Side side, MakeMove_t MakeMove, const Gomoku::Board &realBoard)
				: IPlayer(side, std::move(MakeMove), realBoard)
		{}

		void YourTurn(int row, int col, const std::vector<std::pair<int, int>>& availableMoves) override
		{
			myMove = true;
			availableMoves_ = {availableMoves.begin(), availableMoves.end()};
		}

		Board::MoveResult Ping() override;
	};
}


#endif //GOMOKU_HUMAN_H
