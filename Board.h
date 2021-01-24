//
// Created by 17641238 on 19.01.2021.
//

#ifndef GOMOKU_BOARD_H
#define GOMOKU_BOARD_H

#include <vector>
#include <cstdint>
#include <array>
#include <unordered_set>

# define ROW_BIT_SIZE (38U)
# define REVERSE_LINE(n) { \
	uint32_t ret = 0, power = 63; \
                           \
	while (n != 0) { \
		ret += (n & 1U) << power; \
		n = n >> 1U; \
		power -= 1U; \
	} \
	n = ret;                  \
	n <<= (64U - ROW_BIT_SIZE);	\
}

namespace Gomoku
{
	class Board
	{
	public:
		int arr[19][19]{0};
		[[nodiscard]] bool isFree(int col, int row) const;

	};


	class BoardState
	{
		std::array<uint64_t, 19> cols{};
		std::array<uint64_t, 19> rows{};
		std::array<uint64_t, 37> downLines{};
		std::array<uint64_t, 37> upLines{};

		void RotateBy45Degrees()
		{

		}

		void MirrorHorisontal()
		{
			for (auto &row: rows)
				REVERSE_LINE(row);

			std::reverse(cols.begin(), cols.end());

		}

		void MirrorVertical()
		{

		}

	public:
		[[nodiscard]] size_t hash() const
		{
			std::size_t seed = rows.size() + cols.size() + downLines.size() + upLines.size();

			for(const auto& i : rows)
				seed ^= i + 0x9e3779b9 + (seed << 6U) + (seed >> 2U);
			for(const auto& i : cols)
				seed ^= i + 0x9e3779b9 + (seed << 6U) + (seed >> 2U);
			for(const auto& i : downLines)
				seed ^= i + 0x9e3779b9 + (seed << 6U) + (seed >> 2U);
			for(const auto& i : upLines)
				seed ^= i + 0x9e3779b9 + (seed << 6U) + (seed >> 2U);

			return seed;
		}
		BoardState();
		std::unordered_set<BoardState> meth();
	};
}

namespace std {
	template <>
	struct hash<Gomoku::BoardState> {
		std::size_t operator()(const Gomoku::BoardState& k) const {
			return k.hash();
		}
	};
}



#endif //GOMOKU_BOARD_H
