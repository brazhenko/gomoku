//
// Created by 17641238 on 19.01.2021.
//

#include "Game.h"

Gomoku::Game::Game()
: clock_(0, 0)
{}

void Gomoku::Game::Go(const std::string &player1, const std::string &player2, const std::string &gameVersion,
					  const std::string &gameTime)
{
	if (this->state_ == State::GameInPause)
	{
		clock_.Continue();
		state_ = State::GameInProcess;
		return;
	}
	else if (this->state_ == State::Main)
	{
		if (gameTime == "1 minute")
			clock_ = {60, 60, board_.WhiteMove()};
		else if (gameTime == "3 minutes")
			clock_ = {180, 180, board_.WhiteMove()};
		else if (gameTime == "5 minutes")
			clock_ = {300, 300, board_.WhiteMove()};
		else if (gameTime == "10 minutes")
			clock_ = {600, 600, board_.WhiteMove()};
		else if (gameTime == "15 minutes")
			clock_ = {900, 900, board_.WhiteMove()};
		else
			clock_ = {100, 100, board_.WhiteMove()};

		Gomoku::MakeMove_t MakeMoveWhite = [this](int row, int col) {
			const auto &tmp = this->board_.GetAvailableMoves();

			if (std::find(tmp.begin(), tmp.end(), std::make_pair(row, col)) != tmp.end())
			{
				auto ret = this->board_.MakeMove({row, col});
				this->clock_.ChangeMove();
				this->blackPlayer->YourTurn();
				return ret;
			}
			return Board::MoveResult::Default;
		};

		Gomoku::MakeMove_t MakeMoveBlack = [this](int row, int col) {
			const auto &tmp = this->board_.GetAvailableMoves();

			if (std::find(tmp.begin(), tmp.end(), std::make_pair(row, col)) != tmp.end())
			{
				auto ret = this->board_.MakeMove({row, col});
				this->clock_.ChangeMove();
				this->whitePlayer->YourTurn();
				return ret;
			}
			return Board::MoveResult::Default;
		};

		whitePlayer = PlayerFactory(player1, Board::Side::White, MakeMoveWhite, board_, board_.WhiteMove());
		blackPlayer = PlayerFactory(player2, Board::Side::Black, MakeMoveBlack, board_, !board_.WhiteMove());

		state_ = State::GameInProcess;

		clock_.Start();
	}
}

void Gomoku::Game::Pause()
{
	if (this->state_ == State::GameInProcess)
	{
		state_ = State::GameInPause;
		clock_.Pause();
	}
}

void Gomoku::Game::Stop()
{
	state_ = State::Main;

	whitePlayer.reset();
	blackPlayer.reset();
	clock_.Stop();
	board_.Reset();
}

void Gomoku::Game::TakeBack()
{
	if (this->state_ == State::GameInProcess)
	{
		if (!board_.TakeBackMove())
			return;

		const auto &tmp = board_.GetMovesList();

		{
			if (board_.WhiteMove())
			{
				this->whitePlayer->YourTurn();
				this->blackPlayer->NotYourTurn();
			}
			else
			{
				this->blackPlayer->YourTurn();
				this->whitePlayer->NotYourTurn();
			}
		}
	}
}


