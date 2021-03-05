//
// Created by Brazhenko Andrew on 19.01.2021.
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
#include "Engine.h"

namespace Gomoku
{
	/// @brief Facade for manipulating all game abstractions
	class Game
	{
	public:
		/// @brief white player object
		std::unique_ptr<Gomoku::IPlayer> whitePlayer;

		/// @brief black player object
		std::unique_ptr<Gomoku::IPlayer> blackPlayer;

		/// @brief board of current game
		Gomoku::Board board_;
		/// @brief engine instance
		Gomoku::Engine engine;

		/// @brief clock object
		Gomoku::ChessClock clock_;

		/// @brief state type of the game
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

		/// @brief state of the game
		State state_ = State::Main;

		/// @brief default ctor
		Game();

		/// @brief start/continue the game
		/// @param player1 string repr of white player
		/// @param player2 string repr of black player
		/// @param gameVersion string repr of game version
		/// @param gameTime string repr of time control
		void Go(const std::string &player1,
				const std::string &player2,
				const std::string &gameVersion,
				const std::string &gameTime);

		/// @brief pause the game
		void Pause();

		/// @brief stop the game
		void Stop();

		/// @brief make a takeback
		void TakeBack();
	};
}

#endif //GOMOKU_GAME_H
