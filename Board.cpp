//
// Created by 17641238 on 19.01.2021.
//

#include "Board.h"

bool Gomoku::Board::isFree(int col, int row) const
{
	return !this->arr[col][row];
}
