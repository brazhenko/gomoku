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
	public:
		// Global constants
		static constexpr int bits_per_cell = 2;
		static constexpr int cells_in_line = 19;
		static constexpr int bits_per_line = cells_in_line * bits_per_cell;

	private:
		using board_line = std::bitset<bits_per_line>;
		struct GomokuShape
		{
			board_line 	data;
			int			size{};
		};

	public:
		// Shapes to find
		// White shapes
		constexpr static GomokuShape figure_five_w {0b0101010101, 5};				// XXXXX

		constexpr static GomokuShape figure_free_four_w {0b00'01010101'00, 6};				// _XXXX_

		constexpr static GomokuShape figure_half_four1_w {0b00'01010101'10, 6};				// OXXXX_
		constexpr static GomokuShape figure_half_four2_w {0b10'01010101'00, 6};				// _XXXXO
		constexpr static GomokuShape figure_half_four3_w {0b010101'00'01, 5};				// X_XXX
		constexpr static GomokuShape figure_half_four4_w {0b0101'00'0101, 5};				// XX_XX
		constexpr static GomokuShape figure_half_four5_w {0b01'00'010101, 5};				// XXX_X


		constexpr static GomokuShape figure_free_three1_w { 0b0000'010101'0000, 7 };	// __XXX__
		constexpr static GomokuShape figure_free_three2_w { 0b0000'010101'0010, 7 };	// __XXX_O
		constexpr static GomokuShape figure_free_three3_w { 0b1000'010101'0000, 7 };	// O_XXX__
		constexpr static GomokuShape figure_free_three4_w { 0b00'01000101'00, 6 };	// _X_XX_
		constexpr static GomokuShape figure_free_three5_w { 0b00'01010001'00, 6 };	// _XX_X_

		constexpr static GomokuShape figure_free_two_w { 0b0000'0101'0000, 6 };	// __XX__


		// Black shapes
		constexpr static GomokuShape figure_five_b { 0b1010101010, 5};				// OOOOO

		constexpr static GomokuShape figure_free_four_b { 0b00'10101010'00, 6};		// _OOOO_

		constexpr static GomokuShape figure_half_four1_b { 0b011010101000, 6};		// _OOOOX
		constexpr static GomokuShape figure_half_four2_b { 0b001010101001, 6};		// XOOOO_
		constexpr static GomokuShape figure_half_four3_b { 0b10'00'101010, 5};		// 000_0
		constexpr static GomokuShape figure_half_four4_b { 0b1010'00'1010, 5};		// 00_00
		constexpr static GomokuShape figure_half_four5_b { 0b101010'00'10, 5};		// 0_000

		constexpr static GomokuShape figure_free_three1_b { 0b0000'101010'0000, 7 };	// __OOO__
		constexpr static GomokuShape figure_free_three2_b { 0b0000'101010'0001, 7 };	// __OOO_X
		constexpr static GomokuShape figure_free_three3_b { 0b0100'101010'0000, 7 };	// X_OOO__
		constexpr static GomokuShape figure_free_three4_b { 0b00'10001010'00, 6 };	// _O_OO_
		constexpr static GomokuShape figure_free_three5_b { 0b00'10100010'00, 6 };	// _OO_O_

		constexpr static GomokuShape figure_half_three1_b { 0b0000'1010'1001, 6 };	// X000__
		constexpr static GomokuShape figure_half_three2_b { 0b0010'0010'1001, 6 };	// X00_0_
		constexpr static GomokuShape figure_half_three3_b { 0b0010'1000'1001, 6 };	// X0_00_
		constexpr static GomokuShape figure_half_three4_b { 0b0110'1010'0000, 6 };	// __OOOX
		constexpr static GomokuShape figure_half_three5_b { 0b0110'1000'1000, 6 };	// _O_OOX
		constexpr static GomokuShape figure_half_three6_b { 0b0110'0010'1000, 6 };	// _OO_OX

		constexpr static GomokuShape figure_free_two_b { 0b0000'1010'0000, 6 };	// __OO__

		enum class MoveResult
		{
			Default,
			Capture,
			WhiteWin,
			BlackWin,
			Draw
		};
	private:

		// Mappings of coodinates: (Normal x, y) -> (Vericle, Diagonal1, Diagonal1 lines x, y respectively)
		static std::unordered_map<std::pair<int, int>, std::pair<int, int>, pairhash> _cToVerticles;
		static std::unordered_map<std::pair<int, int>, std::pair<int, int>, pairhash> _cToUpLines;
		static std::unordered_map<std::pair<int, int>, std::pair<int, int>, pairhash> _cToDownLines;

		std::unordered_set<std::pair<int, int>, pairhash> available_moves;

		// Capture points of
		int WhiteCapturePoints = 0;
		int BlackCapturePoints = 0;

		// Normal board: array of rows, board_[1][2] == board["c2"]
		std::array<board_line, 19> board_{};
		// Twisted lines of board
		std::array<board_line, 19> vertical_{};
		std::array<board_line, 38> up_lines_{};
		std::array<board_line, 38> down_lines_{};

		// Pattern of current move stone to put on board
		board_line movePattern { 0b01 };

		// History of moves
		std::vector<std::pair<int, int>> moves_;

		void FindMovesBreaksFifth();
		std::vector<std::pair<int, int>> MakeCapture(int row, int col);
		MoveResult MakeMoveInternal(int row, int col);
		MoveResult lastMoveResult_ = MoveResult::Default;

	public:
		static std::unordered_map<std::pair<int, int>, std::pair<int, int>, pairhash> InitVerticles();
		static std::unordered_map<std::pair<int, int>, std::pair<int, int>, pairhash> InitUpLines();
		static std::unordered_map<std::pair<int, int>, std::pair<int, int>, pairhash> InitDownLines();

		static std::string MoveToString(const std::pair<int, int> &move);
		static std::pair<int, int> StringToMove(const std::string &s);

		enum class Side
		{
			None = 0,
			White,
			Black
		};

		BoardState();
		explicit BoardState(const std::vector<std::pair<int, int>> &moves);
		void Reset();

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
					if (copy == shape.data)
						// shape in a row found!
						ret++;
				}
			}
			return ret;
		}


		[[nodiscard]] int CountFigureOverBoard(const GomokuShape &shape) const;
		[[nodiscard]] bool IsThereFigureOnBoard(const GomokuShape &shape) const;


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

		[[nodiscard]] bool IsMoveCapture(int row, int col, board_line s) const;

		[[nodiscard]] int CountFreeThrees(Side side) const;
		[[nodiscard]] int CountFreeThreesLastMove(Side side, std::pair<int, int> lastMove) const;

		MoveResult MakeMove(int row, int col);

		bool TakeBackMove();

		void Set(int row, int col, Side s);

		[[nodiscard]] const std::vector<std::pair<int, int>>& GetMovesList() const;
		[[nodiscard]] size_t hash() const;
		[[nodiscard]] bool WhiteMove() const;
		[[nodiscard]] Side At(int row, int col) const;
		[[nodiscard]] int GetCapturePoints(Side side) const;
		[[nodiscard]] MoveResult GetLastMoveResult() const;
		[[nodiscard]] const std::unordered_set<std::pair<int, int>, pairhash>& GetAvailableMoves() const;
		[[nodiscard]] std::string ToPgnString() const;

		// I/O of board
		friend std::ostream& operator<<(std::ostream& os, const BoardState& bs);
		friend std::istream& operator>>(std::istream& is, BoardState& bs);

        friend bool operator==(const Gomoku::BoardState& left, const Gomoku::BoardState& right);

//        ~BoardState(){
//        	std::cout << __FUNCTION__  << std::endl;
//        }
	};
}

// Helper
namespace std {
	template <>
	struct hash<Gomoku::BoardState> {
		std::size_t operator()(const Gomoku::BoardState& k) const {
			return k.hash();
		}
	};
}



#endif //GOMOKU_BOARD_H
