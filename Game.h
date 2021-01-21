//
// Created by 17641238 on 19.01.2021.
//

#ifndef GOMOKU_GAME_H
#define GOMOKU_GAME_H

#include <optional>
#include "Cell.h"
#include "GomokuDraw.h"
#include "Board.h"


namespace Gomoku
{
	class Game
	{
	private:
		bool WhiteMove = true;
	public:

		std::vector<std::pair<Gomoku::Cell, int>> postedStones;
		Gomoku::Board board_;
		enum class State
		{
			Start,
			Main,
		};
		State state_ = State::Main;

		void ProcessStone(std::optional<Gomoku::Cell> cell)
		{
			if (cell == std::nullopt)
				return ;

			if (board_.isFree(cell->col_, cell->row_))
			{
				if (cell->pressed_)
				{
					this->board_.arr[cell->col_][cell->row_] = 1;
					if (WhiteMove)
						postedStones.emplace_back(*cell, 2);
					else
						postedStones.emplace_back(*cell, 4);

					WhiteMove ^= true; // Change move
				}
				else
				{
					if (WhiteMove)
						GomokuDraw::DrawStone(cell->placeX_, cell->placeY_, 1);
					else
						GomokuDraw::DrawStone(cell->placeX_, cell->placeY_, 3);
				}
			}
			else
			{
				GomokuDraw::ForbiddenCursor();
			}
		}

		[[nodiscard]] bool isWhiteMove() const;


	};

}

#endif //GOMOKU_GAME_H
