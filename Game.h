//
// Created by 17641238 on 19.01.2021.
//

#ifndef GOMOKU_GAME_H
#define GOMOKU_GAME_H

#include <optional>
#include "Cell.h"
#include "Board.h"
#include "imgui.h"
#include "imfilebrowser.hpp"
#include "IPlayer.h"
#include <utility>
#include <memory>

namespace Gomoku
{
	class Game
	{
	private:
		int focusMove = 0;

	public:
		ImGui::FileBrowser fileDialogBoardPos;
		ImGui::FileBrowser fileDialogGame;

		std::unique_ptr<Gomoku::IPlayer> whitePlayer;
		std::unique_ptr<Gomoku::IPlayer> blackPlayer;

		Gomoku::BoardState board_;
		enum class State
		{
			Start = 0,
			Main,
			GameInProcess
		};
		State state_ = State::Main;

		Game()
		{
			// (optional) set browser properties
			fileDialogGame.SetTitle("Select game file...");
			fileDialogGame.SetTypeFilters({ ".pgn" });
			fileDialogGame.SetPwd(getenv("HOME"));

			fileDialogBoardPos.SetTitle("Select game position file...");
			fileDialogBoardPos.SetTypeFilters({ ".gg" });
			fileDialogBoardPos.SetPwd(getenv("HOME"));

		}

		void Start(
				const std::string &player1,
				const std::string &player2,
				const std::string &gameVersion
				)
		{
			Gomoku::MakeMove_t MakeMoveWhite = [this](int row, int col) {
				if (this->board_.GetAvailableMoves().find({row, col}) != this->board_.GetAvailableMoves().end())
				{
					this->board_.MakeMove(row, col);
					this->blackPlayer->YourTurn(row, col, this->board_.GetAvailableMoves());
				}
			};

			Gomoku::MakeMove_t MakeMoveBlack = [this](int row, int col) {
				if (this->board_.GetAvailableMoves().find({row, col}) != this->board_.GetAvailableMoves().end())
				{
					this->board_.MakeMove(row, col);
					this->whitePlayer->YourTurn(row, col, this->board_.GetAvailableMoves());
				}
			};

			whitePlayer = PlayerFactory(player1, BoardState::Side::White, MakeMoveWhite);
			blackPlayer = PlayerFactory(player2, BoardState::Side::Black, MakeMoveBlack);

			state_ = State::GameInProcess;
			whitePlayer->YourTurn(-1, -1, board_.GetAvailableMoves());
		}

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
