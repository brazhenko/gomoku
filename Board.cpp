#pragma clang diagnostic push
#pragma ide diagnostic ignored "hicpp-signed-bitwise"

//
// Created by Brazhenko Andrew on 19.01.2021.
//

#include "Board.h"
#include <vector>
#include "PGNGame.h"
#include <algorithm>
#include <sstream>

const std::unordered_map<Gomoku::Board::pcell, Gomoku::Board::pcell, Gomoku::Board::PairHash> Gomoku::Board::_cToVerticles = [](){
	std::unordered_map<Gomoku::Board::pcell, Gomoku::Board::pcell, PairHash> ret;
	for (int j = 0; j < cells_in_line; j++)
		for (int i = 0; i < cells_in_line; i++)
			ret.insert({{i, j}, {j, i}});

	return ret;
}();

const std::unordered_map<Gomoku::Board::pcell, Gomoku::Board::pcell, Gomoku::Board::PairHash> Gomoku::Board::_cToUpLines = [](){
	std::unordered_map<Gomoku::Board::pcell, Gomoku::Board::pcell, PairHash> ret;
	constexpr int diagonal_count = cells_in_line * 2 - 1;
	for (int line = 1; line <= diagonal_count; line++)
	{
		int start_col = std::max(0, line - cells_in_line);
		int count = std::min(line, std::min((cells_in_line - start_col), cells_in_line));

		for (int j = 0; j < count; j++)
			ret.insert({{(std::min(cells_in_line, line)-j-1), (start_col+j)}, {(line - 1), j}});
	}
	return ret;
}();

const std::unordered_map<Gomoku::Board::pcell, Gomoku::Board::pcell, Gomoku::Board::PairHash> Gomoku::Board::_cToDownLines = [](){
	std::unordered_map<Gomoku::Board::pcell, Gomoku::Board::pcell, PairHash> ret;
	constexpr int diagonal_count = cells_in_line * 2 - 1;
	for (int line = 1; line <= diagonal_count; line++)
	{
		int start_col = std::max(0, line - cells_in_line);
		int count = std::min(line, std::min((cells_in_line - start_col), cells_in_line));

		for (int j = 0; j < count; j++)
			ret.insert({{(std::min(cells_in_line, line)-j-1), (cells_in_line - 1 - (start_col + j))}, {(line - 1), j}});
	}
	return ret;
}();

Gomoku::Board::Board()
{
// Выбрать какой способ генерации доступных ходов сделать.

	// Этот
	for (int i = 0; i < cells_in_line; i++)
		for (int j = 0; j < cells_in_line; j++)
			this->availableMoves_.emplace_back(i, j);

	// Или этот
// 	GenerateAvailableMovesInternal();
}


Gomoku::Board::Board(const std::vector<Gomoku::Board::pcell> &moves)
	// Construct default board
	: Board()
{
	auto t1 = std::chrono::high_resolution_clock::now();

	for (const auto &move : moves)
	{
		if (std::find(GetAvailableMoves().begin(), GetAvailableMoves().end(), move) == GetAvailableMoves().end()) // if move is not available
		{
			std::stringstream ss;

			ss << "Trying to construct board with wrong move: " << Board::MoveToString(move);
			throw std::runtime_error(ss.str());
		}

		this->MakeMove(move);
	}
}

bool Gomoku::Board::IsMoveCapture(pcell cell, Gomoku::Board::board_line mvPtrn) const
{
    const int& row = cell.first;
    const int& col = cell.second;

	return
			(row + 3 < 19
			 && int(this->At(row + 1, col)) == (mvPtrn.to_ulong() ^ 0b11U)
			 && int(this->At(row + 2, col)) == (mvPtrn.to_ulong() ^ 0b11U)
			 && int(this->At(row + 3, col)) == (mvPtrn.to_ulong())
			)
			|| (row + 3 < 19
				&& col + 3 < 19
				&& int(this->At(row + 1, col + 1)) == (mvPtrn.to_ulong() ^ 0b11U)
				&& int(this->At(row + 2, col + 2)) == (mvPtrn.to_ulong() ^ 0b11U)
				&& int(this->At(row + 3, col + 3)) == (mvPtrn.to_ulong())
			)
			|| (col + 3 < 19
				&& int(this->At(row, col + 1)) == (mvPtrn.to_ulong() ^ 0b11U)
				&& int(this->At(row, col + 2)) == (mvPtrn.to_ulong() ^ 0b11U)
				&& int(this->At(row, col + 3)) == (mvPtrn.to_ulong())
			)
			|| (row - 3 >= 0
				&& col + 3 < 19
				&& int(this->At(row - 1, col + 1)) == (mvPtrn.to_ulong() ^ 0b11U)
				&& int(this->At(row - 2, col + 2)) == (mvPtrn.to_ulong() ^ 0b11U)
				&& int(this->At(row - 3, col + 3)) == (mvPtrn.to_ulong())
			)
			||  (row - 3 >= 0
				 && int(this->At(row - 1, col)) == (mvPtrn.to_ulong() ^ 0b11U)
				 && int(this->At(row - 2, col)) == (mvPtrn.to_ulong() ^ 0b11U)
				 && int(this->At(row - 3, col)) == (mvPtrn.to_ulong())
			)
			|| (row - 3 >= 0
				&& col - 3 >= 0
				&& int(this->At(row - 1, col - 1)) == (mvPtrn.to_ulong() ^ 0b11U)
				&& int(this->At(row - 2, col - 2)) == (mvPtrn.to_ulong() ^ 0b11U)
				&& int(this->At(row - 3, col - 3)) == (mvPtrn.to_ulong())
			)
			|| (col - 3 >= 0
				&& int(this->At(row, col - 1)) == (mvPtrn.to_ulong() ^ 0b11U)
				&& int(this->At(row, col - 2)) == (mvPtrn.to_ulong() ^ 0b11U)
				&& int(this->At(row, col - 3)) == (mvPtrn.to_ulong())
			)
			|| (row + 3 < 19
				&& col - 3 >= 0
				&& int(this->At(row + 1, col - 1)) == (mvPtrn.to_ulong() ^ 0b11U)
				&& int(this->At(row + 2, col - 2)) == (mvPtrn.to_ulong() ^ 0b11U)
				&& int(this->At(row + 3, col - 3)) == (mvPtrn.to_ulong())
			);
}

int Gomoku::Board::CountFreeThreesLastMove(Gomoku::Board::Side side, Gomoku::Board::pcell lastMove) const
{
	int freeThreesCount = 0;

	const auto& row = lastMove.first;
	const auto& col = lastMove.second;

	const auto &upC = _cToUpLines.at({row, col});
	const auto &downC = _cToDownLines.at({row, col});

	int accHorizon = 0;
    for (int i = std::max(col - 2, 0); i < std::min(col + 3, cells_in_line); i++)
        accHorizon += (At(row, i) == side);

	int accVertical = 0;
    for (int i = std::max(row - 2, 0); i < std::min(row + 3, cells_in_line); i++)
        accVertical += (At(i, col) == side);

	int accUplines = 0;
    for (int i = std::max(upC.second - 2, 0); i < std::min(upC.second + 3, cells_in_line); i++)
        accUplines += ((Side((upLines_[upC.first] >> (i * bits_per_cell) & board_line {0b11}).to_ulong()) == side));

	int accDownlines = 0;
    for (int i = std::max(downC.second - 2, 0); i < std::min(downC.second + 3, cells_in_line); i++)
        accDownlines += ((Side((downLines_[downC.first] >> (i * bits_per_cell) & board_line {0b11}).to_ulong()) == side));


	if (Board::Side::White == side)
	{
	    if (accHorizon > 1)
        {
            freeThreesCount += CountFiguresPoints(board_, figure_free_three2_w, row, col);
            freeThreesCount += CountFiguresPoints(board_, figure_free_three3_w, row, col);
            freeThreesCount -= CountFiguresPoints(board_, figure_free_three1_w, row, col);
            freeThreesCount += CountFiguresPoints(board_, figure_free_three4_w, row, col);
            freeThreesCount += CountFiguresPoints(board_, figure_free_three5_w, row, col);
        }

	    if (accVertical > 1)
        {
            freeThreesCount += CountFiguresPoints(vertical_, figure_free_three2_w, col, row);
            freeThreesCount += CountFiguresPoints(vertical_, figure_free_three3_w, col, row);
            freeThreesCount -= CountFiguresPoints(vertical_, figure_free_three1_w, col, row);
            freeThreesCount += CountFiguresPoints(vertical_, figure_free_three4_w, col, row);
            freeThreesCount += CountFiguresPoints(vertical_, figure_free_three5_w, col, row);
        }

        if (accUplines > 1)
        {
            freeThreesCount += CountFiguresPoints(upLines_, figure_free_three2_w, upC.first, upC.second);
            freeThreesCount += CountFiguresPoints(upLines_, figure_free_three3_w, upC.first, upC.second);
            freeThreesCount -= CountFiguresPoints(upLines_, figure_free_three1_w, upC.first, upC.second);
            freeThreesCount += CountFiguresPoints(upLines_, figure_free_three4_w, upC.first, upC.second);
            freeThreesCount += CountFiguresPoints(upLines_, figure_free_three5_w, upC.first, upC.second);
        }

        if (accDownlines > 1)
        {
            freeThreesCount += CountFiguresPoints(downLines_, figure_free_three2_w, downC.first, downC.second);
            freeThreesCount += CountFiguresPoints(downLines_, figure_free_three3_w, downC.first, downC.second);
            freeThreesCount -= CountFiguresPoints(downLines_, figure_free_three1_w, downC.first, downC.second);
            freeThreesCount += CountFiguresPoints(downLines_, figure_free_three4_w, downC.first, downC.second);
            freeThreesCount += CountFiguresPoints(downLines_, figure_free_three5_w, downC.first, downC.second);
        }
	}
	else if (Board::Side::Black == side)
	{
        if (accHorizon > 1)
        {
            freeThreesCount += CountFiguresPoints(board_, figure_free_three2_b, row, col);
            freeThreesCount += CountFiguresPoints(board_, figure_free_three3_b, row, col);
            freeThreesCount -= CountFiguresPoints(board_, figure_free_three1_b, row, col);
            freeThreesCount += CountFiguresPoints(board_, figure_free_three4_b, row, col);
            freeThreesCount += CountFiguresPoints(board_, figure_free_three5_b, row, col);
        }

        if (accVertical > 1)
        {
            freeThreesCount += CountFiguresPoints(vertical_, figure_free_three2_b, col, row);
            freeThreesCount += CountFiguresPoints(vertical_, figure_free_three3_b, col, row);
            freeThreesCount -= CountFiguresPoints(vertical_, figure_free_three1_b, col, row);
            freeThreesCount += CountFiguresPoints(vertical_, figure_free_three4_b, col, row);
            freeThreesCount += CountFiguresPoints(vertical_, figure_free_three5_b, col, row);
        }

        if (accUplines > 1)
        {
            freeThreesCount += CountFiguresPoints(upLines_, figure_free_three2_b, upC.first, upC.second);
            freeThreesCount += CountFiguresPoints(upLines_, figure_free_three3_b, upC.first, upC.second);
            freeThreesCount -= CountFiguresPoints(upLines_, figure_free_three1_b, upC.first, upC.second);
            freeThreesCount += CountFiguresPoints(upLines_, figure_free_three4_b, upC.first, upC.second);
            freeThreesCount += CountFiguresPoints(upLines_, figure_free_three5_b, upC.first, upC.second);
        }

        if (accDownlines > 1)
        {
            freeThreesCount += CountFiguresPoints(downLines_, figure_free_three2_b, downC.first, downC.second);
            freeThreesCount += CountFiguresPoints(downLines_, figure_free_three3_b, downC.first, downC.second);
            freeThreesCount -= CountFiguresPoints(downLines_, figure_free_three1_b, downC.first, downC.second);
            freeThreesCount += CountFiguresPoints(downLines_, figure_free_three4_b, downC.first, downC.second);
            freeThreesCount += CountFiguresPoints(downLines_, figure_free_three5_b, downC.first, downC.second);
        }
	}

	return freeThreesCount;
}

void Gomoku::Board::FindMovesBreaksFifthInternal()
{
	for (int i = 0; i < cells_in_line; i++)
		for (int j = 0; j < cells_in_line; j++)
		{
			// Valid move is an empty cell
			if (Side::None != At(i, j))
				continue;

			// Valid move can be only a capture one
			if (!IsMoveCapture({i, j}, movePattern_))
				continue;

			// Pretending capture
			auto captured = MakeCaptureInternal({i, j});
			if (captured.empty())
				continue;

			int fifthCount = 0;

			if (!WhiteMove())
			{
				fifthCount += CountFigures(board_, figure_five_w);
				fifthCount += CountFigures(vertical_, figure_five_w);
				fifthCount += CountFigures(upLines_, figure_five_w, true);
				fifthCount += CountFigures(downLines_, figure_five_w, true);
                BlackCapturePoints_ -= (int(captured.size()) / 2);
			}
			else
			{
				fifthCount += CountFigures(board_, figure_five_b);
				fifthCount += CountFigures(vertical_, figure_five_b);
				fifthCount += CountFigures(upLines_, figure_five_b, true);
				fifthCount += CountFigures(downLines_, figure_five_b, true);
                WhiteCapturePoints_ -= (int(captured.size()) / 2);
			}

			// Capture destroys five => add it to available moves list
			if (fifthCount == 0)
				availableMoves_.emplace_back(i, j);

			// Return all `captured` stones back
			for (const auto &c : captured)
				SetStoneInternal(c.first, c.second, Side(movePattern_.to_ulong() ^ 0b11U));

		}
}

std::vector<Gomoku::Board::pcell> Gomoku::Board::MakeCaptureInternal(pcell move)
{
	std::vector<pcell> capturedStones;
    const int row = move.first, col = move.second;
    const int centerStone = int(At(move));

	// capture pair up
	if (row + 3 < 19
		&& int(this->At(row + 1, col)) == (centerStone ^ 0b11U)
		&& int(this->At(row + 2, col)) == (centerStone ^ 0b11U)
		&& int(this->At(row + 3, col)) == centerStone
			) {

		SetStoneInternal(row + 1, col, Side::None);
		capturedStones.emplace_back(row + 1, col);
		SetStoneInternal(row + 2, col, Side::None);
		capturedStones.emplace_back(row + 2, col);

		if (WhiteMove())
			WhiteCapturePoints_++;
		else
			BlackCapturePoints_++;
	}

	// capture pair up right
	if (row + 3 < 19
		&& col + 3 < 19
		&& int(this->At(row + 1, col + 1)) == (centerStone ^ 0b11U)
		&& int(this->At(row + 2, col + 2)) == (centerStone ^ 0b11U)
		&& int(this->At(row + 3, col + 3)) == centerStone
			) {

		SetStoneInternal(row + 1, col + 1, Side::None);
		capturedStones.emplace_back(row + 1, col + 1);
		SetStoneInternal(row + 2, col + 2, Side::None);
		capturedStones.emplace_back(row + 2, col + 2);

		if (WhiteMove())
			WhiteCapturePoints_++;
		else
			BlackCapturePoints_++;
	}

	// capture pair right
	if (col + 3 < 19
		&& int(this->At(row, col + 1)) == (centerStone ^ 0b11U)
		&& int(this->At(row, col + 2)) == (centerStone ^ 0b11U)
		&& int(this->At(row, col + 3)) == (centerStone))
	{

		SetStoneInternal(row, col + 1, Side::None);
		capturedStones.emplace_back(row, col + 1);
		SetStoneInternal(row, col + 2, Side::None);
		capturedStones.emplace_back(row, col + 2);

		if (WhiteMove())
			WhiteCapturePoints_++;
		else
			BlackCapturePoints_++;
	}

	// capture pair down right
	if (row - 3 >= 0
		&& col + 3 < 19
		&& int(this->At(row - 1, col + 1)) == (centerStone ^ 0b11U)
		&& int(this->At(row - 2, col + 2)) == (centerStone ^ 0b11U)
		&& int(this->At(row - 3, col + 3)) == (centerStone))
	{

		SetStoneInternal(row - 1, col + 1, Side::None);
		capturedStones.emplace_back(row - 1, col + 1);
		SetStoneInternal(row - 2, col + 2, Side::None);
		capturedStones.emplace_back(row - 2, col + 2);

		if (WhiteMove())
			WhiteCapturePoints_++;
		else
			BlackCapturePoints_++;
	}

	// capture pair down
	if (row - 3 >= 0
		&& int(this->At(row - 1, col)) == (centerStone ^ 0b11U)
		&& int(this->At(row - 2, col)) == (centerStone ^ 0b11U)
		&& int(this->At(row - 3, col)) == (centerStone)) {

		SetStoneInternal(row - 1, col, Side::None);
		capturedStones.emplace_back(row - 1, col);
		SetStoneInternal(row - 2, col, Side::None);
		capturedStones.emplace_back(row - 2, col);

		if (WhiteMove())
			WhiteCapturePoints_++;
		else
			BlackCapturePoints_++;
	}

	// capture pair down left
	if (row - 3 >= 0
		&& col - 3 >= 0
		&& int(this->At(row - 1, col - 1)) == (centerStone ^ 0b11U)
		&& int(this->At(row - 2, col - 2)) == (centerStone ^ 0b11U)
		&& int(this->At(row - 3, col - 3)) == (centerStone))
	{

		SetStoneInternal(row - 1, col - 1, Side::None);
		capturedStones.emplace_back(row - 1, col - 1);
		SetStoneInternal(row - 2, col - 2, Side::None);
		capturedStones.emplace_back(row - 2, col - 2);

		if (WhiteMove())
			WhiteCapturePoints_++;
		else
			BlackCapturePoints_++;
	}

	// capture pair left
	if (col - 3 >= 0
		&& int(this->At(row, col - 1)) == (centerStone ^ 0b11U)
		&& int(this->At(row, col - 2)) == (centerStone ^ 0b11U)
		&& int(this->At(row, col - 3)) == (centerStone))
	{

		SetStoneInternal(row, col - 1, Side::None);
		capturedStones.emplace_back(row, col - 1);
		SetStoneInternal(row, col - 2, Side::None);
		capturedStones.emplace_back(row, col - 2);

		if (WhiteMove())
			WhiteCapturePoints_++;
		else
			BlackCapturePoints_++;
	}

	// capture pair up left
	if (row + 3 < 19
		&& col - 3 >= 0
		&& int(this->At(row + 1, col - 1)) == (centerStone ^ 0b11U)
		&& int(this->At(row + 2, col - 2)) == (centerStone ^ 0b11U)
		&& int(this->At(row + 3, col - 3)) == (centerStone))
	{

		SetStoneInternal(row + 1, col - 1, Side::None);
		capturedStones.emplace_back(row + 1, col - 1);
		SetStoneInternal(row + 2, col - 2, Side::None);
		capturedStones.emplace_back(row + 2, col - 2);

		if (WhiteMove())
			WhiteCapturePoints_++;
		else
			BlackCapturePoints_++;
	}

	return capturedStones;
}

Gomoku::Board::MoveResult Gomoku::Board::MakeMove(pcell move)
{
	auto ret = MakeMoveInternal(move.first, move.second);
	this->lastMoveResult_ = ret;
	return ret;
}

Gomoku::Board::MoveResult Gomoku::Board::MakeMoveInternal(int row, int col)
{
	if (std::find(availableMoves_.begin(), availableMoves_.end(), std::make_pair(row, col)) == availableMoves_.end())
		return Board::MoveResult::WrongMove;

	MoveResult ret = MoveResult::Default;
	// Add to move history
	moves_.emplace_back(row, col);
	// Put stone on board
	SetStoneInternal(row, col, Side(movePattern_.to_ulong()));
	// Delete cell from available ones

	// Make captures if exist
	if (!MakeCaptureInternal({row, col}).empty())
		ret = MoveResult::Capture;

	availableMoves_ = {};

	if (WhiteMove() && WhiteCapturePoints_ >= 5)
		return MoveResult::WhiteWin;
	if (!WhiteMove() && BlackCapturePoints_ >= 5)
		return MoveResult::BlackWin;
	int fivesCount = 0;

	const auto &upC = _cToUpLines.at({row, col});
	const auto &downC = _cToDownLines.at({row, col});

	if (WhiteMove())
	{
		fivesCount += CountFiguresPoints(board_, figure_five_w, row, col);
		fivesCount += CountFiguresPoints(vertical_, figure_five_w, col, row);
		fivesCount += CountFiguresPoints(upLines_, figure_five_w, upC.first, upC.second);
		fivesCount += CountFiguresPoints(downLines_, figure_five_w, downC.first, downC.second);
	}
	else
	{
		fivesCount += CountFiguresPoints(board_, figure_five_b, row, col);
		fivesCount += CountFiguresPoints(vertical_, figure_five_b, col, row);
		fivesCount += CountFiguresPoints(upLines_, figure_five_b, upC.first, upC.second);
		fivesCount += CountFiguresPoints(downLines_, figure_five_b, downC.first, downC.second);
	}

	// Change move
	movePattern_ ^= 0b11;

	if (fivesCount > 0)
	{
		// Form avalable ending moves
		FindMovesBreaksFifthInternal();

		if (availableMoves_.empty())
		{
			if (WhiteMove())
				return MoveResult::BlackWin;
			return MoveResult::WhiteWin;
		}
		return ret;
	}

	GenerateAvailableMovesInternal();

	if (availableMoves_.empty())
		return MoveResult::Draw;

	return ret;
}

std::string Gomoku::Board::ToPgnString() const
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
				pgn::Ply(Gomoku::Board::MoveToString(moves[i])),
                (i + 1 < moves.size()) ? pgn::Ply(Gomoku::Board::MoveToString(moves[i + 1])) : pgn::Ply(""),
				(i/2) + 1);

		ml.push_back(a);
	}

	ss << pgn::Game(tl, ml, gr) << std::endl;

	return ss.str();
}

size_t Gomoku::Board::hash() const
{
	std::size_t seed = board_.size();

	for(const auto& i : board_)
		seed ^= std::hash<board_line>{}(i);

	return seed;
}

bool Gomoku::Board::TakeBackMove()
{
	if (moves_.size() < 2) return false;

	moves_.pop_back();
	moves_.pop_back();

	*this = Board(moves_);

	return true;
}

void Gomoku::Board::Reset()
{
	*this = Board();
}

void Gomoku::Board::SetStoneInternal(int row, int col, Gomoku::Board::Side s)
{
	const auto &verticle = _cToVerticles.at({row, col});
	const auto &upline = _cToUpLines.at({row, col});
	const auto &downline = _cToDownLines.at({row, col});

	board_[row][col * 2] = unsigned(s)&1U;
	board_[row][col * 2 + 1] = (unsigned(s)>>1U)&1U;

	vertical_[verticle.first][verticle.second * 2] = unsigned(s)&1U;
	vertical_[verticle.first][verticle.second * 2 + 1] = (unsigned(s)>>1U)&1U;

	upLines_[upline.first][upline.second * 2] = unsigned(s) & 1U;
	upLines_[upline.first][upline.second * 2 + 1] = (unsigned(s) >> 1U) & 1U;

	downLines_[downline.first][downline.second * 2] = unsigned(s) & 1U;
	downLines_[downline.first][downline.second * 2 + 1] = (unsigned(s) >> 1U) & 1U;
}

Gomoku::Board::Side Gomoku::Board::At(pcell cell) const
{
	const int row = cell.first;
	const int col = cell.second;

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

Gomoku::Board::Side Gomoku::Board::At(int row, int col) const
{
	return At(std::make_pair(row, col));
}

int Gomoku::Board::GetCapturePoints(Gomoku::Board::Side side) const
{
	if (Side::White == side)
		return WhiteCapturePoints_;
	else if (Side::Black == side)
		return BlackCapturePoints_;
	return -1;
}

const std::vector<Gomoku::Board::pcell> &Gomoku::Board::GetAvailableMoves() const
{
	return availableMoves_;
}

std::istream &Gomoku::operator>>(std::istream &is, Gomoku::Board &bs)

{
	bs = Board();

	int a, b;
	is
		>> bs.WhiteCapturePoints_
		>> bs.BlackCapturePoints_
		>> bs.movePattern_;

	for (int i = Gomoku::Board::cells_in_line - 1; i > -1; i--)
	{
		for (int j = 0; j < Gomoku::Board::cells_in_line; j++) {
			char kek;
			is >> kek;

			switch (kek) {
				case '_':
					bs.SetStoneInternal(i, j, Gomoku::Board::Side::None);
					break;
				case 'X':
					bs.SetStoneInternal(i, j, Gomoku::Board::Side::White);
					break;
				case 'O':
					bs.SetStoneInternal(i, j, Gomoku::Board::Side::Black);
					break;
			}
		}
	}

	bs.GenerateAvailableMovesInternal();

	return is;
}

std::ostream &Gomoku::operator<<(std::ostream &os, const Gomoku::Board &bs)
{
	os
            << bs.GetCapturePoints(Gomoku::Board::Side::White)
            << " " << bs.GetCapturePoints(Gomoku::Board::Side::Black)
            << " " << bs.movePattern_ << std::endl;

	for (int i = 18; i >= 0; i--)
	{
		for (int j = 0; j < 19; j++)
		{
			switch (bs.At(i, j)) {
				case Gomoku::Board::Side::None:
					os << "_";
					break;
				case Gomoku::Board::Side::White:
					os << "X";
					break;
				case Gomoku::Board::Side::Black:
					os << "O";
					break;
			}
		}
		os << std::endl;
	}
	return os;
}

bool Gomoku::Board::WhiteMove() const
{
	return movePattern_ == 0b01;
}

const std::vector<Gomoku::Board::pcell> &Gomoku::Board::GetMovesList() const
{
	return this->moves_;
}

Gomoku::Board::pcell Gomoku::Board::StringToMove(const std::string &s)
{
    pcell ret;

	ret.second = s[0] - 'a';

	ret.first = std::stoi(s.c_str() + 1) - 1;

	return ret;
}

std::string Gomoku::Board::MoveToString(const pcell &move)
{
	std::stringstream ss;
	static const char *letters = "abcdefghijklmnopqrs";

	ss << letters[move.second] << move.first + 1;

	return ss.str();
}

int Gomoku::Board::CountFigureOverBoard(const Gomoku::Board::GomokuShape &shape) const
{
	int ret = 0;

	ret += CountFigures(board_, shape);
	ret += CountFigures(vertical_, shape);
	ret += CountFigures(upLines_, shape, true);
	ret += CountFigures(downLines_, shape, true);

	return ret;
}

bool Gomoku::Board::IsThereFigureOnBoard(const Gomoku::Board::GomokuShape &shape) const
{
	int ret = 0;

	ret += CountFigures(board_, shape);
	if (ret) return true;

	ret += CountFigures(vertical_, shape);
	if (ret) return true;

	ret += CountFigures(upLines_, shape, true);
	if (ret) return true;

	ret += CountFigures(downLines_, shape, true);
	if (ret) return true;

	return false;
}

bool Gomoku::operator==(const Gomoku::Board &left, const Gomoku::Board &right)
{
    return left.board_ == right.board_
     	&& left.WhiteCapturePoints_ == right.WhiteCapturePoints_
     	&& left.WhiteCapturePoints_ == right.BlackCapturePoints_;
}

Gomoku::Board::MoveResult Gomoku::Board::GetLastMoveResult() const
{
	return this->lastMoveResult_;
}

int Gomoku::Board::CountFreeThrees(Gomoku::Board::Side side) const
{
	int freeThreesCount = 0;

	if (side == Side::White)
	{
		freeThreesCount += CountFigures(board_, figure_free_three2_w);
		freeThreesCount += CountFigures(vertical_, figure_free_three2_w);
		freeThreesCount += CountFigures(upLines_, figure_free_three2_w, true);
		freeThreesCount += CountFigures(downLines_, figure_free_three2_w, true);

		freeThreesCount += CountFigures(board_, figure_free_three3_w);
		freeThreesCount += CountFigures(vertical_, figure_free_three3_w);
		freeThreesCount += CountFigures(upLines_, figure_free_three3_w, true);
		freeThreesCount += CountFigures(downLines_, figure_free_three3_w, true);

		freeThreesCount -= CountFigures(board_, figure_free_three1_w);
		freeThreesCount -= CountFigures(vertical_, figure_free_three1_w);
		freeThreesCount -= CountFigures(upLines_, figure_free_three1_w, true);
		freeThreesCount -= CountFigures(downLines_, figure_free_three1_w, true);

		freeThreesCount += CountFigures(board_, figure_free_three4_w);
		freeThreesCount += CountFigures(vertical_, figure_free_three4_w);
		freeThreesCount += CountFigures(upLines_, figure_free_three4_w, true);
		freeThreesCount += CountFigures(downLines_, figure_free_three4_w, true);

		freeThreesCount += CountFigures(board_, figure_free_three5_w);
		freeThreesCount += CountFigures(vertical_, figure_free_three5_w);
		freeThreesCount += CountFigures(upLines_, figure_free_three5_w, true);
		freeThreesCount += CountFigures(downLines_, figure_free_three5_w, true);
	}
	else
	{
		freeThreesCount += CountFigures(board_, figure_free_three2_b);
		freeThreesCount += CountFigures(vertical_, figure_free_three2_b);
		freeThreesCount += CountFigures(upLines_, figure_free_three2_b, true);
		freeThreesCount += CountFigures(downLines_, figure_free_three2_b, true);

		freeThreesCount += CountFigures(board_, figure_free_three3_b);
		freeThreesCount += CountFigures(vertical_, figure_free_three3_b);
		freeThreesCount += CountFigures(upLines_, figure_free_three3_b, true);
		freeThreesCount += CountFigures(downLines_, figure_free_three3_b, true);

		freeThreesCount -= CountFigures(board_, figure_free_three1_b);
		freeThreesCount -= CountFigures(vertical_, figure_free_three1_b);
		freeThreesCount -= CountFigures(upLines_, figure_free_three1_b, true);
		freeThreesCount -= CountFigures(downLines_, figure_free_three1_b, true);

		freeThreesCount += CountFigures(board_, figure_free_three4_b);
		freeThreesCount += CountFigures(vertical_, figure_free_three4_b);
		freeThreesCount += CountFigures(upLines_, figure_free_three4_b, true);
		freeThreesCount += CountFigures(downLines_, figure_free_three4_b, true);

		freeThreesCount += CountFigures(board_, figure_free_three5_b);
		freeThreesCount += CountFigures(vertical_, figure_free_three5_b);
		freeThreesCount += CountFigures(upLines_, figure_free_three5_b, true);
		freeThreesCount += CountFigures(downLines_, figure_free_three5_b, true);

	}

	return freeThreesCount;
}

int Gomoku::Board::CountHalfFreeFours(Gomoku::Board::Side side) const
{
	// TODO прижатые к стенке четверки надо сделать.
	int halfFreeFoursCount = 0;

	if (side == Side::White)
	{
		halfFreeFoursCount += CountFigures(board_, figure_half_four1_w);
		halfFreeFoursCount += CountFigures(vertical_, figure_half_four1_w);
		halfFreeFoursCount += CountFigures(upLines_, figure_half_four1_w, true);
		halfFreeFoursCount += CountFigures(downLines_, figure_half_four1_w, true);

		halfFreeFoursCount += CountFigures(board_, figure_half_four2_w);
		halfFreeFoursCount += CountFigures(vertical_, figure_half_four2_w);
		halfFreeFoursCount += CountFigures(upLines_, figure_half_four2_w, true);
		halfFreeFoursCount += CountFigures(downLines_, figure_half_four2_w, true);

		halfFreeFoursCount += CountFigures(board_, figure_half_four3_w);
		halfFreeFoursCount += CountFigures(vertical_, figure_half_four3_w);
		halfFreeFoursCount += CountFigures(upLines_, figure_half_four3_w, true);
		halfFreeFoursCount += CountFigures(downLines_, figure_half_four3_w, true);

		halfFreeFoursCount += CountFigures(board_, figure_half_four4_w);
		halfFreeFoursCount += CountFigures(vertical_, figure_half_four4_w);
		halfFreeFoursCount += CountFigures(upLines_, figure_half_four4_w, true);
		halfFreeFoursCount += CountFigures(downLines_, figure_half_four4_w, true);

		halfFreeFoursCount += CountFigures(board_, figure_half_four5_w);
		halfFreeFoursCount += CountFigures(vertical_, figure_half_four5_w);
		halfFreeFoursCount += CountFigures(upLines_, figure_half_four5_w, true);
		halfFreeFoursCount += CountFigures(downLines_, figure_half_four5_w, true);

		halfFreeFoursCount += CountFiguresBeginRow(board_, figure_half_four6_special_w);
		halfFreeFoursCount += CountFiguresBeginRow(vertical_, figure_half_four6_special_w);
		halfFreeFoursCount += CountFiguresBeginRow(upLines_, figure_half_four6_special_w, true);
		halfFreeFoursCount += CountFiguresBeginRow(downLines_, figure_half_four6_special_w, true);

		halfFreeFoursCount += CountFiguresEndRow(board_, figure_half_four7_special_w);
		halfFreeFoursCount += CountFiguresEndRow(vertical_, figure_half_four7_special_w);
		halfFreeFoursCount += CountFiguresEndRow(upLines_, figure_half_four7_special_w, true);
		halfFreeFoursCount += CountFiguresEndRow(downLines_, figure_half_four7_special_w, true);

	}
	else
	{
		halfFreeFoursCount += CountFigures(board_, figure_half_four1_b);
		halfFreeFoursCount += CountFigures(vertical_, figure_half_four1_b);
		halfFreeFoursCount += CountFigures(upLines_, figure_half_four1_b, true);
		halfFreeFoursCount += CountFigures(downLines_, figure_half_four1_b, true);

		halfFreeFoursCount += CountFigures(board_, figure_half_four2_b);
		halfFreeFoursCount += CountFigures(vertical_, figure_half_four2_b);
		halfFreeFoursCount += CountFigures(upLines_, figure_half_four2_b, true);
		halfFreeFoursCount += CountFigures(downLines_, figure_half_four2_b, true);

		halfFreeFoursCount += CountFigures(board_, figure_half_four3_b);
		halfFreeFoursCount += CountFigures(vertical_, figure_half_four3_b);
		halfFreeFoursCount += CountFigures(upLines_, figure_half_four3_b, true);
		halfFreeFoursCount += CountFigures(downLines_, figure_half_four3_b, true);

		halfFreeFoursCount += CountFigures(board_, figure_half_four4_b);
		halfFreeFoursCount += CountFigures(vertical_, figure_half_four4_b);
		halfFreeFoursCount += CountFigures(upLines_, figure_half_four4_b, true);
		halfFreeFoursCount += CountFigures(downLines_, figure_half_four4_b, true);

		halfFreeFoursCount += CountFigures(board_, figure_half_four5_b);
		halfFreeFoursCount += CountFigures(vertical_, figure_half_four5_b);
		halfFreeFoursCount += CountFigures(upLines_, figure_half_four5_b, true);
		halfFreeFoursCount += CountFigures(downLines_, figure_half_four5_b, true);

		halfFreeFoursCount += CountFiguresBeginRow(board_, figure_half_four6_special_b);
		halfFreeFoursCount += CountFiguresBeginRow(vertical_, figure_half_four6_special_b);
		halfFreeFoursCount += CountFiguresBeginRow(upLines_, figure_half_four6_special_b, true);
		halfFreeFoursCount += CountFiguresBeginRow(downLines_, figure_half_four6_special_b, true);

		halfFreeFoursCount += CountFiguresEndRow(board_, figure_half_four7_special_b);
		halfFreeFoursCount += CountFiguresEndRow(vertical_, figure_half_four7_special_b);
		halfFreeFoursCount += CountFiguresEndRow(upLines_, figure_half_four7_special_b, true);
		halfFreeFoursCount += CountFiguresEndRow(downLines_, figure_half_four7_special_b, true);
	}

	return halfFreeFoursCount;
}

int Gomoku::Board::CountHalfFreeThrees(Gomoku::Board::Side side) const
{
	// TODO прижатые к стенке четверки надо сделать.
	int halfFreeThreesCount = 0;

	if (side == Side::White)
	{
		halfFreeThreesCount += CountFigures(board_, figure_half_three1_w);
		halfFreeThreesCount += CountFigures(vertical_, figure_half_three1_w);
		halfFreeThreesCount += CountFigures(upLines_, figure_half_three1_w, true);
		halfFreeThreesCount += CountFigures(downLines_, figure_half_three1_w, true);

		halfFreeThreesCount += CountFigures(board_, figure_half_three2_w);
		halfFreeThreesCount += CountFigures(vertical_, figure_half_three2_w);
		halfFreeThreesCount += CountFigures(upLines_, figure_half_three2_w, true);
		halfFreeThreesCount += CountFigures(downLines_, figure_half_three2_w, true);

		halfFreeThreesCount += CountFigures(board_, figure_half_three3_w);
		halfFreeThreesCount += CountFigures(vertical_, figure_half_three3_w);
		halfFreeThreesCount += CountFigures(upLines_, figure_half_three3_w, true);
		halfFreeThreesCount += CountFigures(downLines_, figure_half_three3_w, true);

		halfFreeThreesCount += CountFigures(board_, figure_half_three4_w);
		halfFreeThreesCount += CountFigures(vertical_, figure_half_three4_w);
		halfFreeThreesCount += CountFigures(upLines_, figure_half_three4_w, true);
		halfFreeThreesCount += CountFigures(downLines_, figure_half_three4_w, true);

		halfFreeThreesCount += CountFigures(board_, figure_half_three5_w);
		halfFreeThreesCount += CountFigures(vertical_, figure_half_three5_w);
		halfFreeThreesCount += CountFigures(upLines_, figure_half_three5_w, true);
		halfFreeThreesCount += CountFigures(downLines_, figure_half_three5_w, true);

		halfFreeThreesCount += CountFigures(board_, figure_half_three6_w);
		halfFreeThreesCount += CountFigures(vertical_, figure_half_three6_w);
		halfFreeThreesCount += CountFigures(upLines_, figure_half_three6_w, true);
		halfFreeThreesCount += CountFigures(downLines_, figure_half_three6_w, true);
	}
	else
	{
		halfFreeThreesCount += CountFigures(board_, figure_half_three1_b);
		halfFreeThreesCount += CountFigures(vertical_, figure_half_three1_b);
		halfFreeThreesCount += CountFigures(upLines_, figure_half_three1_b, true);
		halfFreeThreesCount += CountFigures(downLines_, figure_half_three1_b, true);

		halfFreeThreesCount += CountFigures(board_, figure_half_three2_b);
		halfFreeThreesCount += CountFigures(vertical_, figure_half_three2_b);
		halfFreeThreesCount += CountFigures(upLines_, figure_half_three2_b, true);
		halfFreeThreesCount += CountFigures(downLines_, figure_half_three2_b, true);

		halfFreeThreesCount += CountFigures(board_, figure_half_three3_b);
		halfFreeThreesCount += CountFigures(vertical_, figure_half_three3_b);
		halfFreeThreesCount += CountFigures(upLines_, figure_half_three3_b, true);
		halfFreeThreesCount += CountFigures(downLines_, figure_half_three3_b, true);

		halfFreeThreesCount += CountFigures(board_, figure_half_three4_b);
		halfFreeThreesCount += CountFigures(vertical_, figure_half_three4_b);
		halfFreeThreesCount += CountFigures(upLines_, figure_half_three4_b, true);
		halfFreeThreesCount += CountFigures(downLines_, figure_half_three4_b, true);

		halfFreeThreesCount += CountFigures(board_, figure_half_three5_b);
		halfFreeThreesCount += CountFigures(vertical_, figure_half_three5_b);
		halfFreeThreesCount += CountFigures(upLines_, figure_half_three5_b, true);
		halfFreeThreesCount += CountFigures(downLines_, figure_half_three5_b, true);

		halfFreeThreesCount += CountFigures(board_, figure_half_three6_b);
		halfFreeThreesCount += CountFigures(vertical_, figure_half_three6_b);
		halfFreeThreesCount += CountFigures(upLines_, figure_half_three6_b, true);
		halfFreeThreesCount += CountFigures(downLines_, figure_half_three6_b, true);
	}

	return halfFreeThreesCount;
}

void Gomoku::Board::GenerateAvailableMovesInternal()
{
	availableMoves_.clear();

	// Form available moves for next turn
	for (int i = 0; i < cells_in_line; i++)
		for (int j = 0; j < cells_in_line; j++)
		{
			if (Side::None != At(i, j))
                continue;

			// Check if move valid
            if (IsMoveCapture({i, j}, movePattern_))
            {
                // Captures always valid
                availableMoves_.emplace_back(i, j);
                continue;
            }

            int newFreeThreesCount = 0;

            // Pretend to make move
            SetStoneInternal(i, j, Side(movePattern_.to_ulong()));

            if (WhiteMove())
                newFreeThreesCount = CountFreeThreesLastMove(Side::White, {i, j});
            else
                newFreeThreesCount = CountFreeThreesLastMove(Side::Black, {i, j});

            // Two or more free threes NOT produced
            // if (!(newFreeThreesCount > freeThreesCount + 1))
            if (newFreeThreesCount < 2)
                availableMoves_.emplace_back(i, j);

            // Return back pretended move
            SetStoneInternal(i, j, Side::None);
		}
}

template<typename B>
int Gomoku::Board::CountFiguresPoints(const B &lines, const Gomoku::Board::GomokuShape &shape, int row, int col) const {
	int ret = 0;

	const auto& row_ = lines[row];

	for (int i = std::max(0, col - shape.size + 1); i <= col; i++) //std::min(col + shape.size, cells_in_line - shape.size)
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

template<typename B>
int Gomoku::Board::CountFiguresEndRow(const B &lines, const Gomoku::Board::GomokuShape &shape, bool diagonal) const
{
	int ret = 0;

	if (!diagonal)
	{
		for (const auto& row_ : lines)
		{
			auto copy = row_ >> ((cells_in_line - shape.size) * bits_per_cell);

			if (copy == shape.data)
				// shape in a row found!
				ret++;
		}
	}
	else
	{
		int len = shape.size;
		int i = len-1;

		for (; len < 19; len++, i++)
		{
			auto copy = lines[i] >> ((len - shape.size) * bits_per_cell);

			if (copy == shape.data)
				ret++;
		}
		for (; len > shape.size - 1; len--, i++)
		{
			auto copy = lines[i] >> ((len - shape.size) * bits_per_cell);

			if (copy == shape.data)
				ret++;
		}
	}

	return ret;
}

template<typename B>
int Gomoku::Board::CountFiguresBeginRow(const B &lines, const Gomoku::Board::GomokuShape &shape, bool diagonal) const
{
	int ret = 0;

	if (!diagonal)
	{
		for (const auto& row_ : lines)
		{
			auto copy = (row_
					<< ((cells_in_line - shape.size) * bits_per_cell)
					>> ((cells_in_line - shape.size) * bits_per_cell));

			if (copy == shape.data)
				// shape in a row found!
				ret++;
		}
	}
	else
	{
		int len = shape.size;
		int i = len-1;

		for (; len < 19; len++, i++)
		{
			auto copy = (lines[i]
					<< ((cells_in_line - shape.size) * bits_per_cell)
					>> ((cells_in_line - shape.size) * bits_per_cell));

			if (copy == shape.data)
			{
				ret++;
			}

		}
		for (; len > shape.size - 1; len--, i++)
		{
			auto copy = (lines[i]
					<< ((cells_in_line - shape.size) * bits_per_cell)
					>> ((cells_in_line - shape.size) * bits_per_cell));

			if (copy == shape.data)
			{
				ret++;
			}

		}
	}

	return ret;
}

template<typename B>
int Gomoku::Board::CountFigures(const B &lines, const Gomoku::Board::GomokuShape &shape, bool diagonal) const
{
	int ret = 0;

	if (!diagonal)
	{
		for (const auto& row_ : lines)
		{
			for (int i = 0; i <= cells_in_line - shape.size; i++)
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
	}
	else
	{
		int len = shape.size;
		int i = len-1;

		for (; len < 19; len++, i++)
		{
			for (int j = 0; j <= len - shape.size; j++)
			{
				auto copy = (lines[i]
						<< ((cells_in_line - j - shape.size) * bits_per_cell)
						>> ((cells_in_line - j - shape.size) * bits_per_cell)
						>> (j * bits_per_cell));


				if (copy == shape.data)
					ret++;
			}
		}
		for (; len > shape.size - 1; len--, i++)
		{
			for (int j = 0; j <= len - shape.size; j++)
			{
				auto copy = (lines[i]
						<< ((cells_in_line - j - shape.size) * bits_per_cell)
						>> ((cells_in_line - j - shape.size) * bits_per_cell)
						>> (j * bits_per_cell));


				if (copy == shape.data)
					ret++;
			}
		}
	}

	return ret;
}

Gomoku::Board::Side Gomoku::Board::At(const std::string &move) const
{
	return At(Board::StringToMove(move).first, Board::StringToMove(move).second);
}

int Gomoku::Board::GetStoneCount() const
{
	int ret = 0;

	for (const auto &row: board_)
		ret += row.count();

	return ret;
}


bool Gomoku::Board::IsCellHasStoneNearby(pcell cell, int eps) const
{
    static constexpr board_line ones = 0b11111111111111111111111111111111111111;

    const int row = cell.first, col = cell.second;
    const auto mask = ones
            << (bits_per_line - 2*(col + 1 + eps)) >> (bits_per_line - 2*(col + 1 + eps))
            >> (2*(col - eps)) << (2*(col - eps));


    bool ret = false;

    for (
        int i = std::max(0, row - eps);
        i < std::min(row + eps + 1, cells_in_line);
        i++
        )
    {
        ret = (mask & board_[i]).any();

        if (ret) return ret;
    }

    return ret;
}

#pragma clang diagnostic pop