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
#include <unordered_map>
#include <fstream>
#include <mutex>

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
			int			size{};
		};

		// White shapes
		constexpr static GomokuShape figure_five_w {0b0101010101, 5};				// XXXXX

		constexpr static GomokuShape figure_free_three1_w { 0b0000'010101'0000, 7 };	// __XXX__
		constexpr static GomokuShape figure_free_three2_w { 0b0000'010101'0010, 7 };	// __XXX_O
		constexpr static GomokuShape figure_free_three3_w { 0b1000'010101'0000, 7 };	// O_XXX__
		constexpr static GomokuShape figure_free_three4_w { 0b00'01000101'00, 6 };	// _X_XX_
		constexpr static GomokuShape figure_free_three5_w { 0b00'01010001'00, 6 };	// _XX_X_

		// Black shapes
		constexpr static GomokuShape figure_five_b { 0b1010101010, 5};				// OOOOO

		constexpr static GomokuShape figure_free_three1_b { 0b0000'101010'0000, 7 };	// __OOO__
		constexpr static GomokuShape figure_free_three2_b { 0b0000'101010'0001, 7 };	// __OOO_X
		constexpr static GomokuShape figure_free_three3_b { 0b0100'101010'0000, 7 };	// X_OOO__
		constexpr static GomokuShape figure_free_three4_b { 0b00'10001010'00, 6 };	// _O_OO_
		constexpr static GomokuShape figure_free_three5_b { 0b00'10100010'00, 6 };	// _OO_O_

		std::unordered_map<std::pair<int, int>, std::pair<int, int>, pairhash> _cToVerticles;
		std::unordered_map<std::pair<int, int>, std::pair<int, int>, pairhash> _cToUpLines;
		std::unordered_map<std::pair<int, int>, std::pair<int, int>, pairhash> _cToDownLines;

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

		static std::pair<int, int> StringToMove(const std::string &s)
		{
			std::pair<int, int> ret;

			ret.second = s[0] - 'a';

			ret.first = std::stoi(s.c_str() + 1) - 1;

			return ret;
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

		bool IsMoveCapture(int row, int col) const;

		int CountFreeThrees(Side side, std::pair<int, int> lastMove) const;
		
		bool MakeMove(int row, int col);
		std::vector<std::pair<int, int>> MakeCapture(int row, int col);
		void FindMovesBreaksFifth();


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

		template<typename B>
		int CountFiguresPoints(const B &lines, const GomokuShape &shape, int x, int y) const
		{
			int ret = 0;

			const auto& row_ = lines[x];

			for (int i = std::max(0, y - shape.size + 1); i <= y; i++) //std::min(y + shape.size, cells_in_line - shape.size)
			{
				auto copy = (row_
						<< ((cells_in_line - i - shape.size) * bits_per_cell)
						>> ((cells_in_line - i - shape.size) * bits_per_cell)
						>> (i * bits_per_cell));
				if (copy == shape.data)
					// shape in a row found!
					ret++;
			}

			return ret;
		}

		bool TakeBackMove()
		{
			if (moves_.empty()) return false;
			moves_.pop_back();


			*this = BoardState(moves_);
			return true;
		}

		void Reset()
		{
			*this = BoardState();
		}

		void Set(int row, int col, Side s)
		{
			const auto &verticle = _cToVerticles.at({row, col});
			const auto &upline = _cToUpLines.at({row, col});
			const auto &downline = _cToDownLines.at({row, col});

			board_[row][col * 2] = unsigned(s)&1U;
			board_[row][col * 2 + 1] = (unsigned(s)>>1U)&1U;

			vertical_[verticle.first][verticle.second * 2] = unsigned(s)&1U;
			vertical_[verticle.first][verticle.second * 2 + 1] = (unsigned(s)>>1U)&1U;

			up_lines_[upline.first][upline.second * 2] = unsigned(s)&1U;
			up_lines_[upline.first][upline.second * 2 + 1] = (unsigned(s)>>1U)&1U;

			down_lines_[downline.first][downline.second * 2] = unsigned(s)&1U;
			down_lines_[downline.first][downline.second * 2 + 1] = (unsigned(s)>>1U)&1U;
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

		std::string ToPgnString() const;

		friend std::ostream& operator<<(std::ostream& os, const BoardState& bs)
		{
			os << bs.GetCapturePoints(Side::White) << " " << bs.GetCapturePoints(Side::Black) << " " << bs.movePattern << std::endl;
			for (int i = 18; i >= 0; i--) {
				for (int j = 0; j < 19; j++) {
					switch (bs.At(i, j)) {
						case Side::None:
							os << "_";
							break;
						case Side::White:
							os << "X";
							break;
						case Side::Black:
							os << "O";
							break;
					}
				}
				os << std::endl;
			}
			return os;
		}
		friend std::istream& operator>>(std::istream& is, BoardState& bs)
		{
			bs = BoardState();

			int a, b;
			is >> bs.WhiteCapturePoints >> bs.BlackCapturePoints >> bs.movePattern;
			std::cout << bs.WhiteCapturePoints << bs.BlackCapturePoints << bs.movePattern;
			for (int i = 18; i >= 0; i--) {
				for (int j = 0; j < 19; j++) {
					char kek;
					is >> kek;
					switch (kek) {
						case '_':
							bs.Set(i, j, Side::None); 
							break;
						case 'X':
							bs.Set(i, j, Side::White); 
							break;
						case 'O':
							bs.Set(i, j, Side::Black); 
							break;
					}
				}
			}
			return is;
		}

	};
}

// std::cout << game.board_;


namespace std {
	template <>
	struct hash<Gomoku::BoardState> {
		std::size_t operator()(const Gomoku::BoardState& k) const {
			return k.hash();
		}
	};
}


#endif //GOMOKU_BOARD_H
