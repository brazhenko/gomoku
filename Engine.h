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

		static constexpr int halfFreeFourCoef = 1;
		static constexpr int potentialCaptureCoef = 1;
		static constexpr int captureCoef = 3;
		static constexpr int freeThreeCoef = 2;
		static constexpr int halfFreeThreeCoef = 1;


		static int internal_(const Gomoku::Board &bs);
		static int StaticPositionAnalize(const Gomoku::Board& bs);
	};
}




#endif //GOMOKU_ENGINE_H
