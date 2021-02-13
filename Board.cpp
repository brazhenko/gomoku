#pragma clang diagnostic push
#pragma ide diagnostic ignored "hicpp-signed-bitwise"

//
// Created by 17641238 on 19.01.2021.
//

#include "Board.h"
#include <vector>
#include "PGNGame.h"

// Mappings of coodinates: (Normal x, y) -> (Vericle, Diagonal1, Diagonal2 lines x, y respectively)
const std::unordered_map<std::pair<int, int>, std::pair<int, int>, pairhash> Gomoku::Board::_cToVerticles = [](){
	std::unordered_map<std::pair<int, int>, std::pair<int, int>, pairhash> ret;
	for (int j = 0; j < cells_in_line; j++)
		for (int i = 0; i < cells_in_line; i++)
			ret.insert({{i, j}, {j, i}});

	return ret;
}();

const std::unordered_map<std::pair<int, int>, std::pair<int, int>, pairhash> Gomoku::Board::_cToUpLines = [](){
	std::unordered_map<std::pair<int, int>, std::pair<int, int>, pairhash> ret;
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

const std::unordered_map<std::pair<int, int>, std::pair<int, int>, pairhash> Gomoku::Board::_cToDownLines = [](){
	std::unordered_map<std::pair<int, int>, std::pair<int, int>, pairhash> ret;
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
			this->available_moves.emplace(i, j);

	// Или этот
// 	GenerateAvailableMovesInternal();
}


Gomoku::Board::Board(const std::vector<std::pair<int, int>> &moves)
	// Construct default board
	: Board()
{
	auto t1 = std::chrono::high_resolution_clock::now();

	for (const auto &move : moves)
	{
		if (GetAvailableMoves().find(move) == GetAvailableMoves().end()) // if move is not available
		{
			std::stringstream ss;

			ss << "Trying to construct board with wrong move: " << Board::MoveToString(move);
			throw std::runtime_error(ss.str());
		}

		this->MakeMove(move.first, move.second);
	}

	auto t2 = std::chrono::high_resolution_clock::now();
	auto duration = std::chrono::duration_cast<std::chrono::milliseconds>( t2 - t1 ).count();

	std::cout << duration << std::endl;
}

bool Gomoku::Board::IsMoveCapture(int row, int col, Gomoku::Board::board_line mvPtrn) const
{
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

// TODO Gomoku::Board::Side side не нужен тут походу
int Gomoku::Board::CountFreeThreesLastMove(Gomoku::Board::Side side, std::pair<int, int> lastMove) const
{
	int freeThreesCount = 0;

	const auto& row = lastMove.first;
	const auto& col = lastMove.second;

	const auto &upC = _cToUpLines.at({row, col});
	const auto &downC = _cToDownLines.at({row, col});

	if (Board::Side::White == side)
	{
		freeThreesCount += CountFiguresPoints(board_, figure_free_three2_w, row, col);
		freeThreesCount += CountFiguresPoints(vertical_, figure_free_three2_w, col, row);
		freeThreesCount += CountFiguresPoints(up_lines_, figure_free_three2_w, upC.first, upC.second);
		freeThreesCount += CountFiguresPoints(down_lines_, figure_free_three2_w, downC.first, downC.second);

		freeThreesCount += CountFiguresPoints(board_, figure_free_three3_w, row, col);
		freeThreesCount += CountFiguresPoints(vertical_, figure_free_three3_w, col, row);
		freeThreesCount += CountFiguresPoints(up_lines_, figure_free_three3_w, upC.first, upC.second);
		freeThreesCount += CountFiguresPoints(down_lines_, figure_free_three3_w, downC.first, downC.second);

		freeThreesCount -= CountFiguresPoints(board_, figure_free_three1_w, row, col);
		freeThreesCount -= CountFiguresPoints(vertical_, figure_free_three1_w, col, row);
		freeThreesCount -= CountFiguresPoints(up_lines_, figure_free_three1_w, upC.first, upC.second);
		freeThreesCount -= CountFiguresPoints(down_lines_, figure_free_three1_w, downC.first, downC.second);

		freeThreesCount += CountFiguresPoints(board_, figure_free_three4_w, row, col);
		freeThreesCount += CountFiguresPoints(vertical_, figure_free_three4_w, col, row);
		freeThreesCount += CountFiguresPoints(up_lines_, figure_free_three4_w, upC.first, upC.second);
		freeThreesCount += CountFiguresPoints(down_lines_, figure_free_three4_w, downC.first, downC.second);

		freeThreesCount += CountFiguresPoints(board_, figure_free_three5_w, row, col);
		freeThreesCount += CountFiguresPoints(vertical_, figure_free_three5_w, col, row);
		freeThreesCount += CountFiguresPoints(up_lines_, figure_free_three5_w, upC.first, upC.second);
		freeThreesCount += CountFiguresPoints(down_lines_, figure_free_three5_w, downC.first, downC.second);
	}
	else if (Board::Side::Black == side)
	{
		freeThreesCount += CountFiguresPoints(board_, figure_free_three2_b, row, col);
		freeThreesCount += CountFiguresPoints(vertical_, figure_free_three2_b, col, row);
		freeThreesCount += CountFiguresPoints(up_lines_, figure_free_three2_b, upC.first, upC.second);
		freeThreesCount += CountFiguresPoints(down_lines_, figure_free_three2_b, downC.first, downC.second);

		freeThreesCount += CountFiguresPoints(board_, figure_free_three3_b, row, col);
		freeThreesCount += CountFiguresPoints(vertical_, figure_free_three3_b, col, row);
		freeThreesCount += CountFiguresPoints(up_lines_, figure_free_three3_b, upC.first, upC.second);
		freeThreesCount += CountFiguresPoints(down_lines_, figure_free_three3_b, downC.first, downC.second);

		freeThreesCount -= CountFiguresPoints(board_, figure_free_three1_b, row, col);
		freeThreesCount -= CountFiguresPoints(vertical_, figure_free_three1_b, col, row);
		freeThreesCount -= CountFiguresPoints(up_lines_, figure_free_three1_b, upC.first, upC.second);
		freeThreesCount -= CountFiguresPoints(down_lines_, figure_free_three1_b, downC.first, downC.second);

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

void Gomoku::Board::FindMovesBreaksFifthInternal()
{
	for (int i = 0; i < cells_in_line; i++)
	{
		for (int j = 0; j < cells_in_line; j++)
		{
			if (Side::None == At(i, j))
			{
				// Check if move valid
				if (IsMoveCapture(i, j, movePattern))
				{
					auto captured = MakeCapture(i, j);
					int fifthCount = 0;

					if (!captured.empty())
					{
						if (!WhiteMove())
						{
							fifthCount += CountFigures(board_, figure_five_w);
							fifthCount += CountFigures(vertical_, figure_five_w);
							fifthCount += CountFigures(up_lines_, figure_five_w, true);
							fifthCount += CountFigures(down_lines_, figure_five_w, true);
							WhiteCapturePoints -= (captured.size() / 2);
						}
						else
						{
							fifthCount += CountFigures(board_, figure_five_b);
							fifthCount += CountFigures(vertical_, figure_five_b);
							fifthCount += CountFigures(up_lines_, figure_five_b, true);
							fifthCount += CountFigures(down_lines_, figure_five_b, true);

							BlackCapturePoints -= (captured.size() / 2);
						}

						// Capture destroys fifth
						if (fifthCount == 0)
							available_moves.emplace(i, j);

						for (const auto &c : captured)
							SetStoneInternal(c.first, c.second, Side(movePattern.to_ulong() ^ 0b11U));
					}

				}
			}
		}
	}
}

std::vector<std::pair<int, int>> Gomoku::Board::MakeCapture(int row, int col)
{
	std::vector<std::pair<int, int>> capturedStones;

	// capture pair up
	if (row + 3 < 19
		&& int(this->At(row + 1, col)) == (movePattern.to_ulong() ^ 0b11U)
		&& int(this->At(row + 2, col)) == (movePattern.to_ulong() ^ 0b11U)
		&& int(this->At(row + 3, col)) == (movePattern.to_ulong())
			) {

		SetStoneInternal(row + 1, col, Side::None);
		capturedStones.emplace_back(row + 1, col);
		SetStoneInternal(row + 2, col, Side::None);
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

		SetStoneInternal(row + 1, col + 1, Side::None);
		capturedStones.emplace_back(row + 1, col + 1);
		SetStoneInternal(row + 2, col + 2, Side::None);
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

		SetStoneInternal(row, col + 1, Side::None);
		capturedStones.emplace_back(row, col + 1);
		SetStoneInternal(row, col + 2, Side::None);
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

		SetStoneInternal(row - 1, col + 1, Side::None);
		capturedStones.emplace_back(row - 1, col + 1);
		SetStoneInternal(row - 2, col + 2, Side::None);
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

		SetStoneInternal(row - 1, col, Side::None);
		capturedStones.emplace_back(row - 1, col);
		SetStoneInternal(row - 2, col, Side::None);
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

		SetStoneInternal(row - 1, col - 1, Side::None);
		capturedStones.emplace_back(row - 1, col - 1);
		SetStoneInternal(row - 2, col - 2, Side::None);
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

		SetStoneInternal(row, col - 1, Side::None);
		capturedStones.emplace_back(row, col - 1);
		SetStoneInternal(row, col - 2, Side::None);
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

		SetStoneInternal(row + 1, col - 1, Side::None);
		capturedStones.emplace_back(row + 1, col - 1);
		SetStoneInternal(row + 2, col - 2, Side::None);
		capturedStones.emplace_back(row + 2, col - 2);

		if (WhiteMove())
			WhiteCapturePoints++;
		else
			BlackCapturePoints++;
	}

	return capturedStones;
}

Gomoku::Board::MoveResult Gomoku::Board:: MakeMove(int row, int col)
{
	auto ret = MakeMoveInternal(row, col);
	this->lastMoveResult_ = ret;
	return ret;
}

Gomoku::Board::MoveResult Gomoku::Board::MakeMoveInternal(int row, int col)
{
	if (available_moves.find({row, col}) == available_moves.end())
		return Board::MoveResult::WrongMove;

	MoveResult ret = MoveResult::Default;
	// Add to move history
	moves_.emplace_back(row, col);
	// Put stone on board
	SetStoneInternal(row, col, Side(movePattern.to_ulong()));
	// Delete cell from available ones

	// Make captures if exist
	if (!MakeCapture(row, col).empty())
		ret = MoveResult::Capture;

	available_moves = {};

	if (WhiteMove() && WhiteCapturePoints >= 5)
		return MoveResult::WhiteWin;
	if (!WhiteMove() && BlackCapturePoints >= 5)
		return MoveResult::BlackWin;
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
		FindMovesBreaksFifthInternal();
		std::cout << "move: " << Gomoku::Board::MoveToString({row, col}) << std::endl;
		for (const auto &move: available_moves)
			std::cout << Gomoku::Board::MoveToString(move) << ";  ";


		std::cout << std::endl;
		if (available_moves.empty())
		{
			if (WhiteMove())
				return MoveResult::BlackWin;
			return MoveResult::WhiteWin;
		}
		return ret;
	}

	GenerateAvailableMovesInternal();

	if (available_moves.empty())
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
		ml.push_back(
				a
				);

		// TODO delete
		std::cout << Gomoku::Board::MoveToString(moves[i]) << ",  ";
		std::cout << ((i + 1 < moves.size()) ? Gomoku::Board::MoveToString(moves[i + 1]) : "" ) << std::endl;
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
	if (moves_.empty()) return false;
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

	up_lines_[upline.first][upline.second * 2] = unsigned(s)&1U;
	up_lines_[upline.first][upline.second * 2 + 1] = (unsigned(s)>>1U)&1U;

	down_lines_[downline.first][downline.second * 2] = unsigned(s)&1U;
	down_lines_[downline.first][downline.second * 2 + 1] = (unsigned(s)>>1U)&1U;
}

Gomoku::Board::Side Gomoku::Board::At(int row, int col) const
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

int Gomoku::Board::GetCapturePoints(Gomoku::Board::Side side) const
{
	if (Side::White == side)
		return WhiteCapturePoints;
	else if (Side::Black == side)
		return BlackCapturePoints;
	return -1;
}

const std::unordered_set<std::pair<int, int>, pairhash> &Gomoku::Board::GetAvailableMoves() const
{
	return available_moves;
}

std::istream &Gomoku::operator>>(std::istream &is, Gomoku::Board &bs)

{
	bs = Board();

	int a, b;
	is
		>> bs.WhiteCapturePoints
		>> bs.BlackCapturePoints
		>> bs.movePattern;

	std::cout << bs.WhiteCapturePoints << bs.BlackCapturePoints << bs.movePattern;

	for (int i = 18; i >= 0; i--)
	{
		for (int j = 0; j < 19; j++) {
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
		<< " " << bs.movePattern << std::endl;

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
	return movePattern == 0b01;
}

const std::vector<std::pair<int, int>> &Gomoku::Board::GetMovesList() const
{
	return this->moves_;
}

std::pair<int, int> Gomoku::Board::StringToMove(const std::string &s)
{
	std::pair<int, int> ret;

	ret.second = s[0] - 'a';

	ret.first = std::stoi(s.c_str() + 1) - 1;

	return ret;
}

std::string Gomoku::Board::MoveToString(const std::pair<int, int> &move)
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
	ret += CountFigures(up_lines_, shape, true);
	ret += CountFigures(down_lines_, shape, true);

	return ret;
}

bool Gomoku::Board::IsThereFigureOnBoard(const Gomoku::Board::GomokuShape &shape) const
{
	int ret = 0;

	ret += CountFigures(board_, shape);
	if (ret) return true;

	ret += CountFigures(vertical_, shape);
	if (ret) return true;

	ret += CountFigures(up_lines_, shape, true);
	if (ret) return true;

	ret += CountFigures(down_lines_, shape, true);
	if (ret) return true;

	return false;
}

bool Gomoku::operator==(const Gomoku::Board &left, const Gomoku::Board &right)
{
    return left.board_ == right.board_;
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
		freeThreesCount += CountFigures(up_lines_, figure_free_three2_w, true);
		freeThreesCount += CountFigures(down_lines_, figure_free_three2_w, true);

		freeThreesCount += CountFigures(board_, figure_free_three3_w);
		freeThreesCount += CountFigures(vertical_, figure_free_three3_w);
		freeThreesCount += CountFigures(up_lines_, figure_free_three3_w, true);
		freeThreesCount += CountFigures(down_lines_, figure_free_three3_w, true);

		freeThreesCount -= CountFigures(board_, figure_free_three1_w);
		freeThreesCount -= CountFigures(vertical_, figure_free_three1_w);
		freeThreesCount -= CountFigures(up_lines_, figure_free_three1_w, true);
		freeThreesCount -= CountFigures(down_lines_, figure_free_three1_w, true);

		freeThreesCount += CountFigures(board_, figure_free_three4_w);
		freeThreesCount += CountFigures(vertical_, figure_free_three4_w);
		freeThreesCount += CountFigures(up_lines_, figure_free_three4_w, true);
		freeThreesCount += CountFigures(down_lines_, figure_free_three4_w, true);

		freeThreesCount += CountFigures(board_, figure_free_three5_w);
		freeThreesCount += CountFigures(vertical_, figure_free_three5_w);
		freeThreesCount += CountFigures(up_lines_, figure_free_three5_w, true);
		freeThreesCount += CountFigures(down_lines_, figure_free_three5_w, true);
	}
	else
	{
		freeThreesCount += CountFigures(board_, figure_free_three2_b);
		freeThreesCount += CountFigures(vertical_, figure_free_three2_b);
		freeThreesCount += CountFigures(up_lines_, figure_free_three2_b, true);
		freeThreesCount += CountFigures(down_lines_, figure_free_three2_b, true);

		freeThreesCount += CountFigures(board_, figure_free_three3_b);
		freeThreesCount += CountFigures(vertical_, figure_free_three3_b);
		freeThreesCount += CountFigures(up_lines_, figure_free_three3_b, true);
		freeThreesCount += CountFigures(down_lines_, figure_free_three3_b, true);

		freeThreesCount -= CountFigures(board_, figure_free_three1_b);
		freeThreesCount -= CountFigures(vertical_, figure_free_three1_b);
		freeThreesCount -= CountFigures(up_lines_, figure_free_three1_b, true);
		freeThreesCount -= CountFigures(down_lines_, figure_free_three1_b, true);

		freeThreesCount += CountFigures(board_, figure_free_three4_b);
		freeThreesCount += CountFigures(vertical_, figure_free_three4_b);
		freeThreesCount += CountFigures(up_lines_, figure_free_three4_b, true);
		freeThreesCount += CountFigures(down_lines_, figure_free_three4_b, true);

		freeThreesCount += CountFigures(board_, figure_free_three5_b);
		freeThreesCount += CountFigures(vertical_, figure_free_three5_b);
		freeThreesCount += CountFigures(up_lines_, figure_free_three5_b, true);
		freeThreesCount += CountFigures(down_lines_, figure_free_three5_b, true);

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
		halfFreeFoursCount += CountFigures(up_lines_, figure_half_four1_w, true);
		halfFreeFoursCount += CountFigures(down_lines_, figure_half_four1_w, true);

		halfFreeFoursCount += CountFigures(board_, figure_half_four2_w);
		halfFreeFoursCount += CountFigures(vertical_, figure_half_four2_w);
		halfFreeFoursCount += CountFigures(up_lines_, figure_half_four2_w, true);
		halfFreeFoursCount += CountFigures(down_lines_, figure_half_four2_w, true);

		halfFreeFoursCount += CountFigures(board_, figure_half_four3_w);
		halfFreeFoursCount += CountFigures(vertical_, figure_half_four3_w);
		halfFreeFoursCount += CountFigures(up_lines_, figure_half_four3_w, true);
		halfFreeFoursCount += CountFigures(down_lines_, figure_half_four3_w, true);

		halfFreeFoursCount += CountFigures(board_, figure_half_four4_w);
		halfFreeFoursCount += CountFigures(vertical_, figure_half_four4_w);
		halfFreeFoursCount += CountFigures(up_lines_, figure_half_four4_w, true);
		halfFreeFoursCount += CountFigures(down_lines_, figure_half_four4_w, true);

		halfFreeFoursCount += CountFigures(board_, figure_half_four5_w);
		halfFreeFoursCount += CountFigures(vertical_, figure_half_four5_w);
		halfFreeFoursCount += CountFigures(up_lines_, figure_half_four5_w, true);
		halfFreeFoursCount += CountFigures(down_lines_, figure_half_four5_w, true);

		halfFreeFoursCount += CountFiguresBeginRow(board_, figure_half_four6_special_w);
		halfFreeFoursCount += CountFiguresBeginRow(vertical_, figure_half_four6_special_w);
		halfFreeFoursCount += CountFiguresBeginRow(up_lines_, figure_half_four6_special_w, true);
		halfFreeFoursCount += CountFiguresBeginRow(down_lines_, figure_half_four6_special_w, true);

		halfFreeFoursCount += CountFiguresEndRow(board_, figure_half_four7_special_w);
		halfFreeFoursCount += CountFiguresEndRow(vertical_, figure_half_four7_special_w);
		halfFreeFoursCount += CountFiguresEndRow(up_lines_, figure_half_four7_special_w, true);
		halfFreeFoursCount += CountFiguresEndRow(down_lines_, figure_half_four7_special_w, true);

	}
	else
	{
		halfFreeFoursCount += CountFigures(board_, figure_half_four1_b);
		halfFreeFoursCount += CountFigures(vertical_, figure_half_four1_b);
		halfFreeFoursCount += CountFigures(up_lines_, figure_half_four1_b, true);
		halfFreeFoursCount += CountFigures(down_lines_, figure_half_four1_b, true);

		halfFreeFoursCount += CountFigures(board_, figure_half_four2_b);
		halfFreeFoursCount += CountFigures(vertical_, figure_half_four2_b);
		halfFreeFoursCount += CountFigures(up_lines_, figure_half_four2_b, true);
		halfFreeFoursCount += CountFigures(down_lines_, figure_half_four2_b, true);

		halfFreeFoursCount += CountFigures(board_, figure_half_four3_b);
		halfFreeFoursCount += CountFigures(vertical_, figure_half_four3_b);
		halfFreeFoursCount += CountFigures(up_lines_, figure_half_four3_b, true);
		halfFreeFoursCount += CountFigures(down_lines_, figure_half_four3_b, true);

		halfFreeFoursCount += CountFigures(board_, figure_half_four4_b);
		halfFreeFoursCount += CountFigures(vertical_, figure_half_four4_b);
		halfFreeFoursCount += CountFigures(up_lines_, figure_half_four4_b, true);
		halfFreeFoursCount += CountFigures(down_lines_, figure_half_four4_b, true);

		halfFreeFoursCount += CountFigures(board_, figure_half_four5_b);
		halfFreeFoursCount += CountFigures(vertical_, figure_half_four5_b);
		halfFreeFoursCount += CountFigures(up_lines_, figure_half_four5_b, true);
		halfFreeFoursCount += CountFigures(down_lines_, figure_half_four5_b, true);

		halfFreeFoursCount += CountFiguresBeginRow(board_, figure_half_four6_special_b);
		halfFreeFoursCount += CountFiguresBeginRow(vertical_, figure_half_four6_special_b);
		halfFreeFoursCount += CountFiguresBeginRow(up_lines_, figure_half_four6_special_b, true);
		halfFreeFoursCount += CountFiguresBeginRow(down_lines_, figure_half_four6_special_b, true);

		halfFreeFoursCount += CountFiguresEndRow(board_, figure_half_four7_special_b);
		halfFreeFoursCount += CountFiguresEndRow(vertical_, figure_half_four7_special_b);
		halfFreeFoursCount += CountFiguresEndRow(up_lines_, figure_half_four7_special_b, true);
		halfFreeFoursCount += CountFiguresEndRow(down_lines_, figure_half_four7_special_b, true);
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
		halfFreeThreesCount += CountFigures(up_lines_, figure_half_three1_w, true);
		halfFreeThreesCount += CountFigures(down_lines_, figure_half_three1_w, true);

		halfFreeThreesCount += CountFigures(board_, figure_half_three2_w);
		halfFreeThreesCount += CountFigures(vertical_, figure_half_three2_w);
		halfFreeThreesCount += CountFigures(up_lines_, figure_half_three2_w, true);
		halfFreeThreesCount += CountFigures(down_lines_, figure_half_three2_w, true);

		halfFreeThreesCount += CountFigures(board_, figure_half_three3_w);
		halfFreeThreesCount += CountFigures(vertical_, figure_half_three3_w);
		halfFreeThreesCount += CountFigures(up_lines_, figure_half_three3_w, true);
		halfFreeThreesCount += CountFigures(down_lines_, figure_half_three3_w, true);

		halfFreeThreesCount += CountFigures(board_, figure_half_three4_w);
		halfFreeThreesCount += CountFigures(vertical_, figure_half_three4_w);
		halfFreeThreesCount += CountFigures(up_lines_, figure_half_three4_w, true);
		halfFreeThreesCount += CountFigures(down_lines_, figure_half_three4_w, true);

		halfFreeThreesCount += CountFigures(board_, figure_half_three5_w);
		halfFreeThreesCount += CountFigures(vertical_, figure_half_three5_w);
		halfFreeThreesCount += CountFigures(up_lines_, figure_half_three5_w, true);
		halfFreeThreesCount += CountFigures(down_lines_, figure_half_three5_w, true);

		halfFreeThreesCount += CountFigures(board_, figure_half_three6_w);
		halfFreeThreesCount += CountFigures(vertical_, figure_half_three6_w);
		halfFreeThreesCount += CountFigures(up_lines_, figure_half_three6_w, true);
		halfFreeThreesCount += CountFigures(down_lines_, figure_half_three6_w, true);
	}
	else
	{
		halfFreeThreesCount += CountFigures(board_, figure_half_three1_b);
		halfFreeThreesCount += CountFigures(vertical_, figure_half_three1_b);
		halfFreeThreesCount += CountFigures(up_lines_, figure_half_three1_b, true);
		halfFreeThreesCount += CountFigures(down_lines_, figure_half_three1_b, true);

		halfFreeThreesCount += CountFigures(board_, figure_half_three2_b);
		halfFreeThreesCount += CountFigures(vertical_, figure_half_three2_b);
		halfFreeThreesCount += CountFigures(up_lines_, figure_half_three2_b, true);
		halfFreeThreesCount += CountFigures(down_lines_, figure_half_three2_b, true);

		halfFreeThreesCount += CountFigures(board_, figure_half_three3_b);
		halfFreeThreesCount += CountFigures(vertical_, figure_half_three3_b);
		halfFreeThreesCount += CountFigures(up_lines_, figure_half_three3_b, true);
		halfFreeThreesCount += CountFigures(down_lines_, figure_half_three3_b, true);

		halfFreeThreesCount += CountFigures(board_, figure_half_three4_b);
		halfFreeThreesCount += CountFigures(vertical_, figure_half_three4_b);
		halfFreeThreesCount += CountFigures(up_lines_, figure_half_three4_b, true);
		halfFreeThreesCount += CountFigures(down_lines_, figure_half_three4_b, true);

		halfFreeThreesCount += CountFigures(board_, figure_half_three5_b);
		halfFreeThreesCount += CountFigures(vertical_, figure_half_three5_b);
		halfFreeThreesCount += CountFigures(up_lines_, figure_half_three5_b, true);
		halfFreeThreesCount += CountFigures(down_lines_, figure_half_three5_b, true);

		halfFreeThreesCount += CountFigures(board_, figure_half_three6_b);
		halfFreeThreesCount += CountFigures(vertical_, figure_half_three6_b);
		halfFreeThreesCount += CountFigures(up_lines_, figure_half_three6_b, true);
		halfFreeThreesCount += CountFigures(down_lines_, figure_half_three6_b, true);
	}

	return halfFreeThreesCount;
}

void Gomoku::Board::GenerateAvailableMovesInternal()
{
	// Form available moves for next turn
	for (int i = 0; i < cells_in_line; i++)
	{
		for (int j = 0; j < cells_in_line; j++)
		{
			if (Side::None == At(i, j))
			{
				// Check if move valid
				if (IsMoveCapture(i, j, movePattern))
				{
					// Captures always valid
					available_moves.emplace(i, j);
					continue;
				}
				int freeThreesCount = 0, newFreeThreesCount = 0;

				// Pretend to make move
				SetStoneInternal(i, j, Side(movePattern.to_ulong()));

				if (WhiteMove())
					newFreeThreesCount = CountFreeThreesLastMove(Side::White, {i, j});
				else
					newFreeThreesCount = CountFreeThreesLastMove(Side::Black, {i, j});

				// Two or more free threes NOT produced
				// if (!(newFreeThreesCount > freeThreesCount + 1))
				if (newFreeThreesCount < 2)
					available_moves.emplace(i, j);

				// Return back pretended move
				SetStoneInternal(i, j, Side::None);
			}
		}
	}
}

template<typename B>
int Gomoku::Board::CountFiguresPoints(const B &lines, const Gomoku::Board::GomokuShape &shape, int x, int y) const {
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
				std::cerr << "!!:" << i << std::endl;
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
				std::cout << "!!:" << i << std::endl;
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
	}
	else
	{
		int len = shape.size;
		int i = len-1;

		for (; len < 19; len++, i++)
		{
			for (int j = 0; j < len - shape.size; j++)
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
			for (int j = 0; j < len - shape.size; j++)
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

#pragma clang diagnostic pop