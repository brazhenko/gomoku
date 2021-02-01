//
// Created by 17641238 on 19.01.2021.
//

#include "Board.h"
#include <vector>
#include "PGNGame.h"

Gomoku::BoardState::BoardState()
{
	for (int i = 0; i < 19; i++)
		for (int j = 0; j < 19; j++)
			this->available_moves.emplace(i, j);

	// Initialize mappings
	// Normal coordinates to projectcions

	// Vertical lines ||||
	for (int j = 0; j < cells_in_line; j++)
		for (int i = 0; i < cells_in_line; i++)
			_cToVerticles.insert({{i, j}, {j, i}});

	// Up Lines ////
	constexpr int diagonal_count = cells_in_line * 2 - 1;
	for (int line = 1; line <= diagonal_count; line++)
	{
		int start_col = std::max(0, line - cells_in_line);
		int count = std::min(line, std::min((cells_in_line - start_col), cells_in_line));

		for (int j = 0; j < count; j++)
			_cToUpLines.insert({{(std::min(cells_in_line, line)-j-1), (start_col+j)}, {(line - 1), j}});
	}

	// Down Lines \\\\  |f
	for (int line = 1; line <= diagonal_count; line++)
	{
		int start_col = std::max(0, line - cells_in_line);
		int count = std::min(line, std::min((cells_in_line - start_col), cells_in_line));

		for (int j = 0; j < count; j++)
			_cToDownLines.insert({{(std::min(cells_in_line, line)-j-1), (cells_in_line - 1 - (start_col + j))}, {(line - 1), j}});
	}


}

Gomoku::BoardState::BoardState(const std::vector<std::pair<int, int>> &moves)
	: BoardState()
{
	auto t1 = std::chrono::high_resolution_clock::now();

	for (const auto &move : moves)
		if (!this->MakeMove(move.first, move.second))
		{
			std::stringstream ss;

			ss << "Trying to construct board with wrong move: " << move.first << " " << move.second;
			throw std::runtime_error(ss.str());
		}


	auto t2 = std::chrono::high_resolution_clock::now();
	auto duration = std::chrono::duration_cast<std::chrono::milliseconds>( t2 - t1 ).count();

	std::cout << duration << std::endl;


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

	const auto &upC = _cToUpLines.at({row, col});
	const auto &downC = _cToDownLines.at({row, col});

	if (WhiteMove())
	{
		freeThreesCount += CountFiguresPoints(board_, figure_free_three1_w, row, col);
		freeThreesCount += CountFiguresPoints(vertical_, figure_free_three1_w, col, row);
		freeThreesCount += CountFiguresPoints(up_lines_, figure_free_three1_w, upC.first, upC.second);
		freeThreesCount += CountFiguresPoints(down_lines_, figure_free_three1_w, downC.first, downC.second);

		freeThreesCount += CountFiguresPoints(board_, figure_free_three2_w, row, col);
		freeThreesCount += CountFiguresPoints(vertical_, figure_free_three2_w, col, row);
		freeThreesCount += CountFiguresPoints(up_lines_, figure_free_three2_w, upC.first, upC.second);
		freeThreesCount += CountFiguresPoints(down_lines_, figure_free_three2_w, downC.first, downC.second);

		freeThreesCount += CountFiguresPoints(board_, figure_free_three3_w, row, col);
		freeThreesCount += CountFiguresPoints(vertical_, figure_free_three3_w, col, row);
		freeThreesCount += CountFiguresPoints(up_lines_, figure_free_three3_w, upC.first, upC.second);
		freeThreesCount += CountFiguresPoints(down_lines_, figure_free_three3_w, downC.first, downC.second);

		freeThreesCount += CountFiguresPoints(board_, figure_free_three4_w, row, col);
		freeThreesCount += CountFiguresPoints(vertical_, figure_free_three4_w, col, row);
		freeThreesCount += CountFiguresPoints(up_lines_, figure_free_three4_w, upC.first, upC.second);
		freeThreesCount += CountFiguresPoints(down_lines_, figure_free_three4_w, downC.first, downC.second);

		freeThreesCount += CountFiguresPoints(board_, figure_free_three5_w, row, col);
		freeThreesCount += CountFiguresPoints(vertical_, figure_free_three5_w, col, row);
		freeThreesCount += CountFiguresPoints(up_lines_, figure_free_three5_w, upC.first, upC.second);
		freeThreesCount += CountFiguresPoints(down_lines_, figure_free_three5_w, downC.first, downC.second);
	}
	else
	{
		freeThreesCount += CountFiguresPoints(board_, figure_free_three1_b, row, col);
		freeThreesCount += CountFiguresPoints(vertical_, figure_free_three1_b, col, row);
		freeThreesCount += CountFiguresPoints(up_lines_, figure_free_three1_b, upC.first, upC.second);
		freeThreesCount += CountFiguresPoints(down_lines_, figure_free_three1_b, downC.first, downC.second);

		freeThreesCount += CountFiguresPoints(board_, figure_free_three2_b, row, col);
		freeThreesCount += CountFiguresPoints(vertical_, figure_free_three2_b, col, row);
		freeThreesCount += CountFiguresPoints(up_lines_, figure_free_three2_b, upC.first, upC.second);
		freeThreesCount += CountFiguresPoints(down_lines_, figure_free_three2_b, downC.first, downC.second);

		freeThreesCount += CountFiguresPoints(board_, figure_free_three3_b, row, col);
		freeThreesCount += CountFiguresPoints(vertical_, figure_free_three3_b, col, row);
		freeThreesCount += CountFiguresPoints(up_lines_, figure_free_three3_b, upC.first, upC.second);
		freeThreesCount += CountFiguresPoints(down_lines_, figure_free_three3_b, downC.first, downC.second);

		freeThreesCount += CountFiguresPoints(board_, figure_free_three4_b, row, col);
		freeThreesCount += CountFiguresPoints(vertical_, figure_free_three4_b, col, row);
		freeThreesCount += CountFiguresPoints(up_lines_, figure_free_three4_b, upC.first, upC.second);
		freeThreesCount += CountFiguresPoints(down_lines_, figure_free_three4_b, downC.first, downC.second);

		freeThreesCount += CountFiguresPoints(board_, figure_free_three5_b, row, col);
		freeThreesCount += CountFiguresPoints(vertical_, figure_free_three5_b, col, row);
		freeThreesCount += CountFiguresPoints(up_lines_, figure_free_three5_b, upC.first, upC.second);
		freeThreesCount += CountFiguresPoints(down_lines_, figure_free_three5_b, downC.first, downC.second);
	}

	return freeThreesCount;
}

void Gomoku::BoardState::FindMovesBreaksFifth()
{
	for (int i = 0; i < cells_in_line; i++)
	{
		for (int j = 0; j < cells_in_line; j++)
		{
			if (Side::None == At(i, j))
			{
				// Check if move valid
				if (IsMoveCapture(i, j))
				{
					auto captured = MakeCapture(i, j);
					int fifthCount = 0;

					if (!captured.empty())
					{
						if (!WhiteMove())
						{
							fifthCount += CountFigures(board_, figure_five_w);
							fifthCount += CountFigures(vertical_, figure_five_w);
							fifthCount += CountFigures(up_lines_, figure_five_w);
							fifthCount += CountFigures(down_lines_, figure_five_w);
							WhiteCapturePoints -= (captured.size() / 2);
						}
						else
						{
							fifthCount += CountFigures(board_, figure_five_b);
							fifthCount += CountFigures(vertical_, figure_five_b);
							fifthCount += CountFigures(up_lines_, figure_five_b);
							fifthCount += CountFigures(down_lines_, figure_five_b);

							BlackCapturePoints -= (captured.size() / 2);
						}

						// Capture destroys fifth
						if (fifthCount == 0)
							available_moves.emplace(i, j);

						for (const auto &c : captured)
							Set(c.first, c.second, Side(movePattern.to_ulong() ^ 0b11U));
					}

				}
			}
		}
	}
}

std::vector<std::pair<int, int>> Gomoku::BoardState::MakeCapture(int row, int col)
{
	std::vector<std::pair<int, int>> capturedStones;

	// capture pair up
	if (row + 3 < 19
		&& int(this->At(row + 1, col)) == (movePattern.to_ulong() ^ 0b11U)
		&& int(this->At(row + 2, col)) == (movePattern.to_ulong() ^ 0b11U)
		&& int(this->At(row + 3, col)) == (movePattern.to_ulong())
			) {

		Set(row + 1, col, Side::None);
		capturedStones.emplace_back(row + 1, col);
		Set(row + 2, col, Side::None);
		capturedStones.emplace_back(row + 2, col);

		if (WhiteMove())
			WhiteCapturePoints++;
		else
			BlackCapturePoints++;
	}

	// capture pair up right
	if (row + 3 < 19
		&& col + 3 < 19
		&& int(this->At(row + 1, col + 1)) == (movePattern.to_ulong() ^ 0b11U)
		&& int(this->At(row + 2, col + 2)) == (movePattern.to_ulong() ^ 0b11U)
		&& int(this->At(row + 3, col + 3)) == (movePattern.to_ulong())
			) {

		Set(row + 1, col + 1, Side::None);
		capturedStones.emplace_back(row + 1, col + 1);
		Set(row + 2, col + 2, Side::None);
		capturedStones.emplace_back(row + 2, col + 2);

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
		capturedStones.emplace_back(row, col + 1);
		Set(row, col + 2, Side::None);
		capturedStones.emplace_back(row, col + 2);

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
		capturedStones.emplace_back(row - 1, col + 1);
		Set(row - 2, col + 2, Side::None);
		capturedStones.emplace_back(row - 2, col + 2);

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
		capturedStones.emplace_back(row - 1, col);
		Set(row - 2, col, Side::None);
		capturedStones.emplace_back(row - 2, col);

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
		capturedStones.emplace_back(row - 1, col - 1);
		Set(row - 2, col - 2, Side::None);
		capturedStones.emplace_back(row - 2, col - 2);

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
		capturedStones.emplace_back(row, col - 1);
		Set(row, col - 2, Side::None);
		capturedStones.emplace_back(row, col - 2);

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
		capturedStones.emplace_back(row + 1, col - 1);
		Set(row + 2, col - 2, Side::None);
		capturedStones.emplace_back(row + 2, col - 2);

		if (WhiteMove())
			WhiteCapturePoints++;
		else
			BlackCapturePoints++;
	}

	return capturedStones;
}

bool Gomoku::BoardState::MakeMove(int row, int col)
{
	// Add to move history
	moves_.emplace_back(row, col);
	// Put stone on board
	Set(row, col, Side(movePattern.to_ulong()));
	// Delete cell from available ones

	// Make captures if exist
	MakeCapture(row, col);

	available_moves = {};

	if (WhiteMove() && WhiteCapturePoints >= 5)
		return true;
	if (!WhiteMove() && BlackCapturePoints >= 5)
		return true;
	int fifthCount = 0;

	const auto &upC = _cToUpLines.at({row, col});
	const auto &downC = _cToDownLines.at({row, col});

	if (WhiteMove())
	{
		fifthCount += CountFiguresPoints(board_, figure_five_w, row, col);
		fifthCount += CountFiguresPoints(vertical_, figure_five_w, col, row);
		fifthCount += CountFiguresPoints(up_lines_, figure_five_w, upC.first, upC.second);
		fifthCount += CountFiguresPoints(down_lines_, figure_five_w, downC.first, downC.second);
	}
	else
	{
		fifthCount += CountFiguresPoints(board_, figure_five_b, row, col);
		fifthCount += CountFiguresPoints(vertical_, figure_five_b, col, row);
		fifthCount += CountFiguresPoints(up_lines_, figure_five_b, upC.first, upC.second);
		fifthCount += CountFiguresPoints(down_lines_, figure_five_b, downC.first, downC.second);
	}

	// Change move
	movePattern ^= 0b11;

	if (fifthCount > 0)
	{
		// Form avalable ending moves
		FindMovesBreaksFifth();
		std::cout << "move: " << Gomoku::BoardState::MoveToString({row, col}) << std::endl;
		for (const auto &move: available_moves)
			std::cout << Gomoku::BoardState::MoveToString(move) << ";  ";

		std::cout << std::endl;
		return true;
	}


	// Form available moves for next turn
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

				// Pretend to make move
				Set(i, j, Side(movePattern.to_ulong()));

				if (WhiteMove())
					newFreeThreesCount = CountFreeThrees(Side::White, {i, j});
				else
					newFreeThreesCount = CountFreeThrees(Side::Black, {i, j});

				// Two or more free threes NOT produced
				// if (!(newFreeThreesCount > freeThreesCount + 1))
				if (newFreeThreesCount < 2)
					available_moves.emplace(i, j);

				// Return back pretended move
				Set(i, j, Side::None);
			}
		}
	}

	return true;
}

std::string Gomoku::BoardState::ToPgnString() const
{
	std::stringstream ss;


	pgn::TagList tl;

	tl.insert(pgn::Tag("Game", "Gomoku"));
	tl.insert(pgn::Tag("Variant", "42"));

	pgn::MoveList ml;
	pgn::GameResult gr;


	const auto& moves = GetMovesList();

	for (int i = 0; i < moves.size(); i += 2)
	{

		auto a = pgn::Move(
				pgn::Ply(Gomoku::BoardState::MoveToString(moves[i])),
				(i + 1 < moves.size()) ? pgn::Ply(Gomoku::BoardState::MoveToString(moves[i+1])) : pgn::Ply(""),
				(i/2) + 1);
		ml.push_back(
				a
				);

		// TODO delete
		std::cout << Gomoku::BoardState::MoveToString(moves[i]) << ",  ";
		std::cout << ((i + 1 < moves.size()) ? Gomoku::BoardState::MoveToString(moves[i+1]) : "" )<< std::endl;
	}

	ss << pgn::Game(tl, ml, gr) << std::endl;

	return ss.str();
}
