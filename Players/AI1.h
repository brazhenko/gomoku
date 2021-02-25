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

        struct CalcTreeNode
        {
			int			positionScore_;
			const bool	maximize_;
			Board		state_;

			std::weak_ptr<CalcTreeNode>					parent_;
			std::vector<std::shared_ptr<CalcTreeNode>>	children_;

            CalcTreeNode() = delete;
            explicit CalcTreeNode(const Board& bs, std::weak_ptr<CalcTreeNode> parent, bool maximize, int positionScore);
            explicit CalcTreeNode(Board&& bs, std::weak_ptr<CalcTreeNode> parent, bool maximize, int positionScore);

            [[nodiscard]] int Depth() const;
        };


        bool FindNext();
        void Worker();

        std::thread             workerThread_;
        std::queue<std::shared_ptr<CalcTreeNode>>	jobs_;
        std::mutex	            jobs_mtx_;
        std::condition_variable jobs_cv_;
        std::mutex              jobs_cv_mtx_;
        std::atomic_int         depth_ = 3;
		static constexpr int	countOfBestCandididates = 5;
		std::atomic_int         count_found = 0;
        std::atomic_bool 		need_reload = false;
		std::atomic_bool        work_;
		std::shared_ptr<CalcTreeNode>	tree;
		int 						best_{};
    public:
        static std::function<bool(int score1, int score2)> lessIntializer(Board::Side side);
        static std::function<bool(int score1, int score2)> greaterIntializer(Board::Side side);
        static int minInitializer(Board::Side side);
		static int maxInitializer(Board::Side side);

        std::function<bool(int score1, int score2)> score1BetterThenScore2;
		std::function<bool(int score1, int score2)> score1WorseThenScore2;

		const int Min, Max;
        static constexpr std::chrono::milliseconds maxTimeToThink { 2'000 };
        std::chrono::system_clock::time_point startThinking{};

        Gomoku::Board::pcell nextMove {};

		explicit AI1(Board::Side side, MakeMove_t MakeMove, const Gomoku::Board &realBoard, bool yourTurn)
				: IPlayer(side, std::move(MakeMove), realBoard)
				, score1BetterThenScore2(greaterIntializer(side))
				, score1WorseThenScore2(lessIntializer(side))
				, Min(minInitializer(side))
				, Max(minInitializer(side))
				// Initializing calculating tree
				// , tree(std::make_shared<CalcTreeNode>(realBoard, std::weak_ptr<CalcTreeNode>(), yourTurn, 0))
				, tree(std::make_shared<CalcTreeNode>(Gomoku::Board{}, std::weak_ptr<CalcTreeNode>(), yourTurn, 0))
				// Starting calculating thread
				, work_(true)
				, workerThread_([this](){ Worker(); })
		{
			if (yourTurn) AI1::YourTurn();
		}

		~AI1()
        {
		    work_ = false;

            {
                std::lock_guard lg(jobs_mtx_);
                jobs_ = {};
            }

            jobs_cv_.notify_one();
            workerThread_.join();

            std::cerr << "Found in tree: " << count_found << std::endl;
        }


		void YourTurn() override;

		Board::MoveResult Ping() override;
	};
}




#endif //GOMOKU_AI1_H
