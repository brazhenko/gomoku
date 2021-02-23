//
// Created by 17641238 on 30.01.2021.
//

#include "IPlayer.h"
#include "Human.h"
#include "AI1.h"
#include "AI2.h"

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
		return std::make_unique<Gomoku::AI1>(side, MakeMove, bs, yourTurn);
	if (name == "AI2")
		return std::make_unique<Gomoku::AI2>(side, MakeMove, bs);
//	if (name == "AI3")
//		return std::make_unique<Gomoku::AI3>(side, MakeMove);

	return nullptr;
}


