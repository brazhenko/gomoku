//
// Created by Lommy Reznak on 2/6/21.
//

#ifndef GOMOKU_AI2_H
#define GOMOKU_AI2_H


#include "IPlayer.h"

namespace Gomoku
{
	class AI2 : public IPlayer
	{
		std::unordered_set<std::pair<int, int>, pairhash> availableMoves_;

	public:
		explicit AI2(BoardState::Side side, MakeMove_t MakeMove)
				: IPlayer(side, std::move(MakeMove))
		{}


		void YourTurn(int row, int col, const std::unordered_set<std::pair<int, int>, pairhash>& availableMoves) override;

		BoardState::MoveResult Ping() override;
	};
}


#endif //GOMOKU_AI2_H
