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
#include <iostream>
#include <sstream>

struct pairhash {
public:
	template <typename T, typename U>
	std::size_t operator()(const std::pair<T, U> &x) const
	{
		return std::hash<T>()(x.first) ^ std::hash<U>()(x.second);
	}
};


namespace Gomoku
{
	class BoardState
	{
		static constexpr int bits_per_cell = 2;
		static constexpr int cells_in_line = 19;
		static constexpr int bits_per_line = cells_in_line * bits_per_cell;

		using board_line = std::bitset<bits_per_line>;

		struct GomokuShape
		{
			board_line 	data;
			int			size;
		};

		constexpr static GomokuShape figure_five_w1 { 0b0101010101, 5}; // XXXXX
		constexpr static GomokuShape figure_free_three1_w { 0b00'010101'00, -1 };	// _XXX_
		constexpr static GomokuShape figure_free_three2_w { 0b00'01000101'00, -1 };	// _X_XX_
		constexpr static GomokuShape figure_free_three3_w { 0b00'01010001'00, -1 };	// _XX_X_

		std::unordered_set<std::pair<int, int>, pairhash> free_cells;
		std::unordered_set<std::pair<int, int>, pairhash> available_moves;

		// array of rows, board_[1][2] == board["c2"]
		std::array<board_line, 19> board_{};
		// Twisted lines of board
		mutable std::array<board_line, 19> vertical_{};
		mutable std::array<board_line, 38> up_lines_{};
		mutable std::array<board_line, 38> down_lines_{};

		board_line movePattern { 0b01 };

		std::vector<std::pair<int, int>> moves_;

	public:
		enum class Side
		{
			None = 0,
			White,
			Black
		};

		BoardState();
		explicit BoardState(const std::vector<std::pair<int, int>> &moves);

		static std::string MoveToString(const std::pair<int, int> &move)
		{
			std::stringstream ss;
			static const char *letters = "abcdefghijklmnopqrs";

			ss << letters[move.second] << move.first + 1;

			return ss.str();
		}

		[[nodiscard]] const auto& GetMovesList() const
		{
			return this->moves_;
		}

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
			// Delete cell from available ones
			free_cells.erase({row, col});
			// Check if capture and remove from board

			auto a = int(this->At(row + 1, col));
			auto b = int(this->At(row + 2, col));
			auto c = int(this->At(row + 3, col));
			auto d = movePattern.to_ulong();

			// capture pair up
			if (row + 3 < 19
				&& int(this->At(row + 1, col)) == (movePattern.to_ulong() ^ 0b11U)
				&& int(this->At(row + 2, col)) == (movePattern.to_ulong() ^ 0b11U)
				&& int(this->At(row + 3, col)) == (movePattern.to_ulong())
			)
			{
				free_cells.emplace(row + 1, col);
				free_cells.emplace(row + 2, col);

				board_[row + 1][col * 2] = false;
				board_[row + 1][col * 2 + 1] = false;

				board_[row + 2][col * 2] = false;
				board_[row + 2][col * 2 + 1] = false;
			}

			// capture pair up right
			if (row + 3 < 19
				&& col + 3 < 19
				&& int(this->At(row + 1, col + 1)) == (movePattern.to_ulong() ^ 0b11U)
				&& int(this->At(row + 2, col + 2)) == (movePattern.to_ulong() ^ 0b11U)
				&& int(this->At(row + 3, col + 3)) == (movePattern.to_ulong())
					)
			{
				free_cells.emplace(row + 1, col + 1);
				free_cells.emplace(row + 2, col + 2);

				board_[(row + 1)][(col + 1) * 2] = false;
				board_[(row + 1)][(col + 1) * 2 + 1] = false;

				board_[(row + 2)][(col + 2) * 2] = false;
				board_[(row + 2)][(col + 2) * 2 + 1] = false;
			}

			// capture pair right
			if (col + 3 < 19
				&& int(this->At(row, col + 1)) == (movePattern.to_ulong() ^ 0b11U)
				&& int(this->At(row, col + 2)) == (movePattern.to_ulong() ^ 0b11U)
				&& int(this->At(row, col + 3)) == (movePattern.to_ulong())
					)
			{
				free_cells.emplace(row, col + 1);
				free_cells.emplace(row, col + 2);

				board_[(row)][(col + 1) * 2] = false;
				board_[(row)][(col + 1) * 2 + 1] = false;

				board_[(row)][(col + 2) * 2] = false;
				board_[(row)][(col + 2) * 2 + 1] = false;
			}

			// capture pair down right
			if (row - 3 >= 0
				&& col + 3 < 19
				&& int(this->At(row - 1, col + 1)) == (movePattern.to_ulong() ^ 0b11U)
				&& int(this->At(row - 2, col + 2)) == (movePattern.to_ulong() ^ 0b11U)
				&& int(this->At(row - 3, col + 3)) == (movePattern.to_ulong())
					)
			{
				free_cells.emplace(row - 1, col + 1);
				free_cells.emplace(row - 2, col + 2);

				board_[(row - 1)][(col + 1) * 2] = false;
				board_[(row - 1)][(col + 1) * 2 + 1] = false;

				board_[(row - 2)][(col + 2) * 2] = false;
				board_[(row - 2)][(col + 2) * 2 + 1] = false;
			}

			// capture pair down
			if (row - 3 >= 0
				&& int(this->At(row - 1, col)) == (movePattern.to_ulong() ^ 0b11U)
				&& int(this->At(row - 2, col)) == (movePattern.to_ulong() ^ 0b11U)
				&& int(this->At(row - 3, col)) == (movePattern.to_ulong())
					)
			{
				free_cells.emplace(row - 1, col);
				free_cells.emplace(row - 2, col);

				board_[(row - 1)][(col) * 2] = false;
				board_[(row - 1)][(col) * 2 + 1] = false;

				board_[(row - 2)][(col) * 2] = false;
				board_[(row - 2)][(col) * 2 + 1] = false;
			}

			// capture pair down left
			if (row - 3 >= 0
				&& col - 3 >= 0
				&& int(this->At(row - 1, col - 1)) == (movePattern.to_ulong() ^ 0b11U)
				&& int(this->At(row - 2, col - 2)) == (movePattern.to_ulong() ^ 0b11U)
				&& int(this->At(row - 3, col - 3)) == (movePattern.to_ulong())
					)
			{
				free_cells.emplace(row - 1, col - 1);
				free_cells.emplace(row - 2, col -2);

				board_[(row - 1)][(col - 1) * 2] = false;
				board_[(row - 1)][(col - 1) * 2 + 1] = false;

				board_[(row - 2)][(col - 2) * 2] = false;
				board_[(row - 2)][(col - 2) * 2 + 1] = false;
			}

			// capture pair left
			if (col - 3 >= 0
				&& int(this->At(row, col - 1)) == (movePattern.to_ulong() ^ 0b11U)
				&& int(this->At(row, col - 2)) == (movePattern.to_ulong() ^ 0b11U)
				&& int(this->At(row, col - 3)) == (movePattern.to_ulong())
					)
			{
				free_cells.emplace(row, col - 1);
				free_cells.emplace(row, col - 2);

				board_[(row)][(col - 1) * 2] = false;
				board_[(row)][(col - 1) * 2 + 1] = false;

				board_[(row)][(col - 2) * 2] = false;
				board_[(row)][(col - 2) * 2 + 1] = false;
			}

			// capture pair up left
			if (row + 3 < 19
				&& col - 3 >= 0
				&& int(this->At(row + 1, col - 1)) == (movePattern.to_ulong() ^ 0b11U)
				&& int(this->At(row + 2, col - 2)) == (movePattern.to_ulong() ^ 0b11U)
				&& int(this->At(row + 3, col - 3)) == (movePattern.to_ulong())
					)
			{
				free_cells.emplace(row + 1, col - 1);
				free_cells.emplace(row + 2, col - 2);

				board_[(row + 1)][(col - 1) * 2] = false;
				board_[(row + 1)][(col - 1) * 2 + 1] = false;

				board_[(row + 2)][(col - 2) * 2] = false;
				board_[(row + 2)][(col - 2) * 2 + 1] = false;
			}


			const auto uple = std::make_pair(row + 3, col - 3);



			// Change move
			movePattern ^= 0b11;

			return true;
		}

		[[nodiscard]] bool WhiteMove() const
		{
			return movePattern == 0b01;
		}

		template<typename B>
		int CountFigures(const B &lines, const GomokuShape &shape) const
		{
			int ret = 0;

			// Check for 'fifth'
			for (const auto& row_ : lines)
			{
				for (int i = 0; i < cells_in_line - shape.size; i++)
				{

					auto copy = (row_
							<< ((cells_in_line - i - shape.size) * bits_per_cell)
							>> ((cells_in_line - i - shape.size) * bits_per_cell)
							>> (i * bits_per_cell));
					if (copy
							== shape.data
							)
						// shape in a row found!
						ret++;
				}
			}

			return ret;
		}

		[[nodiscard]] bool CanMakeMove(int row, int col) const
		{
			std::bitset<38> tmp;
			tmp[col * 2] = true;
			tmp[col * 2 + 1] = true;

			// Check if cell free
			if ((board_[row] & tmp).any())
				return false;

			// Check for 'fifth'
			if (CountFigures(board_, figure_five_w1) > 0)
				return false;

			// Prepare help lines
			// Vertical lines ||||
			for (int j = 0; j < cells_in_line; j++)
				for (int i = 0; i < cells_in_line; i++)
				{
					vertical_[j][2*i] = board_[i][2*j];
					vertical_[j][2*i + 1] = board_[i][2*j + 1];
				}
			if (CountFigures(vertical_, figure_five_w1) > 0)
				return false;
			// Up Lines ////
			constexpr int diagonal_count = cells_in_line * 2 - 1;
			for (int line = 1; line <= diagonal_count; line++)
			{
				/* Get column index of the first element
				   in this line of output.
				   The index is 0 for first ROW lines and
				   line - ROW for remaining lines  */
				int start_col = std::max(0, line - cells_in_line);

				/* Get count of elements in this line. The
				   count of elements is equal to minimum of
				   line number, COL-start_col and ROW */
				int count = std::min(line, std::min((cells_in_line - start_col), cells_in_line));

				/* Print elements of this line */
				for (int j = 0; j < count; j++)
				{
					up_lines_[line - 1][j * 2] = board_[std::min(cells_in_line, line)-j-1][(start_col+j) * 2];
					up_lines_[line - 1][(j * 2) + 1] = board_[std::min(cells_in_line, line)-j-1][(start_col+j) * 2 + 1];
				}
			}

			if (CountFigures(up_lines_, figure_five_w1) > 0)
				return false;
			// Down Lines \\\\  |
			for (int line = 1; line <= diagonal_count; line++)
			{
				int start_col = std::max(0, line - cells_in_line);
				int count = std::min(line, std::min((cells_in_line - start_col), cells_in_line));

				for (int j = 0; j < count; j++)
				{
					down_lines_[line - 1][j * 2] = board_[std::min(cells_in_line, line)-j-1][(cells_in_line - (start_col + j)) * 2];
					down_lines_[line - 1][(j * 2) + 1] = board_[std::min(cells_in_line, line)-j-1][(cells_in_line - (start_col+j)) * 2 + 1];
				}
			}
			if (CountFigures(down_lines_, figure_five_w1) > 0)
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

			// Change move
			movePattern ^= 0b11;

			return true;
		}

		void Reset()
		{
			movePattern = 0b01;
			moves_.clear();
			board_.fill(0);
		}

		Side At(int row, int col) const
		{
			switch (
					int(board_[row][col * 2]) |
					int(board_[row][col * 2 + 1] << 1)
					) {
				case 0:
					return Side::None;
				case 1:
					return Side::White;
				case 2:
					return Side::Black;
			}

			throw std::runtime_error("Unknown stone on board");
		}

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
