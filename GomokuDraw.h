//
// Created by 17641238 on 18.01.2021.
//

#ifndef GOMOKU_GOMOKUDRAW_H
#define GOMOKU_GOMOKUDRAW_H

#include "Cell.h"
#include <optional>

namespace GomokuDraw
{
	bool Init();
	bool Go();

	void DrawSome();


	int HandleWindowClick();
	std::pair<int, int> MouseCoordinatesToStonePosition(float x_, float y_);
	std::pair<float, float> StonePositionToPrintCoorinates(std::pair<int, int> stone);

	bool MouseInsideBoard();

	std::optional<Gomoku::Cell> HandleBoardInteraction();
	void ForbiddenCursor();
	void DrawStone(float x, float y, int type);

	void Render();
	void Cleanup();
};

#endif //GOMOKU_GOMOKUDRAW_H
