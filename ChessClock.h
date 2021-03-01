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
    /// @brief Gomoku::ChessClock is a simple implementation of
    /// two-side chess clock that simulates real chess clock :)
	class ChessClock
	{
		bool WhiteMove_ = true;
		bool PauseOn_ = true;

		std::chrono::system_clock::time_point startWhite_{};
		std::chrono::system_clock::time_point startBlack_{};

		std::chrono::milliseconds whiteTimeLeft_{};
		std::chrono::milliseconds blackTimeLeft_{};

		std::chrono::milliseconds whiteTimeSpentForLastMove_{};
		std::chrono::milliseconds blackTimeSpentForLastMove_{};

        static std::string DurationToString(const std::chrono::duration<double> &duration);
	public:
		ChessClock() = delete;
		/// @brief ctor fo ChessClock
		/// @param whiteSeconds amount of time for white player in seconds
		/// @param blackSeconds amount of time for black player in seconds
		/// @param whiteMove `true` if white player starts, `black` if black starts
		ChessClock(int whiteSeconds, int blackSeconds, bool whiteMove=true);

		/// @brief start countdown of player who's turn was set
		void Start();

		/// @brief pauses countdown of both players
		void Pause();

		/// @brief continues countdown of player who's turn was set, has equal effect as Start(),
		/// but duplicated for right semantics
		void Continue();

		/// @brief Stops the clock, zeroes time-left-vars
		void Stop();

		/// @brief Changes move, countdown swaps to opposite player
		void ChangeMove();

		/// @brief Checks if white player has time left
		/// @return `true` if left, `false` if not
		[[nodiscard]] bool WhiteTimeLeft() const;

        /// @brief Checks if black player has time left
        /// @return `true` if left, `false` if not
		[[nodiscard]] bool BlackTimeLeft() const;

		/// @brief String representation of white player time
		/// @return string
		[[nodiscard]] std::string GetTimeLeftWhite() const;

		/// @brief String representation of black player time
		/// @return string
		[[nodiscard]] std::string GetTimeLeftBlack() const;

		/// @brief String representation of white player spent for last move
		/// @return string
		[[nodiscard]] std::string GetTimeSpentWhite() const;

		/// @brief String representation of black player spent for last move
		/// @return string
		[[nodiscard]] std::string GetTimeSpentBlack() const;
	};
}

#endif //GOMOKU_CHESSCLOCK_H
