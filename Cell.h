//
// Created by 17641238 on 19.01.2021.
//

#ifndef GOMOKU_CELL_H
#define GOMOKU_CELL_H

#include <string>

namespace Gomoku
{

class Cell
{
public:
	Cell() = delete;
	explicit Cell(const std::string& inp, float placeX, float placeY, bool pressed);
	explicit Cell(int col, int row, float placeX, float placeY, bool pressed); // Zero-indexed
	[[nodiscard]] std::string toString() const;
	float placeX_, placeY_;
	bool pressed_ = false;
	int row_, col_;

};

}



#endif //GOMOKU_CELL_H
