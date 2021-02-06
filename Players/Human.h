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
		explicit Human(BoardState::Side side, MakeMove_t MakeMove)
				: IPlayer(side, std::move(MakeMove))
		{}

		void YourTurn(int row, int col, const std::unordered_set<std::pair<int, int>, pairhash>& availableMoves) override
		{
			myMove = true;
			availableMoves_ = availableMoves;
		}

		BoardState::MoveResult Ping() override;
	};
}


#endif //GOMOKU_HUMAN_H