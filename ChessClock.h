//
// Created by Lommy Reznak on 1/28/21.
//

#ifndef GOMOKU_CHESSCLOCK_H
#define GOMOKU_CHESSCLOCK_H

#include <string>
#include <sstream>
#include <iostream>
#include <iomanip>
#include "Board.h"

namespace Gomoku
{
	class ChessClock
	{
		bool WhiteMove = true;
		bool PauseOn = true;

		std::chrono::system_clock::time_point startWhite{};
		std::chrono::system_clock::time_point startBlack{};

		std::chrono::milliseconds whiteTimeLeft{};
		std::chrono::milliseconds blackTimeLeft{};

		std::chrono::milliseconds whiteTimeSpentForLastMove{};
		std::chrono::milliseconds blackTimeSpentForLastMove{};

	public:
		static std::string DurationToString(const std::chrono::duration<double> &duration)
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

		ChessClock() = delete;

		ChessClock(int whiteSeconds, int blackSeconds)
				: whiteTimeLeft{whiteSeconds * 1000 }
				, blackTimeLeft{blackSeconds * 1000 }
				, WhiteMove{ true }
				, PauseOn { true }
		{}

		void Start()
		{
			PauseOn = false;

			startWhite = std::chrono::system_clock::now();
			startBlack = std::chrono::system_clock::now();
		}

		void Pause()
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

		void Continue()
		{
			auto tmp = std::chrono::system_clock::now();

			startWhite = tmp;
			startBlack = tmp;
			PauseOn = false;
		}

		void Stop()
		{
			whiteTimeLeft = std::chrono::milliseconds(1000 * 20);
			blackTimeLeft = std::chrono::milliseconds(1000 * 20);

			whiteTimeSpentForLastMove = {};
			blackTimeSpentForLastMove = {};
			
			WhiteMove = true;
			PauseOn = true;
		}

		void ChangeMove()
		{
			if (!PauseOn)
			{
				// Refreshing clock values
				if (WhiteMove)
				{
					auto tmp =  std::chrono::duration_cast<std::chrono::milliseconds>(whiteTimeLeft - (std::chrono::system_clock::now() - startWhite));
					this->whiteTimeSpentForLastMove = whiteTimeLeft - tmp;

					whiteTimeLeft = tmp;
					startBlack = std::chrono::system_clock::now();
				}
				else
				{
					auto tmp = std::chrono::duration_cast<std::chrono::milliseconds>(blackTimeLeft - (std::chrono::system_clock::now() - startBlack));
					this->blackTimeSpentForLastMove = blackTimeLeft - tmp;

					blackTimeLeft = tmp;
					startWhite =  std::chrono::system_clock::now();
				}
			}
			WhiteMove ^= true;
		}

		[[nodiscard]] bool WhiteTimeLeft() const
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

		[[nodiscard]] bool BlackTimeLeft() const
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

		[[nodiscard]] std::string GetTimeLeftWhite() const
		{

			auto nw = std::chrono::system_clock::now();

			std::chrono::duration<double> timeLeft{};
			if (!WhiteMove || PauseOn)
				timeLeft = whiteTimeLeft;
			else
				timeLeft = whiteTimeLeft - (nw - startWhite);

			return DurationToString(timeLeft);
		}

		[[nodiscard]] std::string GetTimeLeftBlack() const
		{
			auto nw = std::chrono::system_clock::now();

			std::chrono::duration<double> timeLeft{};

			if (WhiteMove || PauseOn)
				timeLeft = blackTimeLeft;
			else
				timeLeft = blackTimeLeft - (nw - startBlack);

			return DurationToString(timeLeft);
		}

		// These methods return time spent for last move
		[[nodiscard]] std::string GetTimeSpentWhite() const
		{
			return DurationToString(whiteTimeSpentForLastMove);
		}
		[[nodiscard]] std::string GetTimeSpentBlack() const
		{
			return DurationToString(blackTimeSpentForLastMove);
		}
	};
}

#endif //GOMOKU_CHESSCLOCK_H
