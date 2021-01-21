//
// Created by 17641238 on 19.01.2021.
//

#ifndef GOMOKU_BOARD_H
#define GOMOKU_BOARD_H

#include <vector>

namespace Gomoku
{
	class Board
	{
	public:
		int arr[19][19]{0};
		[[nodiscard]] bool isFree(int col, int row) const;

	};

}



#endif //GOMOKU_BOARD_H
