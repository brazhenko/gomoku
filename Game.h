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

namespace Gomoku
{
	class Game
	{
	private:
		int focusMove = 0;
	public:
		ImGui::FileBrowser fileDialogBoardPos;
		ImGui::FileBrowser fileDialogGame;

		Gomoku::BoardState board_;
		enum class State
		{
			Start,
			Main,
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
