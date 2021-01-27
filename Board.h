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

		// White shapes
		constexpr static GomokuShape figure_five_w {0b0101010101, 5}; // XXXXX

		constexpr static GomokuShape figure_free_three1_w { 0b0000'010101'0000, 7 };	// __XXX__
		constexpr static GomokuShape figure_free_three2_w { 0b0000'010101'0010, 7 };	// __XXX_O
		constexpr static GomokuShape figure_free_three3_w { 0b1000'010101'0000, 7 };	// O_XXX__
		constexpr static GomokuShape figure_free_three4_w { 0b00'01000101'00, 6 };	// _X_XX_
		constexpr static GomokuShape figure_free_three5_w { 0b00'01010001'00, 6 };	// _XX_X_

		// Black shapes
		constexpr static GomokuShape figure_five_b { 0b1010101010, 5}; // OOOOO

		constexpr static GomokuShape figure_free_three1_b { 0b0000'101010'0000, 7 };	// __OOO__
		constexpr static GomokuShape figure_free_three2_b { 0b0000'101010'0001, 7 };	// __OOO_X
		constexpr static GomokuShape figure_free_three3_b { 0b0100'101010'0000, 7 };	// X_OOO__
		constexpr static GomokuShape figure_free_three4_b { 0b00'10001010'00, 6 };	// _O_OO_
		constexpr static GomokuShape figure_free_three5_b { 0b00'10100010'00, 6 };	// _OO_O_


		std::unordered_set<std::pair<int, int>, pairhash> free_cells;
		std::unordered_set<std::pair<int, int>, pairhash> available_moves;

		int WhiteCapturePoints = 0;
		int BlackCapturePoints = 0;

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

		bool IsMoveCapture(int row, int col) const
		{
			return
					(row + 3 < 19
					 && int(this->At(row + 1, col)) == (movePattern.to_ulong() ^ 0b11U)
					 && int(this->At(row + 2, col)) == (movePattern.to_ulong() ^ 0b11U)
					 && int(this->At(row + 3, col)) == (movePattern.to_ulong())
					)
					|| (row + 3 < 19
						&& col + 3 < 19
						&& int(this->At(row + 1, col + 1)) == (movePattern.to_ulong() ^ 0b11U)
						&& int(this->At(row + 2, col + 2)) == (movePattern.to_ulong() ^ 0b11U)
						&& int(this->At(row + 3, col + 3)) == (movePattern.to_ulong())
					   )
				   	|| (col + 3 < 19
						   && int(this->At(row, col + 1)) == (movePattern.to_ulong() ^ 0b11U)
						   && int(this->At(row, col + 2)) == (movePattern.to_ulong() ^ 0b11U)
						   && int(this->At(row, col + 3)) == (movePattern.to_ulong())
					 	)
					|| (row - 3 >= 0
					   && col + 3 < 19
					   && int(this->At(row - 1, col + 1)) == (movePattern.to_ulong() ^ 0b11U)
					   && int(this->At(row - 2, col + 2)) == (movePattern.to_ulong() ^ 0b11U)
					   && int(this->At(row - 3, col + 3)) == (movePattern.to_ulong())
																	 )
				 	||  (row - 3 >= 0
						&& int(this->At(row - 1, col)) == (movePattern.to_ulong() ^ 0b11U)
						&& int(this->At(row - 2, col)) == (movePattern.to_ulong() ^ 0b11U)
						&& int(this->At(row - 3, col)) == (movePattern.to_ulong())
					)
					|| (row - 3 >= 0
						&& col - 3 >= 0
						&& int(this->At(row - 1, col - 1)) == (movePattern.to_ulong() ^ 0b11U)
						&& int(this->At(row - 2, col - 2)) == (movePattern.to_ulong() ^ 0b11U)
						&& int(this->At(row - 3, col - 3)) == (movePattern.to_ulong())
					)
					|| (col - 3 >= 0
						&& int(this->At(row, col - 1)) == (movePattern.to_ulong() ^ 0b11U)
						&& int(this->At(row, col - 2)) == (movePattern.to_ulong() ^ 0b11U)
						&& int(this->At(row, col - 3)) == (movePattern.to_ulong())
							)
					|| (row + 3 < 19
						&& col - 3 >= 0
						&& int(this->At(row + 1, col - 1)) == (movePattern.to_ulong() ^ 0b11U)
						&& int(this->At(row + 2, col - 2)) == (movePattern.to_ulong() ^ 0b11U)
						&& int(this->At(row + 3, col - 3)) == (movePattern.to_ulong())
							);
		}
		int CountFreeThrees(Side side, const std::pair<int, int> lastMove) const
		{
			int freeThreesCount = 0;

			const auto& row = lastMove.first;
			const auto& col = lastMove.second;

			if (side == Side::White)
			{
				freeThreesCount += CountFiguresLines(board_, figure_free_three1_w, row);
				freeThreesCount += CountFiguresLines(vertical_, figure_free_three1_w, col);
				freeThreesCount += CountFiguresLines(up_lines_, figure_free_three1_w, row + col);
				freeThreesCount += CountFiguresLines(down_lines_, figure_free_three1_w, row + 18 - col);

				freeThreesCount += CountFiguresLines(board_, figure_free_three2_w, row);
				freeThreesCount += CountFiguresLines(vertical_, figure_free_three2_w, col);
				freeThreesCount += CountFiguresLines(up_lines_, figure_free_three2_w, row + col);
				freeThreesCount += CountFiguresLines(down_lines_, figure_free_three2_w, row + 18 - col);

				freeThreesCount += CountFiguresLines(board_, figure_free_three3_w, row);
				freeThreesCount += CountFiguresLines(vertical_, figure_free_three3_w, col);
				freeThreesCount += CountFiguresLines(up_lines_, figure_free_three3_w, row + col);
				freeThreesCount += CountFiguresLines(down_lines_, figure_free_three3_w, row + 18 - col);

				freeThreesCount += CountFiguresLines(board_, figure_free_three4_w, row);
				freeThreesCount += CountFiguresLines(vertical_, figure_free_three4_w, col);
				freeThreesCount += CountFiguresLines(up_lines_, figure_free_three4_w, row + col);
				freeThreesCount += CountFiguresLines(down_lines_, figure_free_three4_w, row + 18 - col);

				freeThreesCount += CountFiguresLines(board_, figure_free_three5_w, row);
				freeThreesCount += CountFiguresLines(vertical_, figure_free_three5_w, col);
				freeThreesCount += CountFiguresLines(up_lines_, figure_free_three5_w, row + 18 - col);
				freeThreesCount += CountFiguresLines(down_lines_, figure_free_three5_w, row + 18 - col);
			}
			else
			{
				freeThreesCount += CountFiguresLines(board_, figure_free_three1_b, row);
				freeThreesCount += CountFiguresLines(vertical_, figure_free_three1_b, col);
				freeThreesCount += CountFiguresLines(up_lines_, figure_free_three1_b, row + col);
				freeThreesCount += CountFiguresLines(down_lines_, figure_free_three1_b, row + 18 - col);

				freeThreesCount += CountFiguresLines(board_, figure_free_three2_b, row);
				freeThreesCount += CountFiguresLines(vertical_, figure_free_three2_b, col);
				freeThreesCount += CountFiguresLines(up_lines_, figure_free_three2_b, row + col);
				freeThreesCount += CountFiguresLines(down_lines_, figure_free_three2_b, row + 18 - col);

				freeThreesCount += CountFiguresLines(board_, figure_free_three3_b, row);
				freeThreesCount += CountFiguresLines(vertical_, figure_free_three3_b, col);
				freeThreesCount += CountFiguresLines(up_lines_, figure_free_three3_b, row + col);
				freeThreesCount += CountFiguresLines(down_lines_, figure_free_three3_b, row + 18 - col);

				freeThreesCount += CountFiguresLines(board_, figure_free_three4_b, row);
				freeThreesCount += CountFiguresLines(vertical_, figure_free_three4_b, col);
				freeThreesCount += CountFiguresLines(up_lines_, figure_free_three4_b, row + col);
				freeThreesCount += CountFiguresLines(down_lines_, figure_free_three4_b, row + 18 - col);

				freeThreesCount += CountFiguresLines(board_, figure_free_three5_b, row);
				freeThreesCount += CountFiguresLines(vertical_, figure_free_three5_b, col);
				freeThreesCount += CountFiguresLines(up_lines_, figure_free_three5_b, row + col);
				freeThreesCount += CountFiguresLines(down_lines_, figure_free_three5_b, row + 18 - col);
			}
			
			return freeThreesCount;
		}
		
		bool MakeMove(int row, int col) {
			if (!CanMakeMove(row, col)) return false;

			// Add to move history
			moves_.emplace_back(row, col);
			// Put stone on board
			board_[row] |= (movePattern << (col * 2));
			// Delete cell from available ones
			free_cells.erase({row, col});

			// Detect captures
			{
				// capture pair up
				if (row + 3 < 19
					&& int(this->At(row + 1, col)) == (movePattern.to_ulong() ^ 0b11U)
					&& int(this->At(row + 2, col)) == (movePattern.to_ulong() ^ 0b11U)
					&& int(this->At(row + 3, col)) == (movePattern.to_ulong())
						) {
					free_cells.emplace(row + 1, col);
					free_cells.emplace(row + 2, col);

					Set(row + 1, col, Side::None);
					Set(row + 2, col, Side::None);
				}

				// capture pair up right
				if (row + 3 < 19
					&& col + 3 < 19
					&& int(this->At(row + 1, col + 1)) == (movePattern.to_ulong() ^ 0b11U)
					&& int(this->At(row + 2, col + 2)) == (movePattern.to_ulong() ^ 0b11U)
					&& int(this->At(row + 3, col + 3)) == (movePattern.to_ulong())
						) {
					free_cells.emplace(row + 1, col + 1);
					free_cells.emplace(row + 2, col + 2);

					Set(row + 1, col + 1, Side::None);
					Set(row + 2, col + 2, Side::None);

					if (WhiteMove())
						WhiteCapturePoints++;
					else
						BlackCapturePoints++;
				}

				// capture pair right
				if (col + 3 < 19
					&& int(this->At(row, col + 1)) == (movePattern.to_ulong() ^ 0b11U)
					&& int(this->At(row, col + 2)) == (movePattern.to_ulong() ^ 0b11U)
					&& int(this->At(row, col + 3)) == (movePattern.to_ulong())
						) {
					free_cells.emplace(row, col + 1);
					free_cells.emplace(row, col + 2);

					Set(row, col + 1, Side::None);
					Set(row, col + 2, Side::None);

					if (WhiteMove())
						WhiteCapturePoints++;
					else
						BlackCapturePoints++;
				}

				// capture pair down right
				if (row - 3 >= 0
					&& col + 3 < 19
					&& int(this->At(row - 1, col + 1)) == (movePattern.to_ulong() ^ 0b11U)
					&& int(this->At(row - 2, col + 2)) == (movePattern.to_ulong() ^ 0b11U)
					&& int(this->At(row - 3, col + 3)) == (movePattern.to_ulong())
						) {
					free_cells.emplace(row - 1, col + 1);
					free_cells.emplace(row - 2, col + 2);

					Set(row - 1, col + 1, Side::None);
					Set(row - 2, col + 2, Side::None);

					if (WhiteMove())
						WhiteCapturePoints++;
					else
						BlackCapturePoints++;
				}

				// capture pair down
				if (row - 3 >= 0
					&& int(this->At(row - 1, col)) == (movePattern.to_ulong() ^ 0b11U)
					&& int(this->At(row - 2, col)) == (movePattern.to_ulong() ^ 0b11U)
					&& int(this->At(row - 3, col)) == (movePattern.to_ulong())
						) {
					free_cells.emplace(row - 1, col);
					free_cells.emplace(row - 2, col);

					Set(row - 1, col, Side::None);
					Set(row - 2, col, Side::None);

					if (WhiteMove())
						WhiteCapturePoints++;
					else
						BlackCapturePoints++;
				}

				// capture pair down left
				if (row - 3 >= 0
					&& col - 3 >= 0
					&& int(this->At(row - 1, col - 1)) == (movePattern.to_ulong() ^ 0b11U)
					&& int(this->At(row - 2, col - 2)) == (movePattern.to_ulong() ^ 0b11U)
					&& int(this->At(row - 3, col - 3)) == (movePattern.to_ulong())
						) {
					free_cells.emplace(row - 1, col - 1);
					free_cells.emplace(row - 2, col - 2);

					Set(row - 1, col - 1, Side::None);
					Set(row - 2, col - 2, Side::None);

					if (WhiteMove())
						WhiteCapturePoints++;
					else
						BlackCapturePoints++;
				}

				// capture pair left
				if (col - 3 >= 0
					&& int(this->At(row, col - 1)) == (movePattern.to_ulong() ^ 0b11U)
					&& int(this->At(row, col - 2)) == (movePattern.to_ulong() ^ 0b11U)
					&& int(this->At(row, col - 3)) == (movePattern.to_ulong())
						) {
					free_cells.emplace(row, col - 1);
					free_cells.emplace(row, col - 2);

					Set(row, col - 1, Side::None);
					Set(row, col - 2, Side::None);

					if (WhiteMove())
						WhiteCapturePoints++;
					else
						BlackCapturePoints++;
				}

				// capture pair up left
				if (row + 3 < 19
					&& col - 3 >= 0
					&& int(this->At(row + 1, col - 1)) == (movePattern.to_ulong() ^ 0b11U)
					&& int(this->At(row + 2, col - 2)) == (movePattern.to_ulong() ^ 0b11U)
					&& int(this->At(row + 3, col - 3)) == (movePattern.to_ulong())
						) {
					free_cells.emplace(row + 1, col - 1);
					free_cells.emplace(row + 2, col - 2);

					Set(row + 1, col - 1, Side::None);
					Set(row + 2, col - 2, Side::None);

					if (WhiteMove())
						WhiteCapturePoints++;
					else
						BlackCapturePoints++;
				}
			}

			available_moves = {};

			if (WhiteMove() && WhiteCapturePoints >= 5)
				return true;
			if (!WhiteMove() && BlackCapturePoints >= 5)
				return true;
			int fifthCount = 0;
			PrepareHelpLines();
			if (WhiteMove())
			{
				fifthCount += CountFigures(board_, figure_five_w);
				fifthCount += CountFigures(vertical_, figure_five_w);
				fifthCount += CountFigures(up_lines_, figure_five_w);
				fifthCount += CountFigures(down_lines_, figure_five_w);
			}
			else
			{
				fifthCount += CountFigures(board_, figure_five_b);
				fifthCount += CountFigures(vertical_, figure_five_b);
				fifthCount += CountFigures(up_lines_, figure_five_b);
				fifthCount += CountFigures(down_lines_, figure_five_b);
			}

			if (fifthCount > 0) return true;

			// Change move
			movePattern ^= 0b11;

			// Form available moves for next turn

			PrepareHelpLines();


			for (int i = 0; i < 19; i++)
			{
				for (int j = 0; j < 19; j++)
				{
					if (Side::None == At(i, j))
					{
						// Check if move valid
						if (IsMoveCapture(i, j))
						{
							// Captures always valid
							available_moves.emplace(i, j);
							continue;
						}
						int freeThreesCount = 0, newFreeThreesCount = 0;

						if (WhiteMove())
							freeThreesCount = CountFreeThrees(Side::White, {i, j});
						else
							freeThreesCount = CountFreeThrees(Side::Black, {i, j});

						// Pretend to make move
						Set(i, j, Side(movePattern.to_ulong()));

						if (WhiteMove())
							newFreeThreesCount = CountFreeThrees(Side::White, {i, j});
						else
							newFreeThreesCount = CountFreeThrees(Side::Black, {i, j});
						// Two or more free threes NOT produced
						if (!(newFreeThreesCount > freeThreesCount + 1))
							available_moves.emplace(i, j);

						// Return back pretended move
						Set(i, j, Side::None);
					}
				}
			}

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

		template<typename B>
		int CountFiguresLines(const B &lines, const GomokuShape &shape, int idx) const
		{
			int ret = 0;

			const auto& row_ = lines[idx];
			
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

			return ret;
		}

		void PrepareHelpLines() const
		{
			// Prepare help lines

			// Horizontal lines ____
			// Default board_

			// Vertical lines ||||
			for (int j = 0; j < cells_in_line; j++)
				for (int i = 0; i < cells_in_line; i++)
				{
					vertical_[j][2*i] = board_[i][2*j];
					vertical_[j][2*i + 1] = board_[i][2*j + 1];
				}
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
			// Down Lines \\\\  |
			for (int line = 1; line <= diagonal_count; line++)
			{
				int start_col = std::max(0, line - cells_in_line);
				int count = std::min(line, std::min((cells_in_line - start_col), cells_in_line));

				for (int j = 0; j < count; j++)
				{
					down_lines_[line - 1][j * 2] = board_[std::min(cells_in_line, line)-j-1][(cells_in_line - 1 - (start_col + j)) * 2];
					down_lines_[line - 1][(j * 2) + 1] = board_[std::min(cells_in_line, line)-j-1][(cells_in_line - 1 - (start_col+j)) * 2 + 1];
				}
			}
		}

		[[nodiscard]] bool CanMakeMove(int row, int col) const
		{
			PrepareHelpLines();

			// Check if cell free
			if (At(row, col) == Side::None)
				return true;

			// Check for 'fifth'
			if (CountFigures(board_, figure_five_w) > 0)
				return false;
			if (CountFigures(vertical_, figure_five_w) > 0)
				return false;
			if (CountFigures(up_lines_, figure_five_w) > 0)
				return false;
			if (CountFigures(down_lines_, figure_five_w) > 0)
				return false;

			return false;
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

			for (int i = 0; i < 19; i++)
				for (int j = 0; j < 19; j++)
					this->free_cells.emplace(i, j);

			moves_.clear();
			board_.fill(0);
		}

		void Set(int row, int col, Side s)
		{
			board_[row][col * 2] = unsigned(s)&1U;
			board_[row][col * 2 + 1] = (unsigned(s)>>1U)&1U;

			PrepareHelpLines();
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

		int GetCapturePoints(Side side) const
		{
			if (Side::White == side)
				return WhiteCapturePoints;
			else if (Side::Black == side)
				return BlackCapturePoints;
			return -1;
		}

		const std::unordered_set<std::pair<int, int>, pairhash>& GetAvailableMoves() const
		{
			return available_moves;
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
