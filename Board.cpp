//
// Created by 17641238 on 19.01.2021.
//

#include "Board.h"
#include <vector>


Gomoku::BoardState::BoardState()
{
	for (int i = 0; i < 19; i++)
		for (int j = 0; j < 19; j++)
		{
			this->available_moves.emplace(i, j);
		}


}


Gomoku::BoardState::BoardState(const std::vector<std::pair<int, int>> &moves) {
	BoardState();

	for (const auto &move : moves)
		if (!this->MakeMove(move.first, move.second))
		{
			std::stringstream ss;

			ss << "Trying to construct board with wrong move: " << move.first << " " << move.second;
			throw std::runtime_error(ss.str());
		}

}

bool Gomoku::BoardState::IsMoveCapture(int row, int col) const
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

int Gomoku::BoardState::CountFreeThrees(Gomoku::BoardState::Side side, std::pair<int, int> lastMove) const
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

bool Gomoku::BoardState::MakeMove(int row, int col)
{
	// Add to move history
	moves_.emplace_back(row, col);
	// Put stone on board
	board_[row] |= (movePattern << (col * 2));
	// Delete cell from available ones

	// Detect captures
	{
		// capture pair up
		if (row + 3 < 19
			&& int(this->At(row + 1, col)) == (movePattern.to_ulong() ^ 0b11U)
			&& int(this->At(row + 2, col)) == (movePattern.to_ulong() ^ 0b11U)
			&& int(this->At(row + 3, col)) == (movePattern.to_ulong())
				) {

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

void Gomoku::BoardState::PrepareHelpLines() const
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

