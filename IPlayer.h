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
	using MakeMove_t=std::function<void(int row, int col)>;

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
		virtual void Ping() = 0;
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
		void Ping() override;
	};

	class AI1 : public IPlayer
	{
	public:
		explicit AI1(BoardState::Side side, MakeMove_t MakeMove)
		: IPlayer(side, std::move(MakeMove))
		{}

		void YourTurn(int row, int col, const std::unordered_set<std::pair<int, int>, pairhash>& availableMoves) override
		{
			if (!availableMoves.empty())
				MakeMove_(availableMoves.begin()->first, availableMoves.begin()->second);
		}
		void Ping() override
		{

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
		void Ping() override
		{}

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
		void Ping() override
		{}

	};

	std::unique_ptr<Gomoku::IPlayer> PlayerFactory(const std::string& name, Gomoku::BoardState::Side side, const Gomoku::MakeMove_t& MakeMove);
}




#endif //GOMOKU_IPLAYER_H