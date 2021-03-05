//
// Created by Brazhenko Andrew on 30.01.2021.
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
	/// @brief "make move" callback passed to IPlayer object
	using MakeMove_t=std::function<Gomoku::Board::MoveResult (int row, int col)>;

	/// @brief interface class for gomoku42 player
	class IPlayer
	{
	protected:
		/// @brief side player plays for
		Board::Side side_;

		/// @brief make move function
		MakeMove_t MakeMove_;

		/// @brief flag
		bool myMove = false;

		/// @brief reference at current state of game
		const Gomoku::Board &currentBoard;
	public:
		/// @brief ctor
		/// @param side color player plays for
		/// @param MakeMove make move callback
		/// @param realBoard current game state
		explicit IPlayer(Board::Side side, MakeMove_t MakeMove, const Gomoku::Board &realBoard)
		: side_{ side }
		, MakeMove_{ std::move(MakeMove) }
		, currentBoard{ realBoard }
		{}

		virtual ~IPlayer() = default;
		/// @brief notifier about turn
		virtual void YourTurn() = 0;
		/// @brief function called every time in game loop. if move is prepared it calls MakeMove_()
		/// @return move result
		virtual Board::MoveResult Ping() = 0;
	};

	std::unique_ptr<Gomoku::IPlayer> PlayerFactory(
			const std::string& name,
			Gomoku::Board::Side side,
			const Gomoku::MakeMove_t& MakeMove,
			const Gomoku::Board &bs,
			bool yourTurn);
}




#endif //GOMOKU_IPLAYER_H
