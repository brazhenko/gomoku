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
		BoardState::MoveResult Ping() override;
	};

	class AI1 : public IPlayer
	{
		std::unordered_set<std::pair<int, int>, pairhash> availableMoves_;
	public:
		explicit AI1(BoardState::Side side, MakeMove_t MakeMove)
		: IPlayer(side, std::move(MakeMove))
		{}

		struct CalcNode
		{
			CalcNode() = delete;
			explicit CalcNode(const BoardState& bs)
			{
				state_ = bs;
			}

			int positionScore = 0;
			BoardState state_;
			std::vector<std::unique_ptr<CalcNode>> children;
		};

		std::unique_ptr<CalcNode> tree = std::make_unique<CalcNode>(BoardState{});
		BoardState currentBoard{};

		bool FindNext()
		{
			for (int i = 0; i < tree->children.size(); i++)
			{
				if (tree->children[i]->state_ == currentBoard)
				{
					tree = std::move(tree->children[i]);
					return true;
				}
			}
			return false;
		}

		void YourTurn(int row, int col, const std::unordered_set<std::pair<int, int>, pairhash>& availableMoves) override
		{
			myMove = true;
			availableMoves_ = availableMoves;

			currentBoard.MakeMove(row, col);
			if (!FindNext())
				tree = std::make_unique<CalcNode>(currentBoard);

			for (const auto &move: availableMoves)
			{
				auto copy = currentBoard;
				currentBoard.MakeMove(move.first, move.second);
				tree->children.emplace_back(std::make_unique<CalcNode>(std::move(copy)));
			}

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
		BoardState::MoveResult Ping() override
		{
			return {};
		}

	};

	std::unique_ptr<Gomoku::IPlayer> PlayerFactory(const std::string& name, Gomoku::BoardState::Side side, const Gomoku::MakeMove_t& MakeMove);
}




#endif //GOMOKU_IPLAYER_H
