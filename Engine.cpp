//
// Created by Lommy Reznak on 2/4/21.
//

#include "Engine.h"


int  Gomoku::Engine::internal_(const Gomoku::Board &bs)
{
	auto time1 = std::chrono::high_resolution_clock::now();

	int ret = 0;

	// Edge cases
	{
		// Game ended
		if (bs.GetLastMoveResult() == Board::MoveResult::Draw)
			return 0;
		if (bs.GetLastMoveResult() == Board::MoveResult::WhiteWin)
			return +100;
		if (bs.GetLastMoveResult() == Board::MoveResult::BlackWin)
			return -100;
	}

	{
		// completed fives
		auto b1 = bs.IsThereFigureOnBoard(Gomoku::Board::figure_five_w);
		auto b2 = bs.IsThereFigureOnBoard(Gomoku::Board::figure_five_b);

		if (b1) return +50;
		if (b2) return -50;
	}

	{
		// free fours
		auto b1 = bs.IsThereFigureOnBoard(Gomoku::Board::figure_free_four_w);
		auto b2 = bs.IsThereFigureOnBoard(Gomoku::Board::figure_free_four_b);

		// half free and flanked fours
		auto t1 = bs.CountHalfFreeFours(Board::Side::White);
		auto t2 = bs.CountHalfFreeFours(Board::Side::Black);

		if (bs.WhiteMove() && (b1 || t1))
			return +40;
		if (!bs.WhiteMove() && (b1 || t1))
			return -40;

		if (b1) return 30;
		if (b2) return -30;

		ret += (t1 - t2) * halfFreeFourCoef;
	}

	{
		// free threes
		auto t1 = bs.CountFreeThrees(Board::Side::White);
		auto t2 = bs.CountFreeThrees(Board::Side::Black);


		if (t1 && bs.WhiteMove()) return 15;
		if (t2 && !bs.WhiteMove()) return -15;

		if (t1 > 1 && t2 == 0) return 13;
		if (t2 > 1 && t1 == 0) return -13;

		ret += (t1 - t2) * freeThreeCoef;
	}
	{
		// half free and flanked threes
		auto t1 = bs.CountHalfFreeThrees(Board::Side::White);
		auto t2 = bs.CountHalfFreeThrees(Board::Side::Black);

		ret += (t1 - t2) * halfFreeThreeCoef;
	}

	{
		// Potential captures
		int potentalWhiteCaptures = 0;
		int potentalBlackCaptures = 0;
		const auto& avm = bs.GetAvailableMoves();
		for (const auto &move : avm)
		{
			bool t1 = bs.IsMoveCapture(move.first, move.second, 0b01);
			bool t2 = bs.IsMoveCapture(move.first, move.second, 0b10);

			if (t1 && t2)
			{
				if (bs.WhiteMove())
					potentalWhiteCaptures++;
				else
					potentalBlackCaptures++;
			}
			else if (t1)
				potentalWhiteCaptures++;
			else if (t2)
				potentalBlackCaptures++;
		}

		ret += (potentalWhiteCaptures - potentalBlackCaptures) * potentialCaptureCoef;
	}


	{
		// Captures
		auto t1 = bs.GetCapturePoints(Gomoku::Board::Side::White);
		auto t2 = bs.GetCapturePoints(Gomoku::Board::Side::Black);

		ret += (t1 - t2) * 	captureCoef;
	}


//	auto t2 = std::chrono::high_resolution_clock::now();
//	auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - time1 ).count();
//	ss << "ms elapsed: " << duration << std::endl;

//	std::cerr << ss.str();

	return ret;
}

int Gomoku::Engine::StaticPositionAnalize(const Gomoku::Board &bs)
{
	static std::unordered_map<Gomoku::Board, int> m;
	static std::atomic_int c = 0;

	{
		auto t = internal_(bs);

		return t;
	}

}
