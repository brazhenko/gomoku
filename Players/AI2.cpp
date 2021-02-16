//
// Created by Lommy Reznak on 2/6/21.
//

#include "AI2.h"

void Gomoku::AI2::YourTurn(int row, int col, const std::vector<std::pair<int, int>> &availableMoves)
{

}

Gomoku::Board::MoveResult Gomoku::AI2::Ping()
{
	// если есть ход, чтобы его сделать:
	// 		MakeMove(row, col)

	return Gomoku::Board::MoveResult::Draw;
}
