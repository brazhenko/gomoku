//
// Created by Lommy Reznak on 1/28/21.
//

#include "ChessClock.h"

std::string Gomoku::ChessClock::DurationToString(const std::chrono::duration<double> &duration)
{
	std::stringstream ss;

	if (duration <= std::chrono::milliseconds(0))
		return "00:00.0";

	const auto hours = std::chrono::duration_cast<std::chrono::hours>(duration).count();
	if (hours)
		ss << std::setw(2) << std::setfill('0') << hours << ":";

	const auto minutes = std::chrono::duration_cast<std::chrono::minutes>(duration).count();

	ss << std::setw(2) << std::setfill('0') << minutes % 60 << ":";
	ss << std::setw(2) << std::setfill('0') << std::chrono::duration_cast<std::chrono::seconds>(duration).count() % 60;

	if (minutes == 0)
		ss << "." << std::chrono::duration_cast<std::chrono::milliseconds>(duration).count() % 1000 / 100;

	return ss.str();
}


Gomoku::ChessClock::ChessClock(int whiteSeconds, int blackSeconds, bool whiteMove)
		: whiteTimeLeft{whiteSeconds * 1000 }
		, blackTimeLeft{blackSeconds * 1000 }
		, WhiteMove{ whiteMove }
		, PauseOn { true }
{}

void Gomoku::ChessClock::Start()
{
	PauseOn = false;

	auto tmp = std::chrono::system_clock::now();

	startWhite = tmp;
	startBlack = tmp;
}

void Gomoku::ChessClock::Pause()
{
	if (PauseOn) return;

	if (WhiteMove)
	{
		whiteTimeLeft
				= std::chrono::duration_cast<std::chrono::milliseconds>(whiteTimeLeft - (std::chrono::system_clock::now() - startWhite));
	}
	else
	{
		blackTimeLeft
				= std::chrono::duration_cast<std::chrono::milliseconds>(blackTimeLeft - (std::chrono::system_clock::now() - startBlack));
	}

	PauseOn = true;
}

void Gomoku::ChessClock::Continue()
{
	auto tmp = std::chrono::system_clock::now();

	startWhite = tmp;
	startBlack = tmp;
	PauseOn = false;
}

void Gomoku::ChessClock::Stop()
{
	whiteTimeLeft = std::chrono::milliseconds(0);
	blackTimeLeft = std::chrono::milliseconds(0);

	whiteTimeSpentForLastMove = {};
	blackTimeSpentForLastMove = {};

	WhiteMove = true;
	PauseOn = true;
}

void Gomoku::ChessClock::ChangeMove()
{
	if (!PauseOn)
	{
		auto now = std::chrono::system_clock::now();

		// Refreshing clock values
		if (WhiteMove)
		{
			auto tmp =  std::chrono::duration_cast<std::chrono::milliseconds>(whiteTimeLeft - (now - startWhite));
			this->whiteTimeSpentForLastMove = whiteTimeLeft - tmp;

			whiteTimeLeft = tmp;
			startBlack = now;
		}
		else
		{
			auto tmp = std::chrono::duration_cast<std::chrono::milliseconds>(blackTimeLeft - (now - startBlack));
			this->blackTimeSpentForLastMove = blackTimeLeft - tmp;

			blackTimeLeft = tmp;
			startWhite =  now;
		}
	}
	WhiteMove ^= true;
}

[[nodiscard]] bool Gomoku::ChessClock::WhiteTimeLeft() const
{
	auto nw = std::chrono::system_clock::now();

	std::chrono::duration<double> timeLeft{};
	if (!WhiteMove || PauseOn)
		timeLeft = whiteTimeLeft;
	else
		timeLeft = whiteTimeLeft - (nw - startWhite);

	if (timeLeft <= std::chrono::milliseconds(0))
		return false;
	return true;
}

[[nodiscard]] bool Gomoku::ChessClock::BlackTimeLeft() const
{
	auto nw = std::chrono::system_clock::now();

	std::chrono::duration<double> timeLeft{};

	if (WhiteMove || PauseOn)
		timeLeft = blackTimeLeft;
	else
		timeLeft = blackTimeLeft - (nw - startBlack);

	if (timeLeft <= std::chrono::milliseconds(0))
		return false;
	return true;
}

[[nodiscard]] std::string Gomoku::ChessClock::GetTimeLeftWhite() const
{
	auto nw = std::chrono::system_clock::now();

	std::chrono::duration<double> timeLeft{};
	if (!WhiteMove || PauseOn)
		timeLeft = whiteTimeLeft;
	else
		timeLeft = whiteTimeLeft - (nw - startWhite);

	return DurationToString(timeLeft);
}

[[nodiscard]] std::string Gomoku::ChessClock::GetTimeLeftBlack() const
{
	auto nw = std::chrono::system_clock::now();

	std::chrono::duration<double> timeLeft{};

	if (WhiteMove || PauseOn)
		timeLeft = blackTimeLeft;
	else
		timeLeft = blackTimeLeft - (nw - startBlack);

	return DurationToString(timeLeft);
}

[[nodiscard]] std::string Gomoku::ChessClock::GetTimeSpentWhite() const
{
	return DurationToString(whiteTimeSpentForLastMove);
}

[[nodiscard]] std::string Gomoku::ChessClock::GetTimeSpentBlack() const
{
	return DurationToString(blackTimeSpentForLastMove);
}