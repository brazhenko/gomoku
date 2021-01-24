//
// Created by 17641238 on 19.01.2021.
//

#include "Board.h"

bool Gomoku::Board::isFree(int col, int row) const
{
	return !this->arr[col][row];
}

Gomoku::BoardState::BoardState()
{
	this->rows.fill(0);
	this->cols.fill(0);
	this->downLines.fill(0);
	this->upLines.fill(0);

}

std::unordered_set<Gomoku::BoardState> Gomoku::BoardState::meth()
{
	return std::unordered_set<BoardState>();
}
