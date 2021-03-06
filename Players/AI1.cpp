//
// Created by Lommy Reznak on 2/3/21.
//

#include "AI1.h"
#include "Engine.h"
#include <map>
#include <future>
#include <queue>
#include <iostream>
#include <sstream>

Gomoku::AI1::AI1(Gomoku::Board::Side side, Gomoku::MakeMove_t MakeMove, const Gomoku::Board &realBoard, int depth, int width, int timeToThinkMS, bool debug, bool yourTurn)
		: IPlayer(side, std::move(MakeMove), realBoard)
		, score1BetterThenScore2(GreaterIntializer(side))
		, score1WorseThenScore2(LessIntializer(side))
		, depth_(depth)
		, countOfBestCandididates_(width)
		, maxTimeToThink_(timeToThinkMS)
		, debug_(debug)
		// Initializing calculating tree
		, tree_(std::make_shared<CalcTreeNode>(realBoard, std::weak_ptr<CalcTreeNode>(), true, 0))
		// Starting calculating thread
		, work_(true)
		, workerThread_([this](){ Worker(); })
{
	if (yourTurn) AI1::YourTurn();
}

Gomoku::AI1::AI1(Gomoku::Board::Side side, Gomoku::MakeMove_t MakeMove, const Gomoku::Board &realBoard, bool yourTurn, bool test)
    : IPlayer(side, std::move(MakeMove), realBoard)
    , score1BetterThenScore2(GreaterIntializer(side))
    , score1WorseThenScore2(LessIntializer(side))
    , depth_(3)
    , countOfBestCandididates_(2)
    , toleft(true)
    , work_(true)
    , tree_([this](){
        auto t = std::make_shared<CalcTreeNode>(std::weak_ptr<CalcTreeNode>(), true, 13);

        t->children_ = {
            std::make_shared<CalcTreeNode>(Board({{1, 1}}), t, false, 12),
            std::make_shared<CalcTreeNode>(Board({{1, 2}}), t, false, 45)
        };

        t->children_[0]->children_ = {
            std::make_shared<CalcTreeNode>(Board({{1, 3}}), t->children_[0], true, 3),
            std::make_shared<CalcTreeNode>(Board({{1, 4}}), t->children_[0], true, 17)
        };

        t->children_[1]->children_ = {
                std::make_shared<CalcTreeNode>(Board({{1, 5}}), t->children_[1], true, 2),
                std::make_shared<CalcTreeNode>(Board({{1, 6}}), t->children_[1], true, 20)
        };

        return t;
    }())
    , workerThread_([this](){ Worker(); })

{

}

Gomoku::AI1::~AI1()
{
	work_ = false;

	std::lock_guard lg(jobsMtx_);

	jobsCv_.notify_one();
	workerThread_.join();
}

void Gomoku::AI1::YourTurn()
{
	myMove = true;
	startedThinkingAt_ = std::chrono::system_clock::now();

	if (debug_)
		std::cout << "Your turn!" << std::endl;

	// setting first available move as default
	if (currentBoard.GetAvailableMoves().empty())
        return;

	{
		std::lock_guard lg(nextMoveMtx_);
		nextMove_ = currentBoard.GetAvailableMoves().front();
	}


	// reload tree root in worker
    needReload_ = true;
    jobsCv_.notify_one();
}

Gomoku::Board::MoveResult Gomoku::AI1::Ping()
{
	if (!myMove) return Board::MoveResult::NotMyMove;

	if (std::chrono::system_clock::now() >= startedThinkingAt_ + maxTimeToThink_)
	{
        // Time to think ended up, returning best move calculated !
		if (debug_)
        	std::cout << "Making Move" << std::endl;

		Gomoku::Board::MoveResult ret;
        {
			std::lock_guard lg(nextMoveMtx_);
			std::lock_guard lg2 {jobsMtx_};

			if (debug_)
				for (auto &child : tree_->children_)
				{
					std::cout << Board::MoveToString(child->state_.GetMovesList().back()) << "~" << child->positionScore_ << "; ";
				}

			nextMove_ =  (*std::max_element(
					tree_->children_.begin(),
					tree_->children_.end(),
					[this](const std::shared_ptr<CalcTreeNode> &l, const std::shared_ptr<CalcTreeNode> &r){
						   return score1WorseThenScore2(l->positionScore_, r->positionScore_);
				   }))->state_.GetMovesList().back();

			if (debug_)
				std::cerr << TreeToString() << std::endl;

			ret = MakeMove_(nextMove_.first, nextMove_.second);
		}


        myMove = false;
		needReload_ = true;
        jobsCv_.notify_one();

        return ret;
	}

	return Board::MoveResult::NotReadyToMove;
}

bool Gomoku::AI1::FindNext()
{
	if (debug_)
    	std::cout << std::boolalpha << tree_->maximize_ << " ";

    for (auto it = tree_->children_.begin(); it != tree_->children_.end(); it++)
	{
    	if (debug_)
			std::cout << Board::MoveToString((*it)->state_.GetMovesList().back()) << "~" << (*it)->positionScore_ << " ";


		if ((*it)->state_ == currentBoard)
		{
			tree_ = *it;
			tree_->parent_.reset();

			if (debug_)
            	std::cout << "found child, replacing" << std::endl;

			return true;
		}
	}

    if (debug_)
    	std::cout << "Not found, generating new" << std::endl;

	return false;
}

void Gomoku::AI1::GenerateChildren(std::shared_ptr<CalcTreeNode> &node)
{
    auto findPerspectiveMoves = [this](std::vector<Board::pcell>::const_iterator left,
                                       std::vector<Board::pcell>::const_iterator right,
                                       const std::shared_ptr<CalcTreeNode>& node)
    {
        std::vector<std::pair<Board, int>> pm;
        pm.reserve(right - left);
        const int localityRadius = 2;

        for (; left != right; left++)
        {
            if (!node->state_.IsCellHasStoneNearby(*left, localityRadius))
                continue;

            auto copy = node->state_;
            copy.MakeMove(*left);
            auto val = Gomoku::Engine::StaticPositionAnalize(copy);

            pm.emplace_back(std::move(copy), val);
		}



        std::partial_sort(pm.begin(), std::min(pm.begin() + countOfBestCandididates_, pm.end()), pm.end(),
                          [this, &node] (const std::pair<Board, int> &left, const std::pair<Board, int> &right) {
                              if (node->maximize_)
                                  return left.second > right.second;
                              return left.second < right.second;
                          });


        return pm;
    };

    // vector of job split in to pieces
    std::vector<std::future<std::vector<std::pair<Board, int>>>> futures;

    const auto &avlMoves = node->state_.GetAvailableMoves();

    int tmp = avlMoves.size();

    int countOfThreads;
    constexpr int countInThread = 30;

    if (tmp < 10) countOfThreads = 1;
    else countOfThreads = tmp / countInThread + (tmp % countInThread != 0);

    // generate job pieces
    futures.reserve(countOfThreads);
    for (int i = 0; i < countOfThreads; i++)
        futures.emplace_back(std::move(std::async(
                findPerspectiveMoves, avlMoves.begin() + i * countInThread,
                std::min(avlMoves.begin() + (i+1) * countInThread, avlMoves.end()),
                node)));

    moves_pq pq([&node](const std::pair<Board, int> &left, const std::pair<Board, int> &right) {
        if (node->maximize_)
            return (left.second < right.second);
        return (left.second > right.second);
    });

    // get countOfBestCandididates_ * countOfThreads best moves
    for (int i = 0; i < countOfThreads; i++)
    {
        auto result = futures[i].get();

        for (int j = 0; j < countOfBestCandididates_ && j < result.size(); j++)
		{
			pq.emplace(std::move(result[j]));
		}
    }

    // put countOfBestCandididates_ best moves in jobs queue and calculation tree
	for (int i = 0; i < countOfBestCandididates_ && !pq.empty(); i++)
	{
		node->children_.emplace_back(std::make_shared<CalcTreeNode>(
				/* std::move(const_cast<Gomoku::Board&>() */
				pq.top().first,
				node,
				node->maximize_ ^ true,
				pq.top().second));

		pq.pop();
	}

}

void Gomoku::AI1::Worker()
{
	if (debug_)
		std::cout << this->TreeToString() << std::endl;

    std::stack<std::pair<int /* i */, std::shared_ptr<CalcTreeNode>>> traverser;

    auto ToLeft = [this, &traverser](std::shared_ptr<CalcTreeNode> pr)
    {
        traverser.emplace(1, pr);

		std::tie(pr->alpha, pr->beta) = pr->parent_.expired()
										? std::make_pair(-1000, +1000)
										: std::make_pair(pr->parent_.lock()->alpha, pr->parent_.lock()->beta);

		if (debug_)
			std::cout << "[" << std::setw(3) << std::setfill(' ') << pr->positionScore_
				  << std::setw(4) << std::setfill(' ')
				  << ((!pr->state_.GetMovesList().empty()) ? Board::MoveToString(pr->state_.GetMovesList().back()) : "no")
				  << " " << pr->maximize_ << " a: " << pr->alpha << " b: " << pr->beta << "]" << "\n";

        while (pr->Depth() < this->depth_)
        {
            if (pr->children_.empty())
                GenerateChildren(pr);

            if (pr->children_.empty())
            {
            	if (debug_)
                	std::cout << "break or prune" << std::endl;
                break;
            }

            traverser.emplace(1, pr->children_[0]);

            pr = pr->children_[0];
            std::tie(pr->alpha, pr->beta) = std::tie(pr->parent_.lock()->alpha, pr->parent_.lock()->beta);

			if (debug_)
				std::cout << "[" << std::setw(3) << std::setfill(' ') << pr->positionScore_
					  << std::setw(4) << std::setfill(' ')
					  << ((!pr->state_.GetMovesList().empty()) ? Board::MoveToString(pr->state_.GetMovesList().back()) : "no")
					  << " " << pr->maximize_ << " a: " << pr->alpha << " b: " << pr->beta << "]" << "\n";
        }
    };

    std::pair<int /* depth */, std::shared_ptr<CalcTreeNode>> var;

	while (work_)
	{
		if (debug_)
	    	std::cout << "// Work start! //" << std::endl;

        auto t1 = std::chrono::high_resolution_clock::now();

        while (true)
        {
            if (!work_) return;

			if (needReload_)
			{
				needReload_ = false;

				// Change tree root
				if (!FindNext())
				{
					if (debug_)
                    	if (!currentBoard.GetMovesList().empty())
                        	std::cout << Board::MoveToString(currentBoard.GetMovesList().back()) << " ";

                    tree_ = std::make_shared<CalcTreeNode>(currentBoard, std::weak_ptr<CalcTreeNode>(), !tree_->maximize_, 0);
                }

                traverser = {};
                ToLeft(tree_);
			}
            if (toleft)
            {
                toleft = false;
                ToLeft(tree_);
            }

			// Pop out a new job or break!
            {
            	std::lock_guard lg(jobsMtx_);
                if (traverser.empty())
                	break;

                auto pair = traverser.top();
                traverser.pop();

                /// Counting alpha-beta!
                if (pair.second->maximize_)
				{
                	if (!pair.second->parent_.expired())
					{
                		pair.second->parent_.lock()->beta = std::min(pair.second->parent_.lock()->beta, pair.second->positionScore_);

						// this is first kid of a parent
						if (pair.second->parent_.lock()->children_[0] == pair.second)
							pair.second->parent_.lock()->positionScore_ = pair.second->positionScore_;
						else
							pair.second->parent_.lock()->positionScore_ = std::min(pair.second->parent_.lock()->positionScore_, pair.second->positionScore_);
					}
                	else
					{
                		if (debug_)
                			std::cout << "[root]" << std::endl;
					}
				}
                else
				{
					if (!pair.second->parent_.expired())
					{
						pair.second->parent_.lock()->alpha = std::max(pair.second->parent_.lock()->alpha, pair.second->positionScore_);

						// this is first kid of a parent
						if (pair.second->parent_.lock()->children_[0] == pair.second)
							pair.second->parent_.lock()->positionScore_ = pair.second->positionScore_;
						else
							pair.second->parent_.lock()->positionScore_ = std::max(pair.second->parent_.lock()->positionScore_, pair.second->positionScore_);
					}
					else
					{
						if (debug_)
							std::cout << "[root]" << std::endl;
					}
				}

                if (debug_)
                	std::cout << "|" << std::setw(3) << std::setfill(' ') << pair.second->positionScore_
                          << std::setw(4) << std::setfill(' ')
                          << ((!pair.second->state_.GetMovesList().empty()) ? Board::MoveToString(pair.second->state_.GetMovesList().back()) : "no")
                          << " " << pair.second->maximize_ << " a: " << pair.second->alpha << " b: " << pair.second->beta << "]" << "\n" ;

                if (pair.first < pair.second->children_.size())
                {
                	if (pair.second->alpha < pair.second->beta)
					{
                		traverser.emplace(pair.first + 1, pair.second);
						ToLeft(pair.second->children_[pair.first]);
					}
                	else
					{
                		if (debug_)
                			std::cout << "Pruned! a: " << pair.second->alpha << ", b: " << pair.second->beta << std::endl;
					}
                }
            }

        }

        // Work done...
		if (debug_)
        	std::cout << "// Work done! Duration: " << std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - t1).count() <<  " //" << std::endl;

		if (debug_)
			std::cout << this->TreeToString() << std::endl;

        if (!work_) return;

        // Lock the worker thread until some job appeared
        std::unique_lock lk(jobsCvMtx_);
        jobsCv_.wait(lk);

        if (debug_)
        	std::cout << "Getting out of lock!" << std::endl;
    }

	if (debug_)
		std::cout << "Worker died" << std::endl;
}

std::function<bool(int score1, int score2)> Gomoku::AI1::GreaterIntializer(Gomoku::Board::Side side)
{
	std::function<bool(int score1, int score2)> ret;

	if (side == Board::Side::White)
		ret = std::greater<int> ();
	else if (side == Board::Side::Black)
		ret = std::less<int> ();
	else
		throw std::runtime_error("wrong side in GreaterIntializer");

	return ret;
}

std::function<bool(int score1, int score2)> Gomoku::AI1::LessIntializer(Gomoku::Board::Side side)
{
	std::function<bool(int score1, int score2)> ret;

	if (side == Board::Side::White)
		ret = std::less<int> ();
	else if (side == Board::Side::Black)
		ret = std::greater<int> ();
	else
		throw std::runtime_error("wrong side in LessIntializer");

	return ret;
}

int Gomoku::AI1::CalcTreeNode::Depth() const
{
	int ret = 1;

	std::weak_ptr<CalcTreeNode> ptr = parent_;

	while (!ptr.expired())
	{
		ptr = ptr.lock()->parent_;
		ret++;
	}

	return ret;
}

Gomoku::AI1::CalcTreeNode::CalcTreeNode(const Gomoku::Board &bs, std::weak_ptr<CalcTreeNode> parent, bool maximize,
										int positionScore)
		: parent_(std::move(parent))
		, state_(bs)
		, maximize_(maximize)
		, positionScore_(positionScore)
		, alpha([&parent]()->int { return !parent.expired() ? parent.lock()->alpha : -1000; }())
		, beta([&parent]()->int { return !parent.expired() ? parent.lock()->beta : 1000; }())
{}

Gomoku::AI1::CalcTreeNode::CalcTreeNode(Gomoku::Board &&bs, std::weak_ptr<CalcTreeNode> parent, bool maximize, int positionScore)
		: state_(bs)
		, parent_(std::move(parent))
		, maximize_(maximize)
		, positionScore_(positionScore)
        , alpha([&parent]()->int { return !parent.expired() ? parent.lock()->alpha : -1000; }())
        , beta([&parent]()->int { return !parent.expired() ? parent.lock()->beta : +1000; }())
{}


Gomoku::AI1::CalcTreeNode::CalcTreeNode(std::weak_ptr<CalcTreeNode> parent, bool maximize, int positionScore)
        : parent_(std::move(parent))
        , maximize_(maximize)
        , positionScore_(positionScore)
        , alpha([&parent]()->int { return !parent.expired() ? parent.lock()->alpha : -101; }())
        , beta([&parent]()->int { return !parent.expired() ? parent.lock()->beta : +101; }())
{}

void Gomoku::AI1::CalcTreeNode::UpdatePositionScore()
{
	int newPositionScore;

	if (children_.empty()) return;

	if (maximize_)
	{
		newPositionScore = (*std::max_element(children_.begin(),
				children_.end(),
				[this](const std::shared_ptr<CalcTreeNode> &l, const std::shared_ptr<CalcTreeNode> &r){
					return l->positionScore_ < r->positionScore_;
		}))->positionScore_;
	}
	else
	{
		newPositionScore = (*std::min_element(children_.begin(),
				  children_.end(),
				  [this](const std::shared_ptr<CalcTreeNode> &l, const std::shared_ptr<CalcTreeNode> &r){
					  return l->positionScore_ < r->positionScore_;
				  }))->positionScore_;
	}

	this->positionScore_ = newPositionScore;
}


std::string Gomoku::AI1::TreeToString() const
{
	std::function<
	        void(std::shared_ptr<CalcTreeNode> node,
			std::string &buffer,
			std::string prefix,
			std::string children_prefix)
			>
			help = [&help](
					std::shared_ptr<CalcTreeNode> node,
					std::string &buffer,
					std::string prefix,
					std::string children_prefix) {
		buffer += prefix;

		std::stringstream ss;

		ss
			<< std::setw(3) << std::setfill(' ') << node->positionScore_
			<< std::setw(4) << std::setfill(' ')
			<< ((!node->state_.GetMovesList().empty()) ? Board::MoveToString(node->state_.GetMovesList().back()) : "no")
			<< " " << node->maximize_;

		buffer += ss.str();


		buffer += "\n";

		for (auto it = node->children_.begin(); it != node->children_.end(); it++)
		{
			if (it != node->children_.end() - 1)
				help(*it, buffer, children_prefix + "├────────── ", children_prefix + "│           ");
			else
				help(*it, buffer, children_prefix + "└────────── ", children_prefix + "            ");
		}

	};

	std::string s;

	help(tree_, s, "", "");

	return s;
}




