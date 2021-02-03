//
// Created by 17641238 on 30.01.2021.
//

#include "IPlayer.h"
#include "AI1.h"
#include "GomokuDraw.h"
#include "Board.h"
#include <string>

std::unique_ptr<Gomoku::IPlayer> Gomoku::PlayerFactory(
		const std::string& name,
		Gomoku::BoardState::Side side,
		const Gomoku::MakeMove_t& MakeMove
		)
{
	if (name == "Human")
		return std::make_unique<Gomoku::Human>(side, MakeMove);
	if (name == "AI1")
		return std::make_unique<Gomoku::AI1>(side, MakeMove);
	if (name == "AI2")
		return std::make_unique<Gomoku::AI2>(side, MakeMove);
	if (name == "AI3")
		return std::make_unique<Gomoku::AI3>(side, MakeMove);

	return nullptr;
}

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
