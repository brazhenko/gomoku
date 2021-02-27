//
// Created by Lommy Reznak on 2/3/21.
//

#include "AI1.h"
#include "../Engine.h"
#include <map>
#include <future>
#include <queue>

Gomoku::AI1::AI1(Gomoku::Board::Side side, Gomoku::MakeMove_t MakeMove, const Gomoku::Board &realBoard, bool yourTurn)
		: IPlayer(side, std::move(MakeMove), realBoard)
		, score1BetterThenScore2(GreaterIntializer(side))
		, score1WorseThenScore2(LessIntializer(side))
		, min_(MinInitializer(side))
		, max_(MaxInitializer(side))
		// Initializing calculating tree
		, tree(std::make_shared<CalcTreeNode>(realBoard, std::weak_ptr<CalcTreeNode>(), true, 0))
		, jobs_ { [this]() { std::deque<std::shared_ptr<CalcTreeNode>> ret; ret.push_back(tree); return ret; }() }
		// Starting calculating thread
		, work_(true)
		, workerThread_([this](){ Worker(); })
{
	if (yourTurn) AI1::YourTurn();
}

Gomoku::AI1::~AI1()
{
	work_ = false;

	{
		std::lock_guard lg(jobsMtx_);
		jobs_ = {};
	}

	jobsCv_.notify_one();
	workerThread_.join();

	std::cerr << "Found in tree: " << count_found << std::endl;
}



void Gomoku::AI1::YourTurn()
{
	myMove = true;
	startedThinkingAt_ = std::chrono::system_clock::now();

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

        std::cout << "Making Move" << std::endl;

		Gomoku::Board::MoveResult ret;
        {
			std::lock_guard lg(nextMoveMtx_);
			std::lock_guard lg2 {jobsMtx_};

			for (auto &child : tree->children_)
			{
				std::cout << Board::MoveToString(child->state_.GetMovesList().back()) << "~" << child->positionScore_ << ";";
			}

			nextMove_ =  (*std::max_element(
					tree->children_.begin(),
					tree->children_.end(),
				   [this](const std::shared_ptr<CalcTreeNode> &l, const std::shared_ptr<CalcTreeNode> &r){
						   return score1WorseThenScore2(l->positionScore_, r->positionScore_);
				   }))->state_.GetMovesList().back();

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
    std::cout << std::boolalpha << tree->maximize_ << " ";

    for (auto it = tree->children_.begin(); it != tree->children_.end(); it++)
	{
		std::cout << Board::MoveToString((*it)->state_.GetMovesList().back()) << "~" << (*it)->positionScore_ << " ";
		if ((*it)->state_ == currentBoard)
		{
			tree = *it;
			tree->parent_.reset();
			return true;
		}
	}

	return false;
}

void Gomoku::AI1::Worker()
{
	std::cout << "SIZE:" << jobs_.size() << std::endl;

    constexpr int localityRadius = 2;

    auto findPerspectiveMoves = [this](std::vector<Board::pcell>::const_iterator left,
                                       std::vector<Board::pcell>::const_iterator right,
                                       const std::shared_ptr<CalcTreeNode>& node)
    {
        std::vector<std::pair<Board, int>> pm;
        pm.reserve(right - left);


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

    std::shared_ptr<CalcTreeNode> var;

	while (work_.load())
	{
	    std::cout << "// Work start! //" << std::endl;
        auto t1 = std::chrono::high_resolution_clock::now();

        while (true)
        {

            if (!work_) return;

			if (needReload_)
			{
                std::lock_guard lg(jobsMtx_);
				// cleaning
				needReload_ = false;
				// Change tree root
				if (!FindNext())
				{
					if (!currentBoard.GetMovesList().empty())
						std::cout << Board::MoveToString(currentBoard.GetMovesList().back()) << " ";
					std::cout << "Not found, generating new" << std::endl;
					tree = std::make_shared<CalcTreeNode>(currentBoard, std::weak_ptr<CalcTreeNode>(), !tree->maximize_, 0);
//					std::cout << "l0" << std::endl;
				}
				else
				{
					std::cout << "found child, replacing" << std::endl;
					count_found++;
				}


				// Empty queue
				jobs_ = {};
				jobs_.emplace_back(tree);
				best_ = min_;
			}

			// Pop out a new job or break!
            {
                std::lock_guard lg(jobsMtx_);

                if (jobs_.empty()
                	|| jobs_.front()->Depth() > depth_ /* Tree is fully built */)
                	break;

//				std::cout << "Depth: " << jobs_.front()->Depth() << " size: " << jobs_.size() << std::endl;
                var = jobs_.front();
                jobs_.pop_front();

//                // costyl
//				if (var->Depth() == 2 && score1BetterThenScore2(var->positionScore_, this->best_))
//				{
//					this->nextMove_ = var->state_.GetMovesList().back();
//					this->best_ = var->positionScore_;
//				}


                if (!var->children_.empty())
				{
                	for (auto &child : var->children_)
                		jobs_.push_back(child);

                	// go and take new job!
					continue;
				}
            }

//			std::cout << "test1" << std::endl;

            // vector of job split in to pieces
            std::vector<std::future<std::vector<std::pair<Board, int>>>> futures;
            const auto &avlMoves = var->state_.GetAvailableMoves();

            int tmp = avlMoves.size();
            int countOfThreads;
            constexpr int countInThread = 30;
            if (tmp < 10)
                countOfThreads = 1;
            else
                countOfThreads = tmp / countInThread + (tmp % countInThread != 0);

            // generate job pieces
            futures.reserve(countOfThreads);
            for (int i = 0; i < countOfThreads; i++)
                futures.emplace_back(std::move(std::async(
                        findPerspectiveMoves, avlMoves.begin() + i * countInThread,
                        std::min(avlMoves.begin() + (i+1) * countInThread, avlMoves.end()),
                        var)));

            moves_pq pq([this, &var](const std::pair<Board, int> &left, const std::pair<Board, int> &right) {
				if (var->maximize_)
					return (left.second < right.second);
				return (left.second > right.second);
            });

//			std::cout << "test2" << std::endl;

            // get countOfBestCandididates_ * countOfThreads best moves
            for (int i = 0; i < countOfThreads; i++)
            {
                auto result = futures[i].get();

                for (int j = 0; j < countOfBestCandididates_ && j < result.size(); j++)
                    pq.emplace(std::move(result[j]));
            }

//			std::cout << "bug3" << std::endl;

            // put countOfBestCandididates_ best moves in jobs queue and calculation tree
			{
				std::lock_guard lg(jobsMtx_);

				for (int i = 0; i < countOfBestCandididates_ && !pq.empty(); i++)
				{
//					// costyl
//					if (var->Depth() == 2 && score1BetterThenScore2(var->positionScore_, this->best_))
//					{
//						this->nextMove_ = var->state_.GetMovesList().back();
//						this->best_ = var->positionScore_;
//					}

					var->children_.emplace_back(std::make_shared<CalcTreeNode>(
							/* std::move(const_cast<Gomoku::Board&>() */
							pq.top().first,
							var,
							var->maximize_ ^ true,
							pq.top().second));
					pq.pop();

					jobs_.emplace_back(var->children_.back());
				}

				std::weak_ptr<CalcTreeNode> iterator = var;
				while (!iterator.expired())
				{
					auto var2 = iterator.lock();
					var2->UpdatePositionScore();
					iterator = var2->parent_;
				}
			}

        }

        auto t2 = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count();

        std::cout << "// Work done! Duration: " << duration << " | Jobs: " << jobs_.size() << " //" << std::endl;
		std::cout << this->TreeToString() << std::endl;

        if (!work_) return;

        // Lock the worker thread until some job appeared
        std::unique_lock lk(jobsCvMtx_);
        jobsCv_.wait(lk);

        std::cout << "Getting out of lock!" << std::endl;
    }
	std::cout << "Worker died" << std::endl;
}

int Gomoku::AI1::MaxInitializer(Gomoku::Board::Side side)
{
	if (Board::Side::White == side)
		return 100;
	else if (Board::Side::Black == side)
		return -100;

	throw std::runtime_error("wrong side in MaxInitializer");
}

int Gomoku::AI1::MinInitializer(Gomoku::Board::Side side)
{
	if (Board::Side::White == side)
		return -100;
	else if (Board::Side::Black == side)
		return 100;

	throw std::runtime_error("wrong side in MinInitializer");
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
{}

Gomoku::AI1::CalcTreeNode::CalcTreeNode(Gomoku::Board &&bs, std::weak_ptr<CalcTreeNode> parent, bool maximize, int positionScore)
		: state_(bs)
		, parent_(std::move(parent))
		, maximize_(maximize)
		, positionScore_(positionScore)
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
		ss << std::setw(3) << std::setfill(' ') << node->positionScore_;
		ss << std::setw(4) << std::setfill(' ');

		ss << ((!node->state_.GetMovesList().empty()) ? Board::MoveToString(node->state_.GetMovesList().back()) : "no");

//		if (!node->state_.GetMovesList().empty())
//			ss << Board::MoveToString(node->state_.GetMovesList().back());
//		else ss << "no";

		ss << " " << node->maximize_;


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

	help(tree, s, "", "");

	return s;
}
