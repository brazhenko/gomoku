//
// Created by Lommy Reznak on 2/4/21.
//

#include "Engine.h"

int Gomoku::Engine::StaticPositionAnalize(const Gomoku::BoardState &bs)
{
	auto t = this->m.find(bs);
	if (t != this->m.end())
		return t->second;

	int ret = 0;

	// Edge cases
	
	auto b1 = bs.IsThereFigureOnBoard(Gomoku::BoardState::figure_four_w);
	auto b2 = bs.IsThereFigureOnBoard(Gomoku::BoardState::figure_four_b);

	if (b1 != b2)
	{
//		if ()
	}

	return ret;
}
