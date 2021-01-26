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
		int focusMove = 0;
	public:

		Gomoku::BoardState board_;
		enum class State
		{
			Start,
			Main,
		};
		State state_ = State::Main;

		void TakeBack()
		{
			board_.TakeBackMove();
		}

		void Reset()
		{
			board_.Reset();
		}

	};

}

#endif //GOMOKU_GAME_H
