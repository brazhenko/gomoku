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
		explicit AI2(Board::Side side, MakeMove_t MakeMove, const Gomoku::Board &realBoard)
				: IPlayer(side, std::move(MakeMove), realBoard)
		{}


		void YourTurn() override;

		Board::MoveResult Ping() override;
	};
}


#endif //GOMOKU_AI2_H
