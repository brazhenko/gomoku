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
		std::vector<Gomoku::Cell> postedStones;
	public:
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
					postedStones.push_back(*cell);
				}
				else
				{
					GomokuDraw::DrawStone(cell->placeX_, cell->placeY_, 1);
				}
			}
			else
			{
				GomokuDraw::ForbiddenCursor();
			}

			for (const auto &stone : postedStones)
				GomokuDraw::DrawStone(stone.placeX_, stone.placeY_, 2);
		}

		[[nodiscard]] bool isWhiteMove() const;


	};

}

#endif //GOMOKU_GAME_H
