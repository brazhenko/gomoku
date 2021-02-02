//
// Created by 17641238 on 30.01.2021.
//

#ifndef GOMOKU_IPLAYER_H
#define GOMOKU_IPLAYER_H

#include <unordered_set>
#include <memory>
#include "Board.h"
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
		virtual void NotYourTurn() = 0;
	
		virtual BoardState::MoveResult Ping() = 0;
	};



	class Human : public IPlayer
	{
		std::unordered_set<std::pair<int, int>, pairhash> availableMoves_;
	public:
		explicit Human(BoardState::Side side, MakeMove_t MakeMove)
		: IPlayer(side, std::move(MakeMove))
		{}

		void YourTurn(int row, int col, const std::unordered_set<std::pair<int, int>, pairhash>& availableMoves) override
		{
			myMove = true;
			availableMoves_ = availableMoves;
		}
		void NotYourTurn() override
		{
			myMove = false;
		}


		BoardState::MoveResult Ping() override;
	};

	class AI1 : public IPlayer
	{
		std::unordered_set<std::pair<int, int>, pairhash> availableMoves_;
	public:
		explicit AI1(BoardState::Side side, MakeMove_t MakeMove)
		: IPlayer(side, std::move(MakeMove))
		{}

		void YourTurn(int row, int col, const std::unordered_set<std::pair<int, int>, pairhash>& availableMoves) override
		{
			myMove = true;
			availableMoves_ = availableMoves;
		}

		void NotYourTurn() override
		{
			myMove = false;
		}
		BoardState::MoveResult Ping() override
		{
			if (!myMove) return {};

			if (!availableMoves_.empty())
			{
				myMove = false;
				return MakeMove_(availableMoves_.begin()->first, availableMoves_.begin()->second);
			}


			return {};
		}
	};

	class AI2 : public IPlayer
	{
	public:
		explicit AI2(BoardState::Side side, MakeMove_t MakeMove)
		: IPlayer(side, std::move(MakeMove))
		{}

		void YourTurn(int row, int col, const std::unordered_set<std::pair<int, int>, pairhash>& availableMoves) override
		{

		}
		void NotYourTurn() override
		{
			myMove = false;
		}
		BoardState::MoveResult Ping() override
		{
			return {};
		}

	};

	class AI3 : public IPlayer
	{
	public:
		explicit AI3(BoardState::Side side, MakeMove_t MakeMove)
		: IPlayer(side, std::move(MakeMove))
		{}

		void YourTurn(int row, int col, const std::unordered_set<std::pair<int, int>, pairhash>& availableMoves) override
		{

		}
		void NotYourTurn() override
		{
			myMove = false;
		}
		BoardState::MoveResult Ping() override
		{
			return {};
		}

	};

	std::unique_ptr<Gomoku::IPlayer> PlayerFactory(const std::string& name, Gomoku::BoardState::Side side, const Gomoku::MakeMove_t& MakeMove);
}




#endif //GOMOKU_IPLAYER_H
