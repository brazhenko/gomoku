//
// Created by 17641238 on 18.01.2021.
//

#ifndef GOMOKU_GOMOKUDRAW_H
#define GOMOKU_GOMOKUDRAW_H

#include "Game.h"
#include "ChessClock.h"
#include "PGNGame.h"
#include "imgui.h"
#include "imfilebrowser.hpp"
#include <optional>

namespace GomokuDraw
{
	bool Init();
	bool Go();

	void DrawSome();

	std::pair<int, int> MouseCoordinatesToStonePosition(float x_, float y_);
	std::pair<float, float> StonePositionToPrintCoorinates(std::pair<int, int> stone);

	bool MouseInsideBoard();

	void ForbiddenCursor();
	void DrawStone(float x, float y, int type);

	void DrawStones(const Gomoku::BoardState &bs);

	void DrawPlayer(const Gomoku::Game &game, const std::string &timeLeft, int lastMove, bool isDisable, bool isWhite);
	void DrawSteps(Gomoku::Game &game);
	void DrawButtons(Gomoku::Game &game, Gomoku::ChessClock &clock);

	void DrawFilesButtons(Gomoku::Game &game);
	void DrawGameMenu(Gomoku::Game &game, Gomoku::ChessClock &clock);

	void DrawGameMoves(const Gomoku::BoardState &bs);

	void Render();
	void Cleanup();
};

#endif //GOMOKU_GOMOKUDRAW_H
