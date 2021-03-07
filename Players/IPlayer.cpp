//
// Created by Brazhenko Andrew on 30.01.2021.
//

#include "IPlayer.h"
#include "Human.h"

#include "AI1.h"

std::unique_ptr<Gomoku::IPlayer> Gomoku::PlayerFactory(
		const std::string& name,
		Gomoku::Board::Side side,
		const Gomoku::MakeMove_t& MakeMove,
		const Gomoku::Board &bs,
		bool yourTurn)
{
	if (name == "Human")
		return std::make_unique<Gomoku::Human>(side, MakeMove, bs, yourTurn);
	if (name == "AI1")
		return std::make_unique<Gomoku::AI1>(side, MakeMove, bs, 5, 5, 1'000, false, yourTurn);
	if (name == "AI_Debug")
		return std::make_unique<Gomoku::AI1>(side, MakeMove, bs, 5, 3, 5'000, true, yourTurn);;
	if (name == "AI_Depth10")
		return std::make_unique<Gomoku::AI1>(side, MakeMove, bs, 10, 2, 5'000, true, yourTurn);
	if (name == "AI_Easy")
		return std::make_unique<Gomoku::AI1>(side, MakeMove, bs, 2, 3, 1'000, false, yourTurn);

	return nullptr;
}


