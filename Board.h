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

/// Description
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
    /// @brief describes the rules of Gomoku42 game
	class Board
	{
	public:
	    /// @brief game board is a square, cells_in_line is a side length
		static constexpr int cells_in_line = 19;

	private:
	    /// @brief internal constant, number of bits represent a cell: \n
	    /// 0b00 - empty, 0b01 - white, 0b10 - black.
        static constexpr int bits_per_cell = 2;

	    /// @brief just help constant
		static constexpr int bits_per_line = cells_in_line * bits_per_cell;

    public:
	    /// @brief bit representation of a string
		using board_line = std::bitset<bits_per_line>;

		/// @brief cell coordinate pair, {row, col}
		using pcell = std::pair<int, int>;

		/// @brief helper class for searching typical figures on board
		struct GomokuShape
		{
			board_line 	data;
			int			size{};
		};

        /// @brief XXXXX
		constexpr static GomokuShape figure_five_w {0b0101010101, 5};

        /// @brief _XXXX_
		constexpr static GomokuShape figure_free_four_w {0b00'01010101'00, 6};
        /// @brief OXXXX_
		constexpr static GomokuShape figure_half_four1_w {0b00'01010101'10, 6};
		/// @brief _XXXXO
		constexpr static GomokuShape figure_half_four2_w {0b10'01010101'00, 6};
		/// @brief X_XXX
		constexpr static GomokuShape figure_half_four3_w {0b010101'00'01, 5};
		/// @brief XX_XX
		constexpr static GomokuShape figure_half_four4_w {0b0101'00'0101, 5};
		/// @brief XXX_X
		constexpr static GomokuShape figure_half_four5_w {0b01'00'010101, 5};
		/// @brief [XXXX_, `[` is a wall
		constexpr static GomokuShape figure_half_four6_special_w { 0b0001010101, 5 };
		/// @brief _XXXX], `]` is a wall
		constexpr static GomokuShape figure_half_four7_special_w { 0b0101010100, 5 };
		/// @brief __XXX__
		constexpr static GomokuShape figure_free_three1_w { 0b0000'010101'0000, 7 };
		/// @brief _XXX__
		constexpr static GomokuShape figure_free_three2_w { 0b0000'010101'00, 6 };
		/// @brief __XXX_
		constexpr static GomokuShape figure_free_three3_w { 0b00'010101'0000, 6};
		/// @brief _X_XX_
		constexpr static GomokuShape figure_free_three4_w { 0b00'01000101'00, 6 };
		/// @brief _XX_X_
		constexpr static GomokuShape figure_free_three5_w { 0b00'01010001'00, 6 };

        /// @brief OXXX__
		constexpr static GomokuShape figure_half_three1_w { 0b0000'010101'10, 6 };
		/// @brief __XXX0
		constexpr static GomokuShape figure_half_three2_w { 0b10'010101'0000, 6 };
		/// @brief OXX_X_
		constexpr static GomokuShape figure_half_three3_w { 0b00'01000101'10, 6 };
		/// @brief _X_XXO
		constexpr static GomokuShape figure_half_three4_w { 0b10'01010001'00, 6 };
		/// @brief OX_XX_
		constexpr static GomokuShape figure_half_three5_w { 0b00'01010001'10, 6 };
		/// @brief _XX_XO
		constexpr static GomokuShape figure_half_three6_w { 0b10'01000101'00, 6 };

		/// @brief OOOOO
		constexpr static GomokuShape figure_five_b { 0b1010101010, 5};
		/// @brief _OOOO_
		constexpr static GomokuShape figure_free_four_b { 0b00'10101010'00, 6};
		/// @brief _OOOOX
		constexpr static GomokuShape figure_half_four1_b { 0b011010101000, 6};
		/// @brief XOOOO_
		constexpr static GomokuShape figure_half_four2_b { 0b001010101001, 6};
		/// @brief OOO_O
		constexpr static GomokuShape figure_half_four3_b { 0b10'00'101010, 5};
		/// @brief OO_OO
		constexpr static GomokuShape figure_half_four4_b { 0b1010'00'1010, 5};
		/// @brief O_OOO
		constexpr static GomokuShape figure_half_four5_b { 0b101010'00'10, 5};
        /// @brief [OOOO_, `[` is a wall
		constexpr static GomokuShape figure_half_four6_special_b { 0b0010101010, 5 };
		/// @brief _OOOO], `]` is a wall
		constexpr static GomokuShape figure_half_four7_special_b { 0b1010101000, 5 };
		/// @brief __OOO__
		constexpr static GomokuShape figure_free_three1_b { 0b0000'101010'0000, 7 };
		/// @brief _OOO__
		constexpr static GomokuShape figure_free_three2_b { 0b0000'101010'00, 6 };
		/// @brief __OOO_
		constexpr static GomokuShape figure_free_three3_b { 0b00'101010'0000, 6 };
		/// @brief _O_OO_
		constexpr static GomokuShape figure_free_three4_b { 0b00'10001010'00, 6 };
		/// @brief _OO_O_
		constexpr static GomokuShape figure_free_three5_b { 0b00'10100010'00, 6 };
        /// @brief XOOO__
		constexpr static GomokuShape figure_half_three1_b { 0b0000'1010'1001, 6 };
		/// @brief __OOOX
		constexpr static GomokuShape figure_half_three2_b { 0b0110'1010'0000, 6 };
		/// @brief XOO_O_
		constexpr static GomokuShape figure_half_three3_b { 0b0010'0010'1001, 6 };
		/// @brief _O_OOX
		constexpr static GomokuShape figure_half_three4_b { 0b0110'1000'1000, 6 };
		/// @brief XO_OO_
		constexpr static GomokuShape figure_half_three5_b { 0b0010'1000'1001, 6 };
		/// @brief _OO_OX
		constexpr static GomokuShape figure_half_three6_b { 0b0110'0010'1000, 6 };

		/// @brief Main identifier of player side. White = Cross = `X` = Red. Black = Zero = `O` = Blue.
		enum class Side
		{
			None = 0,
			White,
			Black
		};

		enum class MoveResult
		{
			Default = 0,
			NotMyMove,
			NotReadyToMove,
			Capture,
			WrongMove,
			WhiteWin,
			BlackWin,
			Draw
		};

	private:
		// Mappings of coodinates: (Normal x, y) -> (Vericle, Diagonal1, Diagonal1 lines x, y respectively)
		const static std::unordered_map<pcell, pcell, pairhash> _cToVerticles;
		const static std::unordered_map<pcell, pcell, pairhash> _cToUpLines;
		const static std::unordered_map<pcell, pcell, pairhash> _cToDownLines;

		std::vector<pcell> availableMoves_;

		/// @brief White captured black stones count
		int WhiteCapturePoints = 0;
		/// @brief Black captured white stones count
		int BlackCapturePoints = 0;

		// Normal board: array of rows, board_[1][2] == board["c2"]
		/// @brief just normal board \n

        /// \code{}
        /// .X.. - board_[3] = 00 01 00 00
        /// ...X - board_[2] = 00 00 00 01
        /// O.X. - board_[1] = 10 00 01 00
        /// .O.. - board_[0] = 00 10 00 00
        ///  \endcode
		std::array<board_line, 19> board_{};

        /// @brief vertical mapping of the board \n

        /// if board_ is
        /// \code{}
        /// .X.. - board_[3] = 00 01 00 00
        /// ...X - board_[2] = 00 00 00 01
        /// O.X. - board_[1] = 10 00 01 00
        /// .O.. - board_[0] = 00 10 00 00
        ///  \endcode

        /// vertical_ is
        /// \code{}
        /// .O.. - vertical_[3]
        /// O..X - vertical_[2]
        /// .X.. - vertical_[1]
        /// ..X. - vertical_[0]
        ///  \endcode
		std::array<board_line, 19> vertical_{};

        /// @brief diagonal1 mapping of the board \n

        /// if board_ is
        /// \code{}
        /// .X.. - board_[3] = 00 01 00 00
        /// ...X - board_[2] = 00 00 00 01
        /// O.X. - board_[1] = 10 00 01 00
        /// .O.. - board_[0] = 00 10 00 00
        ///  \endcode

        /// upLines_ is
        /// \code{}
        /// .    - upLines_[0]
        /// OO   - upLines_[1]
        /// ...  - upLines_[2]
        /// ..X. - upLines_[3]
        /// X..  - upLines_[4]
        /// .X   - upLines_[5]
        /// .    - upLines_[6]
        ///  \endcode
		std::array<board_line, 38> upLines_{};

        /// @brief diagonal2 mapping of the board \n

        /// if board_ is
        /// \code{}
        /// .X.. - board_[3] = 00 01 00 00
        /// ...X - board_[2] = 00 00 00 01
        /// O.X. - board_[1] = 10 00 01 00
        /// .O.. - board_[0] = 00 10 00 00
        ///  \endcode

        /// downLines_ is
        /// \code{}
        /// .    - downLines_[0]
        /// ..   - downLines_[1]
        /// OXX  - downLines_[2]
        /// .... - downLines_[3]
        /// O..  - downLines_[4]
        /// .X   - downLines_[5]
        /// .    - downLines_[6]
        ///  \endcode
		std::array<board_line, 38> downLines_{};

		/// @brief current move pattern, 0b01 - white, 0b10 - black
		board_line movePattern_ {0b01 };

		/// Moves history
		std::vector<pcell> moves_;

		// Default MoveResult
		MoveResult lastMoveResult_ = MoveResult::Default;

		/// @brief Internal functions generates available moves ONLY IF there is a five on a board
		void FindMovesBreaksFifthInternal();
		/// General available moves generator
		void GenerateAvailableMovesInternal();

		std::vector<pcell> MakeCapture(pcell move);
		MoveResult MakeMoveInternal(int row, int col);

		/// @brief Sets particular cell as occupied with a stone or becomes empty
		/// @param [in] row row of a cell
		/// @param [in] col column of a cell
		/// @param [in] s side of a stone. May me Side::None
		void SetStoneInternal(int row, int col, Side s);

		// Const methods
		template<typename B>
		int CountFigures(const B &lines, const GomokuShape &shape, bool diagonal=false) const;
		template<typename B>
		int CountFiguresBeginRow(const B &lines, const GomokuShape &shape, bool diagonal=false) const;
		template<typename B>
		int CountFiguresEndRow(const B &lines, const GomokuShape &shape, bool diagonal=false) const;
		template<typename B>
		int CountFiguresPoints(const B &lines, const GomokuShape &shape, int x, int y) const;

	public:
	    /// @brief Default ctor
		Board();

	    /// @brief Costructs a board from a list of moves.
	    /// @param [in] moves list of moves
		explicit Board(const std::vector<pcell> &moves);

		/// @brief the board becomes new as default constructed
		void Reset();

		/// @brief Undo one move (2 semi-moves)
		/// @return true if success, false if not
		bool TakeBackMove();

        /// @brief function with obvious purpose
        /// @param [in] move
        /// @return // TODO
		MoveResult MakeMove(pcell move);

		/// @brief converts internal representation of board cell to human one
		/// @param [in] move internal representation of board cell, e.g. {0, 1},
		///     move MUST be a valid one otherwise behaviour is undefined
		/// @return human representation of a move, e.g. "j10"
		static std::string MoveToString(const pcell &move);

		/// @brief converts human representation of board cell to internal
		/// @param [in] s string representation, e.g. j10
		/// @return internal representation of a move, e.g. {10, 3}
		static pcell StringToMove(const std::string &s);


		[[nodiscard]] int CountFigureOverBoard(const GomokuShape &shape) const;
		[[nodiscard]] bool IsThereFigureOnBoard(const GomokuShape &shape) const;
		[[nodiscard]] int CountFreeThrees(Side side) const;
		[[nodiscard]] int CountHalfFreeThrees(Side side) const;
		[[nodiscard]] int CountFreeThreesLastMove(Side side, pcell lastMove) const;
		[[nodiscard]] int CountHalfFreeFours(Side side) const;


		[[nodiscard]] bool IsMoveCapture(int row, int col, board_line s) const;

		/// @brief Getter for list of moves made.
		/// @return vector of moves which were made during the game
		[[nodiscard]] const std::vector<pcell>& GetMovesList() const;


		[[nodiscard]] size_t hash() const;

		/// @brief Who's turn?
		/// @return true White's turn
		///         false - otherwise
		[[nodiscard]] bool WhiteMove() const;

		/// @brief Counts total amount of stones on the board
		/// @return count of stoned
		[[nodiscard]] int GetStoneCount() const;

		/// @brief Get list of moves that can be made on this move
		/// @return vector of available moves
		[[nodiscard]] const std::vector<pcell>& GetAvailableMoves() const;

        /// @brief What is on a particular cell?
        /// @param [in] cell cell
        /// @return Side::None if cell is empty
        ///         Side::White if White stone
        ///         Side::Black if Black stone
		[[nodiscard]] Side At(pcell cell) const;

		/// @brief What is on a particular cell?
		/// @param [in] row row of a cell
		/// @param [in] col column of a cell
		/// @return Side::None if cell is empty
        ///         Side::White if White stone
        ///         Side::Black if Black stone
		[[nodiscard]] Side At(int row, int col) const;

		/// @brief What is on a particular cell?
		/// @param [in] move string repr. of cell
		/// @return Side::None if cell is empty
        ///         Side::White if White stone
        ///         Side::Black if Black stone
		[[nodiscard]] Side At(const std::string& move) const;

		/// @brief How many capture points player has?
		/// @param [in] side Side::White of Side::Black, otherwise behaviour is undefined
		/// @return how many captures particular $side made
		[[nodiscard]] int GetCapturePoints(Side side) const;

		[[nodiscard]] MoveResult GetLastMoveResult() const;

        /// @brief Checks if particular cell has a stone in its locality of radius eps \n
        /// Example:
        /// V is a target cell, XO are stones
        /// \code{}
        /// ...........
        /// ...........
        /// ....V...X..
        /// ........O..
        /// ...........
        ///  \endcode
        /// if eps <= 3 return is false, if eps > 3 return is true

        /// @param [in] cell cell
        /// @param [in] eps epsilon, locality "radius"
        /// @return true if has, false otherwise
        [[nodiscard]] bool IsCellHasStoneNearby(pcell cell, int eps=1) const;

        /// @brief Converts object to portable game notation string
        /// @return pgn string
		[[nodiscard]] std::string ToPgnString() const;

		/// @brief output operator
		friend std::ostream& operator<<(std::ostream& os, const Board& bs);
		/// @brief input operator
		friend std::istream& operator>>(std::istream& is, Board& bs);
        /// @brief equ operator
        friend bool operator==(const Gomoku::Board& left, const Gomoku::Board& right);
	};
}

// Helper
namespace std {
	template <>
	struct hash<Gomoku::Board> {
		std::size_t operator()(const Gomoku::Board& k) const {
			return k.hash();
		}
	};
}

#endif //GOMOKU_BOARD_H
