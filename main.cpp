#include "GomokuDraw.h"
#include <iostream>
#include "Game.h"
#include "gtest/gtest.h"

int main()
{
//#if DEBUG
	testing::InitGoogleTest();
    (void)RUN_ALL_TESTS();
# if ONLYTEST
	return 0;
# endif

//#endif

	Gomoku::Game game{};

	if (!GomokuDraw::Init())
	{
		std::cerr << "Cannot init GomokuDraw" << std::endl;
		return (-1);
	}

	// Main loop
	while (GomokuDraw::Go())
	{
		if (game.state_ == Gomoku::Game::State::Start)
		{
			// Отрисовать какие-то менюшки, туториалы и прочее.
		}
		// Экран ожидания
		else if (game.state_ == Gomoku::Game::State::Main
				 || game.state_ == Gomoku::Game::State::GameInPause)
		{
			GomokuDraw::DrawSome();

			GomokuDraw::DrawStones(game.board_);

			GomokuDraw::DrawGameMenu(game);
			GomokuDraw::DrawGameMoves(game.board_);

			ImGui::End();
		}
		// Экран игры
		else if (game.state_ == Gomoku::Game::State::GameInProcess)
		{
			GomokuDraw::DrawSome();

			GomokuDraw::DrawStones(game.board_);

			auto w = game.whitePlayer->Ping();
			auto b = game.blackPlayer->Ping();

			bool wt = game.clock_.WhiteTimeLeft();
			bool bt = game.clock_.BlackTimeLeft();

			if (Gomoku::Board::MoveResult::WhiteWin == w
				|| !bt)
			{
				game.state_ = Gomoku::Game::State::GameEndedWhiteWin;
				game.clock_.Pause();
			}
			else if (Gomoku::Board::MoveResult::Draw == w)
			{
				game.state_ = Gomoku::Game::State::GameEndedDraw;
				game.clock_.Pause();
			}


			if (Gomoku::Board::MoveResult::BlackWin == b
				|| !wt)
			{
				game.state_ = Gomoku::Game::State::GameEndedBlackWin;
				game.clock_.Pause();
			}
			else if (Gomoku::Board::MoveResult::Draw == b)
			{
				game.state_ = Gomoku::Game::State::GameEndedDraw;
				game.clock_.Pause();
			}

			GomokuDraw::DrawGameMenu(game);
			GomokuDraw::DrawGameMoves(game.board_);

			ImGui::End();
		}
		else if (game.state_ == Gomoku::Game::State::GameEndedWhiteWin
				|| game.state_ == Gomoku::Game::State::GameEndedBlackWin
				|| game.state_ == Gomoku::Game::State::GameEndedDraw)
		{
			GomokuDraw::DrawSome();
			GomokuDraw::DrawStones(game.board_);

			GomokuDraw::DrawGameMenu(game);
			GomokuDraw::DrawGameMoves(game.board_);

			ImGui::End();
		}

		// Render
		GomokuDraw::Render();
	}

	// Cleanup
	GomokuDraw::Cleanup();

	return 0;
}
