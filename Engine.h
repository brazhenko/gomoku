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
		static constexpr int potentialCaptureCoef = 1;
		static constexpr int captureCoef = 1;
		static constexpr int freeThreeCoef = 1;

		static int StaticPositionAnalize(const Gomoku::BoardState& bs);
	};
}




#endif //GOMOKU_ENGINE_H
