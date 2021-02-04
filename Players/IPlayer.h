//
// Created by 17641238 on 30.01.2021.
//

#ifndef GOMOKU_IPLAYER_H
#define GOMOKU_IPLAYER_H

#include <unordered_set>
#include <memory>
#include "../Board.h"
#include <functional>
#include <utility>

namespace Gomoku
{
	using MakeMove_t=std::function<Gomoku::BoardState::MoveResult(int row, int col)>;

	class IPlayer
	{
	protected:
		BoardState::Side side_;
		MakeMove_t MakeMove_;
		bool myMove = false;
	public:

		explicit IPlayer(BoardState::Side side, MakeMove_t MakeMove)
		: side_{side}
		, MakeMove_{std::move(MakeMove)}
		{}
		virtual ~IPlayer() = default;
		virtual void YourTurn(int row, int col, const std::unordered_set<std::pair<int, int>, pairhash>& availableMoves) = 0;
		void NotYourTurn()
		{
			myMove = false;
		}
	
		virtual BoardState::MoveResult Ping() = 0;
	};


	std::unique_ptr<Gomoku::IPlayer> PlayerFactory(const std::string& name, Gomoku::BoardState::Side side, const Gomoku::MakeMove_t& MakeMove);
}




#endif //GOMOKU_IPLAYER_H
