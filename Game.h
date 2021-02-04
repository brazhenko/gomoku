//
// Created by 17641238 on 19.01.2021.
//

#ifndef GOMOKU_GAME_H
#define GOMOKU_GAME_H

#include <optional>
#include "Board.h"
#include "imgui.h"
#include "imfilebrowser.hpp"
#include "Players/IPlayer.h"
#include <utility>
#include <memory>
#include <thread>
#include "ChessClock.h"

namespace Gomoku
{
	class Game
	{
	public:
		std::unique_ptr<Gomoku::IPlayer> whitePlayer;
		std::unique_ptr<Gomoku::IPlayer> blackPlayer;

		Gomoku::BoardState board_;
		enum class State
		{
			Start = 0,
			Main,
			GameInProcess,
			GameInPause,
			GameEndedWhiteWin,
			GameEndedBlackWin,
			GameEndedDraw
		};
		State state_ = State::Main;
		Gomoku::ChessClock clock_;

		Game();
		void Go(const std::string &player1,
				const std::string &player2,
				const std::string &gameVersion,
				const std::string &gameTime);
		void Pause();
		void Stop();
		void TakeBack();
	};
}

#endif //GOMOKU_GAME_H
