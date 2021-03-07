//
// Created by Brazhenko Andrew on 2/3/21.
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

			int alpha = -1000;
			int beta = +1000;

			std::weak_ptr<CalcTreeNode>					parent_;
			std::vector<std::shared_ptr<CalcTreeNode>>	children_;

            CalcTreeNode() = delete;
            explicit CalcTreeNode(const Board& bs, std::weak_ptr<CalcTreeNode> parent, bool maximize, int positionScore);
            explicit CalcTreeNode(Board&& bs, std::weak_ptr<CalcTreeNode> parent, bool maximize, int positionScore);
            explicit CalcTreeNode(std::weak_ptr<CalcTreeNode> parent, bool maximize, int positionScore);
            [[nodiscard]] int Depth() const;
            void UpdatePositionScore();
        };

		std::string TreeToString() const;

		using moves_pq = std::priority_queue<
				std::pair<Board, int>,
				std::vector<std::pair<Board, int>>,
				std::function<bool
				(const std::pair<Board, int> &left,
				 const std::pair<Board, int> &right)>>;

		std::function<bool(int score1, int score2)> score1BetterThenScore2;
		std::function<bool(int score1, int score2)> score1WorseThenScore2;

        const int         		depth_ = 5;
        const int 				countOfBestCandididates_ = 3;
		Gomoku::Board::pcell 	nextMove_ {};

		std::chrono::system_clock::time_point startedThinkingAt_{};
		const std::chrono::milliseconds maxTimeToThink_  {  6'000 };
		const bool debug_ = false;

        std::atomic_bool 		needReload_ = false;

        bool					toleft = true;

		std::shared_ptr<CalcTreeNode>	tree_;

		std::mutex	            jobsMtx_;
		std::condition_variable jobsCv_;
		std::mutex              jobsCvMtx_;

		std::mutex				nextMoveMtx_;

		bool                 work_;
		std::thread             workerThread_;


		bool FindNext();
		void Worker();
        void GenerateChildren(std::shared_ptr<CalcTreeNode> &node);

		static std::function<bool(int score1, int score2)> LessIntializer(Board::Side side);
		static std::function<bool(int score1, int score2)> GreaterIntializer(Board::Side side);
    public:
		AI1() = delete;
		explicit AI1(Board::Side side,
				MakeMove_t MakeMove,
				const Gomoku::Board &realBoard,
				int depth,
				int width,
				int timeToThinkMS,
				bool debug,
				bool yourTurn);

		// Test ctor
        explicit AI1(Board::Side side, MakeMove_t MakeMove, const Gomoku::Board &realBoard, bool yourTurn, bool test);
		~AI1();

		void YourTurn() override;
		Board::MoveResult Ping() override;

	};
}

#endif //GOMOKU_AI1_H
