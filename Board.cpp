//
// Created by 17641238 on 19.01.2021.
//

#include "Board.h"
#include <vector>


Gomoku::BoardState::BoardState()
{
	for (int i = 0; i < 19; i++)
		for (int j = 0; j < 19; j++)
		{
			this->free_cells.emplace(i, j);
			this->available_moves.emplace(i, j);
		}


}


Gomoku::BoardState::BoardState(const std::vector<std::pair<int, int>> &moves) {
	BoardState();

	for (const auto &move : moves)
		if (!this->MakeMove(move.first, move.second))
		{
			std::stringstream ss;

			ss << "Trying to construct board with wrong move: " << move.first << " " << move.second;
			throw std::runtime_error(ss.str());
		}

}

