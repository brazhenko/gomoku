//
// Created by 17641238 on 19.01.2021.
//

#ifndef GOMOKU_BOARD_H
#define GOMOKU_BOARD_H

#include <vector>
#include <cstdint>
#include <array>
#include <unordered_set>
#include <bitset>

namespace Gomoku
{
//	class Board
//	{
//	public:
//		int arr[19][19]{0};
//		[[nodiscard]] bool isFree(int col, int row) const;
//	};

	class BoardState
	{
		using board_line=std::bitset<19*2>;

		const board_line figure_five_w { 0b0101010101 };				// XXXXX

		const board_line figure_free_three1_w { 0b00'010101'00 };	// _XXX_
		const board_line figure_free_three2_w { 0b00'01000101'00 };	// _X_XX_
		const board_line figure_free_three3_w { 0b00'01010001'00 };	// _XX_X_

		const board_line figure_free_three2 {0b010101};


		// array of rows, board_[1][2] == board["c2"]
		std::array<board_line, 19> board_{};

		board_line movePattern{0b01};

		std::vector<std::pair<int, int>> moves_;

	public:
		[[nodiscard]] size_t hash() const
		{
			std::size_t seed = board_.size();

			for(const auto& i : board_)
				seed ^= std::hash<board_line>{}(i);

			return seed;
		}

		bool MakeMove(int row, int col)
		{
			if (!CanMakeMove(row, col)) return false;

			// Add to move history
			moves_.emplace_back(row, col);
			// Put stone on board
			board_[row] |= (movePattern << (col * 2));
			// Change move
			movePattern ^= 0b11;

			return true;
		}

		[[nodiscard]] bool WhiteMove() const
		{
			return movePattern == 0b01;
		}

		[[nodiscard]] bool CanMakeMove(int row, int col) const
		{
			std::bitset<38> tmp;
			tmp[col * 2] = true;
			tmp[col * 2 + 1] = true;

			// Check if cell free
			if ((board_[row] & tmp).any())
				return false;
			return true;
		}

		bool TakeBackMove()
		{
			if (moves_.empty()) return false;

			const auto &lastMove = moves_.back();

			// Clear board from one stone.
			board_[lastMove.first][lastMove.second * 2] = false;
			board_[lastMove.first][lastMove.second * 2 + 1] = false;

			moves_.pop_back();

			return true;
		}

		BoardState();
		explicit BoardState(const std::vector<std::pair<int, int>()> &moves);

		std::unordered_set<BoardState> meth();
	};
}

namespace std {
	template <>
	struct hash<Gomoku::BoardState> {
		std::size_t operator()(const Gomoku::BoardState& k) const {
			return k.hash();
		}
	};
}



#endif //GOMOKU_BOARD_H
