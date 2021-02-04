//
// Created by Lommy Reznak on 2/4/21.
//

#ifndef GOMOKU_ENGINE_H
#define GOMOKU_ENGINE_H

#include "Board.h"

namespace Gomoku
{
	class Engine {
	public:
		std::unordered_map<Gomoku::BoardState, int> m;

		int StaticPositionAnalize(const Gomoku::BoardState& bs);
	};
}




#endif //GOMOKU_ENGINE_H
