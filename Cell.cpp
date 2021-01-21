//
// Created by 17641238 on 19.01.2021.
//

#include "Cell.h"
#include <stdexcept>
#include <sstream>

Gomoku::Cell::Cell(const std::string& inp, float placeX, float placeY, bool pressed)
{
	if (inp.size() == 2 || inp.size() == 3)
	{
		std::stringstream ss(inp);
		char c1;
		int c2;

		ss >> c1 >> c2;

		if ('a' <= c1 && c1 <= 'z'
			&& 1 <= c2 && c2 <= 19)
		{
			this->col_ = c1 - 'a';
			this->row_ = c2 - 1;
			this->placeX_ = placeX;
			this->placeY_ = placeY;
			this->pressed_ = pressed;
			return;
		}
		else
		{
			std::stringstream ss3;
			ss3 << "Incorrect input chars: " << inp;
			throw std::logic_error(inp.c_str());
		}
	}


	std::stringstream ss3;
	ss3 << "Incorrect input format: " << inp;
	throw std::logic_error(inp.c_str());
}
#include <iostream>
Gomoku::Cell::Cell(int col, int row, float placeX, float placeY, bool pressed)
{
	if (0 <= col && col < 19
		&& 0 <= row &&  row < 19)
	{
		this->row_ = row;
		this->col_ = col;
		this->placeX_ = placeX;
		this->placeY_ = placeY;
		this->pressed_ = pressed;
		return;
	}

	std::stringstream ss3;
	ss3 << "Incorrect input for Gomoku Cell: " << col << ":" << row;
	throw std::runtime_error(ss3.str());
}

std::string Gomoku::Cell::toString() const
{
	std::stringstream ss;
	static const char *letters = "abcdefghijklmnopqrs";

	ss << letters[this->col_] << this->row_;

	return ss.str();
}
