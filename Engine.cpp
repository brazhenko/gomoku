//
// Created by Lommy Reznak on 2/4/21.
//

#include "Engine.h"


int  Gomoku::Engine::internal_(const Gomoku::Board &bs)
{
	auto time1 = std::chrono::high_resolution_clock::now();

	static int c=0;

	int ret = 0;

	std::stringstream ss;
	ss << "////eval:" << c++ << "////" << std::endl;

	// Edge cases

	// Game ended (fifth)
	if (bs.GetLastMoveResult() == Board::MoveResult::Draw)
	{
		ss << "Draw" << std::endl;
		return 0;
	}
	if (bs.GetLastMoveResult() == Board::MoveResult::WhiteWin)
		return +100;
	if (bs.GetLastMoveResult() == Board::MoveResult::BlackWin)
		return -100;

	{
		// completed fives
		auto b1 = bs.IsThereFigureOnBoard(Gomoku::Board::figure_five_w);
		auto b2 = bs.IsThereFigureOnBoard(Gomoku::Board::figure_five_b);

		if (b1) return +20;
		if (b2) return -20;
	}

	{
		// free fours
		auto b1 = bs.IsThereFigureOnBoard(Gomoku::Board::figure_free_four_w);
		auto b2 = bs.IsThereFigureOnBoard(Gomoku::Board::figure_free_four_b);
		if (b1 && b2)
		{
			if (bs.WhiteMove())
				return +10;
			return -10;
		}
		if (b1) return +10;
		if (b2) return -10;
	}

	{
		// half free and flanked fours
		auto t1 = bs.CountHalfFreeFours(Board::Side::White);
		auto t2 = bs.CountHalfFreeFours(Board::Side::Black);

		if (bs.WhiteMove() && t1)
			return +10;
		if (!bs.WhiteMove() && t2)
			return -10;

		ss << "half 4, white: " << t1 << " , black: " << t2 << std::endl;
		ret += (t1 - t2) * halfFreeFourCoef;
	}

	{
		// free threes
		auto t1 = bs.CountFreeThrees(Board::Side::White);
		auto t2 = bs.CountFreeThrees(Board::Side::Black);

		if (t1 >= 2) return 8;
		if (t2 >= 2) return -8;

		if (t1 && bs.WhiteMove()) return 7;
		if (t2 && !bs.WhiteMove()) return -7;

		ss << "free 3, white: " << t1 << " , black: " << t2 << std::endl;
		ret += (t1 - t2) * freeThreeCoef;
	}
	{
		// half free and flanked threes
		auto t1 = bs.CountHalfFreeThrees(Board::Side::White);
		auto t2 = bs.CountHalfFreeThrees(Board::Side::Black);

		ss << "half 3, white: " << t1 << " , black: " << t2 << std::endl;
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

		ss << "potential captures, white: " << potentalWhiteCaptures << " , black: " << potentalBlackCaptures << std::endl;
		ret += (potentalWhiteCaptures - potentalBlackCaptures) * potentialCaptureCoef;
	}


	{
		// Captures
		auto t1 = bs.GetCapturePoints(Gomoku::Board::Side::White);
		auto t2 = bs.GetCapturePoints(Gomoku::Board::Side::Black);

		ss << "real captures, white: " << t1 << " , black: " << t2 << std::endl;
		
		ret += (t1 - t2) * 	captureCoef;
	}


	auto t2 = std::chrono::high_resolution_clock::now();
	auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - time1 ).count();

	ss << "ms elapsed: " << duration << std::endl;

	std::cerr << ss.str();

	return ret;
}

int Gomoku::Engine::StaticPositionAnalize(const Gomoku::Board &bs)
{
	static std::unordered_map<Gomoku::Board, int> m;

	if (m.find(bs) == m.end())
	{
		auto t = internal_(bs);

		m[bs] = t;
		return t;
	}

	return m.at(bs);
}
