//
// Created by Lommy Reznak on 2/3/21.
//

#include "AI1.h"
#include "../Engine.h"
#include <map>
#include <future>
#include <queue>

void Gomoku::AI1::YourTurn(int row, int col, const std::vector<std::pair<int, int>> &availableMoves)
{
	auto t1 = std::chrono::high_resolution_clock::now();

	using moves_pq = std::priority_queue<
			std::pair<Board, int>,
			std::vector<std::pair<Board, int>>,
			std::function<bool(const std::pair<Board, int> &left, const std::pair<Board, int> &right)>>;


	myMove = true;

	if (this->currentBoard.GetStoneCount() == 0
		&& (std::find(availableMoves.begin(), availableMoves.end(),
		std::make_pair(Board::StringToMove("j10").first, Board::StringToMove("j10").second))
				!= availableMoves.end()))
	{
		this->nextMove = std::make_pair(Board::StringToMove("j10").first, Board::StringToMove("j10").second);
		return;
	}

	if (!FindNext())
	{
		tree = std::make_shared<CalcNode>(currentBoard, std::weak_ptr<CalcNode>());
	}


	auto findPerspectiveMoves = [this](std::vector<Board::pcell>::const_iterator left,
									   std::vector<Board::pcell>::const_iterator right,
									   const std::shared_ptr<CalcNode>& node,
									   bool max = true)
	{
//		auto t1 = std::chrono::high_resolution_clock::now();

		std::vector<std::pair<Board, int>> pm;

		moves_pq perspectiveMoves([this, &node]
		        (const std::pair<Board, int> &left, const std::pair<Board, int> &right)
        {

			return score1WorseThenScore2(left.second, right.second);
		});


		for (; left != right; left++)
		{
			const auto &move = *left;
			if (!node->state_.IsCellHasStoneNearby(move, 3)) continue;

			auto copy = node->state_;
			copy.MakeMove(move);
			auto val = Gomoku::Engine::StaticPositionAnalize(copy);

			perspectiveMoves.emplace(std::move(copy), val);

		}

//		auto t2 = std::chrono::high_resolution_clock::now();
//		auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count();
//		std::cout << "ONE: "  << duration << std::endl;

        return perspectiveMoves;
	};

	const int depth = 3;
	const int countOfBestCandididates = 2;

	int countOfThreads = 0;
	std::vector<std::future<moves_pq>> futures;

	int tmp = availableMoves.size();
	const int countInThread = 30;
	if (tmp < 10)
		countOfThreads = 1;
	else
		countOfThreads = tmp / countInThread + (tmp % countInThread != 0);

	futures.reserve(countOfThreads);
	for (int i = 0; i < countOfThreads; i++)
	{
		futures.emplace_back(std::move(std::async(
				findPerspectiveMoves, availableMoves.begin() + i * countInThread,
				std::min(availableMoves.begin() + (i+1) * countInThread, availableMoves.end()),
				tree
		)));
	}



    moves_pq pq([this](const std::pair<Board, int> &left, const std::pair<Board, int> &right) {
			return score1WorseThenScore2(left.second, right.second);
	});

    for (int i = 0; i < countOfThreads; i++)
	{
		auto tmp2 = futures[i].get();

		for (int j = 0; j < countOfBestCandididates && !tmp2.empty(); j++)
		{
			pq.emplace(tmp2.top());

			tmp2.pop();
		}
	}


	if (!pq.empty() && !pq.top().first.GetMovesList().empty())
    {
        this->nextMove = pq.top().first.GetMovesList().back();
        std::cout << "MOVE:" << Board::MoveToString(pq.top().first.GetMovesList().back()) << std::endl;
    }
	else
    {
	    std::cout << "NOT FOUND" << std::endl;
    }


	for (int i = 0; i < countOfBestCandididates && !pq.empty(); i++)
	{
		tree->children_.emplace_back(std::make_shared<CalcNode>(
				std::move(const_cast<Gomoku::Board&>(pq.top().first)),
				std::weak_ptr<CalcNode>()));

		pq.pop();
	}

	std::queue<std::pair<int /* depth */, std::shared_ptr<CalcNode>>> t{};
	t.emplace(0, tree);

	while (!t.empty() && !t.front().second->children_.empty())
	{
		for (const auto &child : t.front().second->children_)
			t.emplace(t.front().first + 1, child);

		t.pop();
	}

	while (t.front().first < depth + currentBoard.GetMovesList().size())
	{
		std::cout << "get: " << t.front().first << ", :" << t.size() << std::endl;

		std::vector<std::future<moves_pq>> futures2;

		const auto &avlMoves = t.front().second->state_.GetAvailableMoves();

		int tmp2 = avlMoves.size();

		if (tmp < 10)
			countOfThreads = 1;
		else
			countOfThreads = tmp2 / countInThread + (tmp2 % countInThread != 0);

		futures2.reserve(countOfThreads);
		for (int i = 0; i < countOfThreads; i++)
		{
			futures2.emplace_back(std::move(std::async(
					findPerspectiveMoves, avlMoves.begin() + i * countInThread,
					std::min(avlMoves.begin() + (i+1) * countInThread, avlMoves.end()),
					t.front().second
			)));
		}

		moves_pq pq2([this](const std::pair<Board, int> &left, const std::pair<Board, int> &right) {
			return score1WorseThenScore2(left.second, right.second);
		});

		for (int i = 0; i < countOfThreads; i++)
		{
			auto tmp22 = futures2[i].get();

			for (int j = 0; j < countOfBestCandididates && !tmp22.empty(); j++)
			{
				pq2.emplace(tmp22.top());
				tmp22.pop();
			}
		}

		for (int i = 0; i < countOfBestCandididates && !pq2.empty(); i++)
		{
			t.front().second->children_.emplace_back(std::make_shared<CalcNode>(
					std::move(const_cast<Gomoku::Board&>(pq2.top().first)),
					t.front().second));
			pq2.pop();

			t.emplace(t.front().first + 1, t.front().second->children_.back());
		}
		t.pop();

	}

	auto t2 = std::chrono::high_resolution_clock::now();
	auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count();
    std::cout << "BEST MOVE: " << Gomoku::Board::MoveToString(this->nextMove) << ", ms elapsed: " << duration << std::endl;
}

Gomoku::Board::MoveResult Gomoku::AI1::Ping()
{
	if (!myMove) return {};

	{
		myMove = false;
		return MakeMove_(nextMove.first, nextMove.second);
	}

	return {};
}

bool Gomoku::AI1::FindNext()
{
	for (auto it = tree->children_.begin(); it != tree->children_.end(); it++)
	{
		if ((*it)->state_ == currentBoard)
		{
//			tree = std::move(tree->children.extract(it).value());
			return true;
		}
	}

	return false;
}
