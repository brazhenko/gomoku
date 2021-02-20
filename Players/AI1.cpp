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
	this->nextMove = availableMoves.front();

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

	const int depth = 7;
	const int countOfBestCandididates = 3;

	auto findPerspectiveMoves = [this](std::vector<Board::pcell>::const_iterator left,
									   std::vector<Board::pcell>::const_iterator right,
									   const std::shared_ptr<CalcNode>& node,
									   bool max = true)
	{
		std::vector<std::pair<Board, int>> pm;
		pm.reserve(right - left);

		for (; left != right; left++)
		{
			const auto &move = *left;
			if (!node->state_.IsCellHasStoneNearby(move, 2)) continue;

			auto copy = node->state_;
			copy.MakeMove(move);
			auto val = Gomoku::Engine::StaticPositionAnalize(copy);

			pm.emplace_back(std::move(copy), val);

		}

		std::partial_sort(pm.begin(), std::min(pm.begin() + countOfBestCandididates, pm.end()), pm.end(),
		[this, &node]
			(const std::pair<Board, int> &left, const std::pair<Board, int> &right)
			{
				return score1BetterThenScore2(left.second, right.second);
			});

        return pm;
	};



	int countOfThreads = 0;
	std::vector<std::future<std::vector<std::pair<Board, int>>>> futures;

	int tmp = availableMoves.size();
	const int countInThread = 60;

	if (tmp < 10) countOfThreads = 1;
	else countOfThreads = tmp / countInThread + (tmp % countInThread != 0);

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

		for (int j = 0; j < countOfBestCandididates && j < tmp2.size(); j++)
		{
			pq.emplace(tmp2[j]);
		}
	}

    std::cout << "PQ:" << pq.size() << std::endl;


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

	while (t.front().first < depth && !	t.empty()) // + currentBoard.GetMovesList().size())
	{
		std::cout << "get: " << t.front().first << ", :" << t.size() << std::endl;

		std::vector<std::future<std::vector<std::pair<Board, int>>>> futures2;

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

			for (int j = 0; j < countOfBestCandididates && j < tmp22.size(); j++)
			{
				pq2.emplace((tmp22[j]));
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

			tree = *it;
			return true;
		}
	}

	return false;
}

