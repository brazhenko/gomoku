//
// Created by Brazhenko Andrew on 18.01.2021.
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

	static std::vector<std::string> messages; 
	std::pair<int, int> MouseCoordinatesToStonePosition(float x_, float y_);
	std::pair<float, float> StonePositionToPrintCoorinates(std::pair<int, int> stone);

	void Render();

	bool MouseInsideBoard();
	void ForbiddenCursor();
	void DrawStone(float x, float y, int type);
	void DrawStones(const Gomoku::Board &bs);

	void DrawGameMenu(Gomoku::Game &game);
	void DrawPlayer(const Gomoku::Game &game, bool isWhite);
	void DrawButtons(Gomoku::Game &game);
	void DrawFilesButtons(Gomoku::Game &game);

	void DrawGameMoves(const Gomoku::Board &bs);

	void Cleanup();

	void PrintMessage(std::string);
};

#endif //GOMOKU_GOMOKUDRAW_H
