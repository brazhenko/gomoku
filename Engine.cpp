//
// Created by Lommy Reznak on 2/4/21.
//

#include "Engine.h"

int Gomoku::Engine::StaticPositionAnalize(const Gomoku::BoardState &bs)
{
	int ret = 0;

	// Edge cases

	// Game ended (fifth)
	if (bs.GetLastMoveResult() == BoardState::MoveResult::Draw)
		return 0;
	if (bs.GetLastMoveResult() == BoardState::MoveResult::WhiteWin)
		return +100;
	if (bs.GetLastMoveResult() == BoardState::MoveResult::BlackWin)
		return -100;

	{
		// Fours
		auto b1 = bs.IsThereFigureOnBoard(Gomoku::BoardState::figure_four_w);
		auto b2 = bs.IsThereFigureOnBoard(Gomoku::BoardState::figure_four_b);
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
		// Threes




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
		auto t1 = bs.GetCapturePoints(Gomoku::BoardState::Side::White);
		auto t2 = bs.GetCapturePoints(Gomoku::BoardState::Side::Black);

		ret += (t1 - t2) * 	captureCoef;
	}


    return ret;
}
