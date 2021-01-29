//
// Created by 17641238 on 18.01.2021.
//

#ifndef GOMOKU_GOMOKUDRAW_H
#define GOMOKU_GOMOKUDRAW_H

#include "Game.h"
#include "ChessClock.h"
#include "Cell.h"
#include "PGNGame.h"
#include "imgui.h"
#include "imfilebrowser.hpp"
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

	void DrawPlayer(const Gomoku::Game &game, const std::string &timeLeft, int lastMove, bool isDisable, bool isWhite);
	void DrawSteps(Gomoku::Game &game);
	void DrawButtons(Gomoku::Game &game, Gomoku::ChessClock &clock);
	void DrawCheckboxes();
	void DrawFilesButtons(Gomoku::Game &game, ImGui::FileBrowser &fileDialog);
	void DrawGameMenu(Gomoku::Game &game, Gomoku::ChessClock &clock, ImGui::FileBrowser &fileDialog);
};

#endif //GOMOKU_GOMOKUDRAW_H
