//
// Created by Brazhenko Andrew on 19.01.2021.
//

#ifndef GOMOKU_BOARD_H
#define GOMOKU_BOARD_H

#include <vector>
#include <array>
#include <bitset>
#include <unordered_map>

namespace Gomoku
{
    /// @brief Gomoku::Board is main class of Gomoku42 game. \n
    /// Game rules and a bit more are implemented here.

	class Board
	{
	public:
	    /// @brief game board is a square, cells_in_line is a side length
		static constexpr int cells_in_line = 19;

	private:
	    /// @brief internal constant, number of bits represent a cell: \n
	    /// `0b00` - empty, `0b01` - white, `0b10` - black.
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

		/// @brief hasher for std::pair<T, U>
		struct PairHash
        {
			template <typename T, typename U>
			std::size_t operator()(const std::pair<T, U> &x) const
			{
				return std::hash<T>()(x.first) ^ std::hash<U>()(x.second);
			}
		};

        /// @brief `XXXXX`
		constexpr static GomokuShape figure_five_w {0b0101010101, 5};

        /// @brief `_XXXX_`
		constexpr static GomokuShape figure_free_four_w {0b00'01010101'00, 6};
        /// @brief `OXXXX_`
		constexpr static GomokuShape figure_half_four1_w {0b00'01010101'10, 6};
		/// @brief `_XXXXO`
		constexpr static GomokuShape figure_half_four2_w {0b10'01010101'00, 6};
		/// @brief `X_XXX`
		constexpr static GomokuShape figure_half_four3_w {0b010101'00'01, 5};
		/// @brief `XX_XX`
		constexpr static GomokuShape figure_half_four4_w {0b0101'00'0101, 5};
		/// @brief `XXX_X`
		constexpr static GomokuShape figure_half_four5_w {0b01'00'010101, 5};
		/// @brief `[XXXX_`, `[` is a wall
		constexpr static GomokuShape figure_half_four6_special_w { 0b0001010101, 5 };
		/// @brief `_XXXX]`, `]` is a wall
		constexpr static GomokuShape figure_half_four7_special_w { 0b0101010100, 5 };
		/// @brief `__XXX__`
		constexpr static GomokuShape figure_free_three1_w { 0b0000'010101'0000, 7 };
		/// @brief `_XXX__`
		constexpr static GomokuShape figure_free_three2_w { 0b0000'010101'00, 6 };
		/// @brief `__XXX_`
		constexpr static GomokuShape figure_free_three3_w { 0b00'010101'0000, 6};
		/// @brief `_X_XX_`
		constexpr static GomokuShape figure_free_three4_w { 0b00'01000101'00, 6 };
		/// @brief `_XX_X_`
		constexpr static GomokuShape figure_free_three5_w { 0b00'01010001'00, 6 };

        /// @brief `OXXX__`
		constexpr static GomokuShape figure_half_three1_w { 0b0000'010101'10, 6 };
		/// @brief `__XXX0`
		constexpr static GomokuShape figure_half_three2_w { 0b10'010101'0000, 6 };
		/// @brief `OXX_X_`
		constexpr static GomokuShape figure_half_three3_w { 0b00'01000101'10, 6 };
		/// @brief `_X_XXO`
		constexpr static GomokuShape figure_half_three4_w { 0b10'01010001'00, 6 };
		/// @brief `OX_XX_`
		constexpr static GomokuShape figure_half_three5_w { 0b00'01010001'10, 6 };
		/// @brief `_XX_XO`
		constexpr static GomokuShape figure_half_three6_w { 0b10'01000101'00, 6 };
		/// @brief `X_X_X_X`
		constexpr static GomokuShape figure_dots_w { 0b01'00'01'00'01'00'01, 7 };


		/// @brief `OOOOO`
		constexpr static GomokuShape figure_five_b { 0b1010101010, 5};
		/// @brief `_OOOO_`
		constexpr static GomokuShape figure_free_four_b { 0b00'10101010'00, 6};
		/// @brief `_OOOOX`
		constexpr static GomokuShape figure_half_four1_b { 0b011010101000, 6};
		/// @brief `XOOOO_`
		constexpr static GomokuShape figure_half_four2_b { 0b001010101001, 6};
		/// @brief `OOO_O`
		constexpr static GomokuShape figure_half_four3_b { 0b10'00'101010, 5};
		/// @brief `OO_OO`
		constexpr static GomokuShape figure_half_four4_b { 0b1010'00'1010, 5};
		/// @brief `O_OOO`
		constexpr static GomokuShape figure_half_four5_b { 0b101010'00'10, 5};
        /// @brief `[OOOO_`, `[` is a wall
		constexpr static GomokuShape figure_half_four6_special_b { 0b0010101010, 5 };
		/// @brief `_OOOO]`, `]` is a wall
		constexpr static GomokuShape figure_half_four7_special_b { 0b1010101000, 5 };
		/// @brief `__OOO__`
		constexpr static GomokuShape figure_free_three1_b { 0b0000'101010'0000, 7 };
		/// @brief `_OOO__`
		constexpr static GomokuShape figure_free_three2_b { 0b0000'101010'00, 6 };
		/// @brief `__OOO_`
		constexpr static GomokuShape figure_free_three3_b { 0b00'101010'0000, 6 };
		/// @brief `_O_OO_`
		constexpr static GomokuShape figure_free_three4_b { 0b00'10001010'00, 6 };
		/// @brief `_OO_O_`
		constexpr static GomokuShape figure_free_three5_b { 0b00'10100010'00, 6 };
        /// @brief `XOOO__`
		constexpr static GomokuShape figure_half_three1_b { 0b0000'1010'1001, 6 };
		/// @brief `__OOOX`
		constexpr static GomokuShape figure_half_three2_b { 0b0110'1010'0000, 6 };
		/// @brief `XOO_O_`
		constexpr static GomokuShape figure_half_three3_b { 0b0010'0010'1001, 6 };
		/// @brief `_O_OOX`
		constexpr static GomokuShape figure_half_three4_b { 0b0110'1000'1000, 6 };
		/// @brief `XO_OO_`
		constexpr static GomokuShape figure_half_three5_b { 0b0010'1000'1001, 6 };
		/// @brief `_OO_OX`
		constexpr static GomokuShape figure_half_three6_b { 0b0110'0010'1000, 6 };
		/// @brief `O_O_O_O`
		constexpr static GomokuShape figure_dots_b { 0b10'00'10'00'10'00'10, 7 };


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
		/// @brief It was decided that we have to store four projections of the board
		/// to have an opportunity to search for figures quickly
		///
		/// We have to find _XXX__ on board:
		///
		/// @code{}
		///	______
		///	____X_
		///	___X__
		///	__X___
		///	______
		/// ______
		/// @endcode
		/// The initial `board_` stores it row by row and we have no opportunity
		/// to look up in verticles and diagonals. \n
		/// Obviously we generate the following projections: \n
		/// horizonal (general one), `board_`
		/// @code{}
		///	______
		///
		///	____X_
		///
		///	___X__
		///
		///	__X___
		///
		///	______
		///
		/// ______
		/// @endcode

		/// `vertical_`
		/// @code{}
		///	_ _ _ _ _ _
		///	_ _ _ _ X _
		///	_ _ _ X _ _
		///	_ _ X _ _ _
		///	_ _ _ _ _ _
		/// _ _ _ _ _ _
		/// @endcode

		/// `upLines_`
		/// @code{}
		///	_/_/_/_/_/_
		/// / / / / / /
		///	_/_/_/_/X/_
		/// / / / / / /
		///	_/_/_/X/_/_
		/// / / / / / /
		///	_/_/X/_/_/_
		/// / / / / / /
		///	_/_/_/_/_/_
		/// / / / / / /
		/// _/_/_/_/_/_
		/// @endcode

		/// `downLines_`
		/// @code{}
		///	_\_\_\_\_\_
		/// \ \ \ \ \ \
		///	_\_\_\_\X\_
		/// \ \ \ \ \ \
		///	_\_\_\X\_\_
		/// \ \ \ \ \ \
		///	_\_\X\_\_\_
		/// \ \ \ \ \ \
		///	_\_\_\_\_\_
		/// \ \ \ \ \ \
		/// _\_\_\_\_\_
		/// @endcode

		/// After that we need mappings `board_ -> vertical_`,
		/// `board_ -> upLines_`,
		/// `board_ -> downLines_`. \n
		/// `_cToVerticles`, `_cToUpLines`, `_cToDownLines` are the respective mentioned mappings.
		const static std::unordered_map<pcell, pcell, PairHash> _cToVerticles;

		/// @brief Read `_cToVerticles` before
		const static std::unordered_map<pcell, pcell, PairHash> _cToUpLines;

		/// @brief Read `_cToVerticles` before
		const static std::unordered_map<pcell, pcell, PairHash> _cToDownLines;

		std::vector<pcell> availableMoves_;

		/// @brief White captured black stones count
		int WhiteCapturePoints_ = 0;
		/// @brief Black captured white stones count
		int BlackCapturePoints_ = 0;

		/// @brief just normal board \n

        /// \code{}
        /// .X.. - board_[3] = 00 01 00 00
        /// ...X - board_[2] = 00 00 00 01
        /// O.X. - board_[1] = 10 00 01 00
        /// .O.. - board_[0] = 00 10 00 00
        ///  \endcode
        /// `board_[1][2] == board["c2"]`
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

		/// @brief Moves history
		std::vector<pcell> moves_;

		/// @brief move result
		MoveResult lastMoveResult_ = MoveResult::Default;

		/// @brief Internal function generates available moves ONLY IF there is a five on a board
		void FindMovesBreaksFifthInternal();

		/// @brief General available moves generator
		void GenerateAvailableMovesInternal();

		/// @brief Performs a "capture" as if `move` is a last one
		/// @param [in] move "last move"
		/// @param [in] side side of capturer stone
		/// @return vector of cells wich were captured
		std::vector<pcell> MakeCaptureInternal(pcell move, Side side);

		/// @brief Internal MakeMove function. Really is not supposed to be called. Call MakeMove instead
		/// @param [in] row row index
		/// @param [in] col column index
		/// @return result code
		MoveResult MakeMoveInternal(int row, int col);

		/// @brief Sets particular cell as occupied with a stone removes stone
		/// @param [in] row row of a cell
		/// @param [in] col column of a cell
		/// @param [in] s side of a stone. May me `Side::None`
		void SetStoneInternal(int row, int col, Side s);

		/// @brief Internal function counts `shape` entries in `lines`
		/// @tparam B interable, consists of bitsets, supposed to be one of `board_`, `vertical_`, `upLines_` or `downLines_`
		/// @param [in] lines lines where to search
		/// @param [in] shape shape to search for
		/// @param [in] diagonal is `lines` diagonal
		/// @return count of `shape` entries in `lines`
		template<typename B>
		int CountFigures(const B &lines, const GomokuShape &shape, bool diagonal = false) const;

		/// @brief Internal function counts `shape` entries in `lines` touches beginning wall
		/// @tparam B interable, consists of bitsets, supposed to be one of `board_`, `vertical_`, `upLines_` or `downLines_`
		/// @param [in] lines lines where to search
		/// @param [in] shape shape to search for
		/// @param [in] diagonal is `lines` diagonal
		/// @return count of `shape` entries in `lines`
		template<typename B>
		int CountFiguresBeginRow(const B &lines, const GomokuShape &shape, bool diagonal = false) const;

		/// @brief Internal function counts `shape` entries in `lines` touches ending wall
		/// @tparam B interable, consists of bitsets, supposed to be one of `board_`, `vertical_`, `upLines_` or `downLines_`
		/// @param [in] lines lines where to search
		/// @param [in] shape shape to search for
		/// @param [in] diagonal is `lines` diagonal
		/// @return count of `shape` entries in `lines`
		template<typename B>
		int CountFiguresEndRow(const B &lines, const GomokuShape &shape, bool diagonal = false) const;

		/// @brief Internal function counts `shape` entries in `lines`, `{row, col}` is a part of `shape`
		/// @tparam B interable, consists of bitsets, supposed to be one of `board_`, `vertical_`, `upLines_` or `downLines_`
		/// @param [in] lines lines where to search
		/// @param [in] shape shape to search for
		/// @param [in] row row coordinate of cell
		/// @param [in] col column coordinate of cell
		/// @return count of `shape` entries in `lines`
		template<typename B>
		int CountFiguresPoints(const B &lines, const GomokuShape &shape, int row, int col) const;

	public:
	    /// @brief Default ctor
		Board();

	    /// @brief Costructs a board from a list of moves.
	    /// @param [in] moves list of moves
		explicit Board(const std::vector<pcell> &moves);

		/// @brief the board becomes new as default constructed
		void Reset();

		/// @brief Undo one move (2 semi-moves)
		/// @return `true` if success, `false` if not
		bool TakeBackMove();

        /// @brief function with obvious purpose, makes move depending on result of `WhiteMove_()`
        /// @param [in] move move to make
        /// @return
        /// `MoveResult::Default` - move was made successfully \n
		///	`MoveResult::WrongMove` - move was not made (move passed was not in `GetAvailableMoves()`) \n
		///	`MoveResult::WhiteWin` - move led to victory of white player \n
		///	`MoveResult::BlackWin` - move led to victory of black player \n
		///	`MoveResult::Draw` - move led to draw
		MoveResult MakeMove(pcell move);

		/// @brief converts internal representation of board cell to human one
		/// @param [in] move internal representation of board cell, e.g. `{0, 1}`,
		///     move MUST be a valid one otherwise behaviour is undefined
		/// @return human representation of a move, e.g. `"j10"`
		static std::string MoveToString(const pcell &move);

		/// @brief converts human representation of board cell to internal
		/// @param [in] s string representation, e.g. `"j10"`
		/// @return internal representation of a move, e.g. `{10, 3}`
		static pcell StringToMove(const std::string &s);

		/// @brief Counts how many `shape` are on the board
		/// @param [in] shape
		/// @return count of `shape` on the board
		[[nodiscard]] int CountFigureOverBoard(const GomokuShape &shape) const;

		/// @brief Checks if a `shape` on board
		/// @param [in] shape
		/// @return `true` if found, `false` if not
		[[nodiscard]] bool IsThereFigureOnBoard(const GomokuShape &shape) const;

		/// @brief Counts how many so-called free threes particular player has. \n
		/// Free three is a figure that can become a free four in just one move
		/// @param [in] side player side
		/// @return count of free threes
		[[nodiscard]] int CountFreeThrees(Side side) const;

		/// @brief Counts how many so-called flanked threes particular player has \n
		/// @param [in] side
		/// @return count of flanked threes
		[[nodiscard]] int CountHalfFreeThrees(Side side) const;

		/// @brief Counts how many so-called free threes were formed with `lastMove`
		/// @param [in] side player
		/// @param [in] lastMove "last move"
		/// @return count of free threes formed with `lastMove`
		[[nodiscard]] int CountFreeThreesLastMove(Side side, pcell lastMove) const;

		/// @brief Counts how many so-called free fours particular player has
		/// @param [in] side player
		/// @return count of free fours
		[[nodiscard]] int CountHalfFreeFours(Side side) const;

		/// @brief Checks if putting a `movePattern` in `cell` will trigger a capture
		/// @param [in] cell
		/// @param [in] movePattern
		/// @return `true` if will trigger `false` if not
        [[nodiscard]] bool IsMoveCapture(pcell cell, board_line movePattern) const;

		/// @brief Getter for list of moves made.
		/// @return vector of moves which were made during the game
		[[nodiscard]] const std::vector<pcell>& GetMovesList() const;

		/// @brief Hash method
		/// @return hashsum
		[[nodiscard]] size_t hash() const;

		/// @brief Who's turn?
		/// @return `true` if White's turn,
		///         `false` otherwise
		[[nodiscard]] bool WhiteMove() const;

		/// @brief Counts total amount of stones on the board
		/// @return count of stoned
		[[nodiscard]] int GetStoneCount() const;

		/// @brief Get list of moves that can be made on this move
		/// @return vector of available moves
		[[nodiscard]] const std::vector<pcell>& GetAvailableMoves() const;

        /// @brief What is on a particular cell?
        /// @param [in] cell cell
		/// @return `Side::None` if cell is empty \n
		///         `Side::White` if White stone \n
		///         `Side::Black` if Black stone \n
		[[nodiscard]] Side At(pcell cell) const;

		/// @brief What is on a particular cell?
		/// @param [in] row row of a cell
		/// @param [in] col column of a cell
		/// @return `Side::None` if cell is empty \n
		///         `Side::White` if White stone \n
		///         `Side::Black` if Black stone \n
		[[nodiscard]] Side At(int row, int col) const;

		/// @brief What is on a particular cell?
		/// @param [in] move string repr. of cell
		/// @return `Side::None` if cell is empty \n
        ///         `Side::White` if White stone \n
        ///         `Side::Black` if Black stone \n
		[[nodiscard]] Side At(const std::string& move) const;

		/// @brief How many capture points player has?
		/// @param [in] side Side::White of Side::Black, otherwise behaviour is undefined
		/// @return how many captures particular `side` made
		[[nodiscard]] int GetCapturePoints(Side side) const;

		/// @brief Get result code of last move
		/// @return result code of last move
		[[nodiscard]] MoveResult GetLastMoveResult() const;

        /// @brief Checks if particular cell has a stone in its locality of radius eps. \n
        /// Example:
        /// V is a target cell, XO are stones
        /// @code{}
        /// ...........
        /// ...........
        /// ....V...X..
        /// ........O..
        /// ...........
        ///  @endcode
        /// if `eps <= 3` return is `false`, if `eps > 3` return is `true`

        /// @param [in] cell cell
        /// @param [in] eps epsilon, locality "radius"
        /// @return `true` if has, `false` otherwise
        [[nodiscard]] bool IsCellHasStoneNearby(pcell cell, int eps=1) const;

        /// @brief Converts object to portable game notation string
        /// @return pgn string
		[[nodiscard]] std::string ToPgnString() const;

		[[nodiscard]] static int DistanceFromCenter(pcell cell);

		/// @brief output operator
		friend std::ostream& operator<<(std::ostream& os, const Board& bs);

		/// @brief input operator
		friend std::istream& operator>>(std::istream& is, Board& bs);

        /// @brief equ operator
        friend bool operator==(const Gomoku::Board& left, const Gomoku::Board& right);
	};
}

#endif //GOMOKU_BOARD_H
