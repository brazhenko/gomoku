//
// Created by Lommy Reznak on 2/4/21.
//

#include "Engine.h"

int Gomoku::Engine::StaticPositionAnalize(const Gomoku::BoardState &bs)
{
	int ret = 0;

	// Edge cases
	if (bs.GetLastMoveResult() == BoardState::MoveResult::Draw)
		return 0;
	if (bs.GetLastMoveResult() == BoardState::MoveResult::WhiteWin)
		return +100;
	if (bs.GetLastMoveResult() == BoardState::MoveResult::BlackWin)
		return -100;


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




    return ret;
}
