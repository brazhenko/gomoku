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
		static std::string DurationToString(const std::chrono::duration<double> &duration);

		ChessClock() = delete;
		ChessClock(int whiteSeconds, int blackSeconds, bool whiteMove=true);

		void Start();
		void Pause();
		void Continue();
		void Stop();

		void ChangeMove();

		[[nodiscard]] bool WhiteTimeLeft() const;
		[[nodiscard]] bool BlackTimeLeft() const;

		[[nodiscard]] std::string GetTimeLeftWhite() const;
		[[nodiscard]] std::string GetTimeLeftBlack() const;

		// These methods return time spent for last move
		[[nodiscard]] std::string GetTimeSpentWhite() const;
		[[nodiscard]] std::string GetTimeSpentBlack() const;
	};
}

#endif //GOMOKU_CHESSCLOCK_H
