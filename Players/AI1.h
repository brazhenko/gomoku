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
            void UpdatePositionScore();
        };

		using moves_pq = std::priority_queue<
				std::pair<Board, int>,
				std::vector<std::pair<Board, int>>,
				std::function<bool
				(const std::pair<Board, int> &left,
				 const std::pair<Board, int> &right)>>;

		std::queue<std::shared_ptr<CalcTreeNode>>	jobs_;
        std::mutex	            jobsMtx_;
        std::condition_variable jobsCv_;
        std::mutex              jobsCvMtx_;
        std::atomic_int         depth_ = 5;
		static constexpr int	countOfBestCandididates_ = 3;
		std::atomic_int         count_found = 0;
        std::atomic_bool 		needReload_ = false;
		std::atomic_bool        work_;
		std::thread             workerThread_;
		std::shared_ptr<CalcTreeNode>	tree;
		int 					best_{};

		std::function<bool(int score1, int score2)> score1BetterThenScore2;
		std::function<bool(int score1, int score2)> score1WorseThenScore2;

		std::mutex				nextMoveMtx_;
		Gomoku::Board::pcell 	nextMove_ {};

		const int min_, max_;
		static constexpr std::chrono::milliseconds maxTimeToThink_  { 2'000 };
		std::chrono::system_clock::time_point startedThinkingAt_{};

		bool FindNext();
		void Worker();

		static std::function<bool(int score1, int score2)> LessIntializer(Board::Side side);
		static std::function<bool(int score1, int score2)> GreaterIntializer(Board::Side side);
		static int MinInitializer(Board::Side side);
		static int MaxInitializer(Board::Side side);
    public:
		AI1() = delete;
		explicit AI1(Board::Side side, MakeMove_t MakeMove, const Gomoku::Board &realBoard, bool yourTurn);
		~AI1();

		void YourTurn() override;
		Board::MoveResult Ping() override;
	};
}




#endif //GOMOKU_AI1_H
