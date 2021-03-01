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
		: whiteTimeLeft_{ whiteSeconds * 1000 }
		, blackTimeLeft_{ blackSeconds * 1000 }
		, WhiteMove_{ whiteMove }
		, PauseOn_ { true }
{}

void Gomoku::ChessClock::Start()
{
	auto tmp = std::chrono::system_clock::now();

    startWhite_ = tmp;
    startBlack_ = tmp;

    PauseOn_ = false;
}

void Gomoku::ChessClock::Pause()
{
	if (PauseOn_) return;

	if (WhiteMove_)
	{
		whiteTimeLeft_
				= std::chrono::duration_cast<std::chrono::milliseconds>(whiteTimeLeft_ - (std::chrono::system_clock::now() - startWhite_));
	}
	else
	{
		blackTimeLeft_
				= std::chrono::duration_cast<std::chrono::milliseconds>(blackTimeLeft_ - (std::chrono::system_clock::now() - startBlack_));
	}

    PauseOn_ = true;
}

void Gomoku::ChessClock::Continue()
{
	auto tmp = std::chrono::system_clock::now();

    startWhite_ = tmp;
    startBlack_ = tmp;

    PauseOn_ = false;
}

void Gomoku::ChessClock::Stop()
{
	whiteTimeLeft_ = std::chrono::milliseconds(0);
	blackTimeLeft_ = std::chrono::milliseconds(0);

    whiteTimeSpentForLastMove_ = {};
    blackTimeSpentForLastMove_ = {};

    WhiteMove_ = true;
    PauseOn_ = true;
}

void Gomoku::ChessClock::ChangeMove()
{
	if (!PauseOn_)
	{
		auto now = std::chrono::system_clock::now();

		// Refreshing clock values
		if (WhiteMove_)
		{
			auto tmp =  std::chrono::duration_cast<std::chrono::milliseconds>(whiteTimeLeft_ - (now - startWhite_));
			this->whiteTimeSpentForLastMove_ = whiteTimeLeft_ - tmp;

			whiteTimeLeft_ = tmp;
            startBlack_ = now;
		}
		else
		{
			auto tmp = std::chrono::duration_cast<std::chrono::milliseconds>(blackTimeLeft_ - (now - startBlack_));
			this->blackTimeSpentForLastMove_ = blackTimeLeft_ - tmp;

			blackTimeLeft_ = tmp;
            startWhite_ =  now;
		}
	}
    WhiteMove_ ^= true;
}

[[nodiscard]] bool Gomoku::ChessClock::WhiteTimeLeft() const
{
	auto nw = std::chrono::system_clock::now();

	std::chrono::duration<double> timeLeft{};
	if (!WhiteMove_ || PauseOn_)
		timeLeft = whiteTimeLeft_;
	else
		timeLeft = whiteTimeLeft_ - (nw - startWhite_);

	if (timeLeft <= std::chrono::milliseconds(0))
		return false;
	return true;
}

[[nodiscard]] bool Gomoku::ChessClock::BlackTimeLeft() const
{
	auto nw = std::chrono::system_clock::now();

	std::chrono::duration<double> timeLeft{};

	if (WhiteMove_ || PauseOn_)
		timeLeft = blackTimeLeft_;
	else
		timeLeft = blackTimeLeft_ - (nw - startBlack_);

	if (timeLeft <= std::chrono::milliseconds(0))
		return false;
	return true;
}

[[nodiscard]] std::string Gomoku::ChessClock::GetTimeLeftWhite() const
{
	auto nw = std::chrono::system_clock::now();

	std::chrono::duration<double> timeLeft{};
	if (!WhiteMove_ || PauseOn_)
		timeLeft = whiteTimeLeft_;
	else
		timeLeft = whiteTimeLeft_ - (nw - startWhite_);

	return DurationToString(timeLeft);
}

[[nodiscard]] std::string Gomoku::ChessClock::GetTimeLeftBlack() const
{
	auto nw = std::chrono::system_clock::now();

	std::chrono::duration<double> timeLeft{};

	if (WhiteMove_ || PauseOn_)
		timeLeft = blackTimeLeft_;
	else
		timeLeft = blackTimeLeft_ - (nw - startBlack_);

	return DurationToString(timeLeft);
}

[[nodiscard]] std::string Gomoku::ChessClock::GetTimeSpentWhite() const
{
	return DurationToString(whiteTimeSpentForLastMove_);
}

[[nodiscard]] std::string Gomoku::ChessClock::GetTimeSpentBlack() const
{
	return DurationToString(blackTimeSpentForLastMove_);
}