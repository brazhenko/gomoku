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


	std::optional<Gomoku::Cell> HandleBoardInteraction();
	void ForbiddenCursor();
	void DrawStone(float x, float y, int type);

	void Render();
	void Cleanup();
};

#endif //GOMOKU_GOMOKUDRAW_H
