//
// Created by Lommy Reznak on 2/4/21.
//

#include "Human.h"

#include "../GomokuDraw.h"

Gomoku::BoardState::MoveResult Gomoku::Human::Ping()
{
	if (!myMove) return Gomoku::BoardState::MoveResult::Default;

	if (GomokuDraw::MouseInsideBoard()) // && !game.fileDialogGame.IsOpened() && !game.fileDialogBoardPos.IsOpened())
	{
		const auto &mPt = ImGui::GetMousePos();

		auto stone = GomokuDraw::MouseCoordinatesToStonePosition(mPt.x, mPt.y);

		if (availableMoves_.find({stone.first, stone.second}) != availableMoves_.end())
		{
			auto placeToDraw = GomokuDraw::StonePositionToPrintCoorinates(stone);

			GomokuDraw::DrawStone(placeToDraw.first, placeToDraw.second, side_ == Gomoku::BoardState::Side::White ? 1 : 3);

			if (ImGui::IsMouseClicked(0))
			{
				myMove = false;
				availableMoves_ = {};
				return MakeMove_(stone.first, stone.second);
			}
		}
		else
			GomokuDraw::ForbiddenCursor();
	}
	return Gomoku::BoardState::MoveResult::Default;
}