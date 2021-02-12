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

		static auto cmpIntializer(BoardState::Side side)
		{
			std::function<bool(int score1, int score2)> ret;

			if (side == BoardState::Side::White)
				ret = [](int left, int right){ return left > right; };
			else if (side == BoardState::Side::Black)
				ret =  [](int left, int right){ return left < right; };
			else
				throw std::runtime_error("wrong side in cmpIntializer");

			return ret;
		}

		std::function<bool(int score1, int score2)> score1BetterThenScore2;
		const Gomoku::BoardState &currentBoard;
	public:

		explicit IPlayer(BoardState::Side side, MakeMove_t MakeMove, const Gomoku::BoardState &realBoard)
		: side_{side}
		, MakeMove_{std::move(MakeMove)}
		, score1BetterThenScore2(cmpIntializer(side))
		, currentBoard(realBoard)
		{
		}
		virtual ~IPlayer() = default;
		virtual void YourTurn(int row, int col, const std::unordered_set<std::pair<int, int>, pairhash>& availableMoves) = 0;
		void NotYourTurn()
		{
			myMove = false;
		}
	
		virtual BoardState::MoveResult Ping() = 0;
	};


	std::unique_ptr<Gomoku::IPlayer> PlayerFactory(const std::string& name, Gomoku::BoardState::Side side, const Gomoku::MakeMove_t& MakeMove, const Gomoku::BoardState &bs);
}




#endif //GOMOKU_IPLAYER_H
