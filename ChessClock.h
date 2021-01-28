//
// Created by Lommy Reznak on 1/28/21.
//

#ifndef GOMOKU_CHESSCLOCK_H
#define GOMOKU_CHESSCLOCK_H

#include <string>
#include <sstream>
#include <iostream>
#include <iomanip>

namespace Gomoku
{
	class ChessClock
	{
		bool WhiteMove;
		bool PauseOn;

		std::chrono::system_clock::time_point startWhite{};
		std::chrono::system_clock::time_point startBlack{};

		//std::chrono::duration<double>
		std::chrono::milliseconds whiteTimeLeft;
		std::chrono::milliseconds blackTimeLeft;

	public:
		ChessClock() = delete;

		ChessClock(int whiteSeconds, int blackSeconds)
				: whiteTimeLeft{whiteSeconds * 1000 }
				, blackTimeLeft{blackSeconds * 1000 }
				, WhiteMove{ true }
				, PauseOn { true }
		{}

		void Start()
		{
			startWhite = std::chrono::system_clock::now();
		}

		void Pause()
		{
			PauseOn = true;
		}

		void Continue()
		{
			PauseOn = false;
		}

		void Stop()
		{

		}

		void ChangeMove()
		{
			if (WhiteMove)
				whiteTimeLeft = std::chrono::duration_cast<std::chrono::milliseconds>(whiteTimeLeft - (std::chrono::system_clock::now() - startWhite));
			else
				blackTimeLeft = std::chrono::duration_cast<std::chrono::milliseconds>(blackTimeLeft - (std::chrono::system_clock::now() - startBlack));

			WhiteMove ^= true;
		}

		[[nodiscard]] std::string GetTimeLeftWhite()
		{
			std::stringstream ss;

			auto nw = std::chrono::system_clock::now();

			if (WhiteMove)
				startBlack = nw;

			if (whiteTimeLeft <= (nw - startWhite))
				return "00:00";

			std::chrono::duration<double> diff = whiteTimeLeft - (nw - startWhite);


			const auto hours = std::chrono::duration_cast<std::chrono::hours>(diff).count();
			if (hours)
				ss << std::setw(2) << std::setfill('0') << hours << ":";

			const auto minutes = std::chrono::duration_cast<std::chrono::minutes>(diff).count();

			ss << std::setw(2) << std::setfill('0') << minutes % 60 << ":";
			ss << std::setw(2) << std::setfill('0') << std::chrono::duration_cast<std::chrono::seconds>(diff).count() % 60;

			if (minutes == 0)
				ss << "." << std::chrono::duration_cast<std::chrono::milliseconds>(diff).count() % 1000 / 100;

			return ss.str();
		}

		[[nodiscard]] std::string GetTimeLeftBlack()
		{
			std::stringstream ss;

			auto nw = std::chrono::system_clock::now();

			if (!WhiteMove)
				startWhite = nw;

			if (blackTimeLeft <= (nw - startBlack))
				return "00:00";

			std::chrono::duration<double> diff = blackTimeLeft - (nw - startBlack);

			const auto hours = std::chrono::duration_cast<std::chrono::hours>(diff).count();
			if (hours)
				ss << std::setw(2) << std::setfill('0') <<  hours << ":";

			const auto minutes = std::chrono::duration_cast<std::chrono::minutes>(diff).count();

			ss << std::setw(2) << std::setfill('0') << minutes % 60 << ":";

			ss << std::setw(2) << std::setfill('0') << std::chrono::duration_cast<std::chrono::seconds>(diff).count() % 60;

			if (minutes == 0)
				ss << "." << std::chrono::duration_cast<std::chrono::milliseconds>(diff).count() % 1000 / 100;

			return ss.str();
		}
	};
}




#endif //GOMOKU_CHESSCLOCK_H
