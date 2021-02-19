//
// Created by Lommy Reznak on 2/3/21.
//

#ifndef GOMOKU_AI1_H
#define GOMOKU_AI1_H

#include <utility>

#include "IPlayer.h"

namespace Gomoku
{
	class AI1 : public IPlayer
	{
//		std::vector<Board::pcell> availableMoves_;

	public:
		static auto lessIntializer(Board::Side side)
		{
			std::function<bool(int score1, int score2)> ret;

			if (side == Board::Side::White)
				ret = std::less<int>{};
			else if (side == Board::Side::Black)
				ret = std::greater<int>{};
			else
				throw std::runtime_error("wrong side in lessIntializer");

			return ret;
		}
		static auto greaterIntializer(Board::Side side)
		{
			std::function<bool(int score1, int score2)> ret;

			if (side == Board::Side::White)
				ret = std::greater<int>{};
			else if (side == Board::Side::Black)
				ret = std::less<int>{};
			else
				throw std::runtime_error("wrong side in greaterIntializer");

			return ret;
		}

		static auto minInitializer(Board::Side side)
		{
			if (Board::Side::White == side)
				return -100;
			else if (Board::Side::Black == side)
				return 100;

			throw std::runtime_error("wrong side in minInitializer");
		}
		static auto maxInitializer(Board::Side side)
		{
			if (Board::Side::White == side)
				return 100;
			else if (Board::Side::Black == side)
				return -100;

			throw std::runtime_error("wrong side in minInitializer");
		}


		std::function<bool(int score1, int score2)> score1BetterThenScore2;
		std::function<bool(int score1, int score2)> score1WorseThenScore2;

		int Min, Max;

		explicit AI1(Board::Side side, MakeMove_t MakeMove, const Gomoku::Board &realBoard)
				: IPlayer(side, std::move(MakeMove), realBoard)
				, score1BetterThenScore2(greaterIntializer(side))
				, score1WorseThenScore2(lessIntializer(side))
				, Min(minInitializer(side))
				, Max(minInitializer(side))
		{}


		struct CalcNode
		{
			CalcNode() = delete;
			explicit CalcNode(const Board& bs, std::weak_ptr<CalcNode> parent)
			: parent_(std::move(parent))
			{
				state_ = bs;
			}
			explicit CalcNode(Board&& bs, std::weak_ptr<CalcNode> parent)
			: state_(bs)
			, parent_(std::move(parent))
			{}

			int positionScore = 0;
			Board state_;

            const std::weak_ptr<CalcNode> parent_;
			std::vector<std::shared_ptr<CalcNode>> children_;
		};

		std::shared_ptr<CalcNode> tree = std::make_shared<CalcNode>(Board{}, std::weak_ptr<CalcNode>());

		bool FindNext();

		Gomoku::Board::pcell nextMove;

		void YourTurn(int row, int col, const std::vector<std::pair<int, int>>& availableMoves) override;

		Board::MoveResult Ping() override;
	};
}




#endif //GOMOKU_AI1_H
