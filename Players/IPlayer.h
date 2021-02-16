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
	using MakeMove_t=std::function<Gomoku::Board::MoveResult(int row, int col)>;

	class IPlayer
	{
	protected:
		Board::Side side_;
		MakeMove_t MakeMove_;
		bool myMove = false;

		static auto cmpIntializer(Board::Side side)
		{
			std::function<bool(int score1, int score2)> ret;

			if (side == Board::Side::White)
				ret = [](int left, int right){ return left > right; };
			else if (side == Board::Side::Black)
				ret =  [](int left, int right){ return left < right; };
			else
				throw std::runtime_error("wrong side in cmpIntializer");

			return ret;
		}

		std::function<bool(int score1, int score2)> score1BetterThenScore2;
		const Gomoku::Board &currentBoard;
	public:

		explicit IPlayer(Board::Side side, MakeMove_t MakeMove, const Gomoku::Board &realBoard)
		: side_{side}
		, MakeMove_{std::move(MakeMove)}
		, score1BetterThenScore2(cmpIntializer(side))
		, currentBoard(realBoard)
		{
		}
		virtual ~IPlayer() = default;
		virtual void YourTurn(int row, int col, const std::vector<std::pair<int, int>>& availableMoves) = 0;
		void NotYourTurn()
		{
			myMove = false;
		}
	
		virtual Board::MoveResult Ping() = 0;
	};


	std::unique_ptr<Gomoku::IPlayer> PlayerFactory(const std::string& name, Gomoku::Board::Side side, const Gomoku::MakeMove_t& MakeMove, const Gomoku::Board &bs);
}




#endif //GOMOKU_IPLAYER_H
