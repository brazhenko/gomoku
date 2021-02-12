//
// Created by 17641238 on 30.01.2021.
//

#include "IPlayer.h"
#include "AI1.h"
#include "AI2.h"
#include "Human.h"

#include <string>

std::unique_ptr<Gomoku::IPlayer> Gomoku::PlayerFactory(
		const std::string& name,
		Gomoku::BoardState::Side side,
		const Gomoku::MakeMove_t& MakeMove,
		const Gomoku::BoardState &bs
		)
{
	if (name == "Human")
		return std::make_unique<Gomoku::Human>(side, MakeMove, bs);
	if (name == "AI1")
		return std::make_unique<Gomoku::AI1>(side, MakeMove, bs);
	if (name == "AI2")
		return std::make_unique<Gomoku::AI2>(side, MakeMove, bs);
//	if (name == "AI3")
//		return std::make_unique<Gomoku::AI3>(side, MakeMove);

	return nullptr;
}


