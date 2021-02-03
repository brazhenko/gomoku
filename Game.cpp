//
// Created by 17641238 on 19.01.2021.
//

#include "Game.h"

Gomoku::Game::Game()
: clock_(0, 0)
{
	// (optional) set browser properties
	fileDialogGame.SetTitle("Select game file...");
	fileDialogGame.SetTypeFilters({ ".pgn" });
	fileDialogGame.SetPwd(getenv("HOME"));

	fileDialogBoardPos.SetTitle("Select game position file...");
	fileDialogBoardPos.SetTypeFilters({ ".gg" });
	fileDialogBoardPos.SetPwd(getenv("HOME"));

}

void Gomoku::Game::Go(const std::string &player1, const std::string &player2, const std::string &gameVersion,
					  const std::string &gameTime)
{
	if (this->state_ == State::GameInProcess)
	{
		return;
	}
	else if (this->state_ == State::GameInPause)
	{
		clock_.Continue();
		state_ = State::GameInProcess;
		return;
	}

	if (gameTime == "1 minute")
		clock_ = {60, 60};
	else if (gameTime == "3 minutes")
		clock_ = {180, 180};
	else if (gameTime == "5 minutes")
		clock_ = {300, 300};
	else if (gameTime == "10 minutes")
		clock_ = {600, 600};
	else if (gameTime == "15 minutes")
		clock_ = {900, 900};
	else
		clock_ = {100, 100};

	Gomoku::MakeMove_t MakeMoveWhite = [this](int row, int col) {
		if (this->board_.GetAvailableMoves().find({row, col}) != this->board_.GetAvailableMoves().end())
		{
			auto ret = this->board_.MakeMove(row, col);
			this->clock_.ChangeMove();
			this->blackPlayer->YourTurn(row, col, this->board_.GetAvailableMoves());
			return ret;
		}
		return BoardState::MoveResult::Default;
	};

	Gomoku::MakeMove_t MakeMoveBlack = [this](int row, int col) {
		if (this->board_.GetAvailableMoves().find({row, col}) != this->board_.GetAvailableMoves().end())
		{
			auto ret = this->board_.MakeMove(row, col);
			this->clock_.ChangeMove();
			this->whitePlayer->YourTurn(row, col, this->board_.GetAvailableMoves());
			return ret;
		}
		return BoardState::MoveResult::Default;
	};

	whitePlayer = PlayerFactory(player1, BoardState::Side::White, MakeMoveWhite);
	blackPlayer = PlayerFactory(player2, BoardState::Side::Black, MakeMoveBlack);

	state_ = State::GameInProcess;
	whitePlayer->YourTurn(-1, -1, board_.GetAvailableMoves());
	clock_.Start();
}

void Gomoku::Game::Pause()
{
	state_ = State::GameInPause;
	clock_.Pause();
}

void Gomoku::Game::Stop()
{
	state_ = State::Main;
	clock_.Stop();
	board_.Reset();
}

void Gomoku::Game::TakeBack()
{
	board_.TakeBackMove();
	clock_.ChangeMove();

	const auto &tmp = board_.GetMovesList();

	if (tmp.empty())
	{
		if (board_.WhiteMove())
		{
			this->whitePlayer->YourTurn(-1, -1, board_.GetAvailableMoves());
			this->blackPlayer->NotYourTurn();
		}
		else
		{
			this->blackPlayer->YourTurn(-1, -1, board_.GetAvailableMoves());
			this->whitePlayer->NotYourTurn();
		}

	}
	else
	{
		if (board_.WhiteMove())
		{
			this->whitePlayer->YourTurn(tmp.back().first, tmp.back().second, board_.GetAvailableMoves());
			this->blackPlayer->NotYourTurn();
		}
		else
		{
			this->blackPlayer->YourTurn(tmp.back().first, tmp.back().second, board_.GetAvailableMoves());
			this->whitePlayer->NotYourTurn();
		}

	}
}

void Gomoku::Game::Reset()
{
	board_.Reset();
}


