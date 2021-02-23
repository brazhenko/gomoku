//
// Created by Lommy Reznak on 2/3/21.
//

#ifndef GOMOKU_AI1_H
#define GOMOKU_AI1_H

#include <utility>
#include <mutex>
#include <atomic>
#include "IPlayer.h"
#include <thread>
#include <queue>
namespace Gomoku
{
	class AI1 : public IPlayer
	{
        using moves_pq = std::priority_queue<
                std::pair<Board, int>,
                std::vector<std::pair<Board, int>>,
                std::function<bool
                (const std::pair<Board, int> &left,
                const std::pair<Board, int> &right)>>;

        struct CalcNode
        {
            CalcNode() = delete;
            explicit CalcNode(const Board& bs, std::weak_ptr<CalcNode> parent, bool maximize)
                    : parent_(std::move(parent))
                    , state_(bs)
                    , maximize_(maximize)
            {}
            explicit CalcNode(Board&& bs, std::weak_ptr<CalcNode> parent, bool maximize)
                    : state_(bs)
                    , parent_(std::move(parent))
					, maximize_(maximize)
            {}

            [[nodiscard]] bool IsMax() const
			{
            	return false;
			}


            int positionScore = 0;
            const bool maximize_ = false;
            Board state_;

            const std::weak_ptr<CalcNode>			parent_;
            std::vector<std::shared_ptr<CalcNode>>	children_;
        };


        bool FindNext();
        void Worker();
        std::thread             workerThread_;
        std::queue<std::pair<int /* depth */, std::shared_ptr<CalcNode>>>	jobs_;
        std::mutex	            jobs_mtx_;
        std::condition_variable jobs_cv_;
        std::mutex              jobs_cv_mtx_;
        std::atomic_int         depth_ = 6;

		std::atomic_bool        work_ = true;
		std::shared_ptr<CalcNode>	tree;

    public:
        static auto lessIntializer(Board::Side side)
		{
			std::function<bool(int score1, int score2)> ret;

			if (side == Board::Side::White)
				ret = std::less<int>{};
			else if (side == Board::Side::Black)
				ret = std::greater<int>{};
			else
				throw std::runtime_error("wrong side in lessIntializer");

			return ret;
		}
        static auto greaterIntializer(Board::Side side)
		{
			std::function<bool(int score1, int score2)> ret;

			if (side == Board::Side::White)
				ret = std::greater<int>{};
			else if (side == Board::Side::Black)
				ret = std::less<int>{};
			else
				throw std::runtime_error("wrong side in greaterIntializer");

			return ret;
		}
        static auto minInitializer(Board::Side side)
		{
			if (Board::Side::White == side)
				return -100;
			else if (Board::Side::Black == side)
				return 100;

			throw std::runtime_error("wrong side in minInitializer");
		}
		static auto maxInitializer(Board::Side side)
		{
			if (Board::Side::White == side)
				return 100;
			else if (Board::Side::Black == side)
				return -100;

			throw std::runtime_error("wrong side in minInitializer");
		}

        std::function<bool(int score1, int score2)> score1BetterThenScore2;
		std::function<bool(int score1, int score2)> score1WorseThenScore2;

		const int Min, Max;
        static constexpr std::chrono::milliseconds maxTimeToThink { 500 };
        std::chrono::system_clock::time_point startThinking{};

        Gomoku::Board::pcell nextMove {};

		explicit AI1(Board::Side side, MakeMove_t MakeMove, const Gomoku::Board &realBoard, bool yourTurn)
				: IPlayer(side, std::move(MakeMove), realBoard)
				, score1BetterThenScore2(greaterIntializer(side))
				, score1WorseThenScore2(lessIntializer(side))
				, Min(minInitializer(side))
				, Max(minInitializer(side))
				// Initializing calculating tree
				, tree(std::make_shared<CalcNode>(realBoard, std::weak_ptr<CalcNode>(), yourTurn))
				// Starting calculating thread
				, workerThread_([this](){ Worker(); })

		{
			if (yourTurn) AI1::YourTurn();
		}

		~AI1()
        {
		    work_ = false;
            { std::lock_guard<std::mutex> lg(jobs_mtx_); jobs_ = {}; }
            jobs_cv_.notify_one();

            workerThread_.join();
        }


		void YourTurn() override;

		Board::MoveResult Ping() override;
	};
}




#endif //GOMOKU_AI1_H
