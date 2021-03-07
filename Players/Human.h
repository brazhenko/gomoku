//
// Created by Brazhenko Andrew on 2/4/21.
//

#ifndef GOMOKU_HUMAN_H
#define GOMOKU_HUMAN_H

#include <unordered_set>
#include <memory>
#include "Board.h"
#include <functional>
#include <utility>
#include "IPlayer.h"

namespace Gomoku
{
	class Human : public IPlayer
	{
		std::unordered_set<std::pair<int, int>, Gomoku::Board::PairHash> availableMoves_;
	public:
		explicit Human(Board::Side side, MakeMove_t MakeMove, const Gomoku::Board &realBoard, bool yourTurn)
				: IPlayer(side, std::move(MakeMove), realBoard)
		{
			if (yourTurn) Human::YourTurn();
		}

		void YourTurn() override
		{
			myMove = true;
			availableMoves_ = {currentBoard.GetAvailableMoves().begin(), currentBoard.GetAvailableMoves().end()};
		}

		Board::MoveResult Ping() override;
	};
}


#endif //GOMOKU_HUMAN_H
