#include "GomokuDraw.h"
#include "imgui_little/imgui.h"
#include <unordered_map>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>

bool LoadTextureFromFile(const char* filename, GLuint* out_texture, int* out_width, int* out_height);

#include <PGNGameCollection.h>
#include <iostream>
#include <fstream>
#include "PGNGame.h"
#include <sstream>
#include "Game.h"

int main()
{
//	try
//	{
//		std::ifstream pgnfile("./sample.pgn");
//
//		pgn::TagList tl;
//		tl.insert(pgn::Tag("Game", "Gomoku"));
//		pgn::MoveList ml;
//		pgn::GameResult gr;
//
//
//
//		ml.push_back(pgn::Move(pgn::Ply("i14"), pgn::Ply("e5"), 1));
//
//
//
//		std::stringstream ss;
//		ss << pgn::Game(tl, ml, gr) << std::endl;
//
//		pgn::Game g2;
//
//		ss >> g2;
//
//		std::cerr << g2;
//
//
//
//	}
//	catch (std::exception &e)
//	{
//		std::cerr << "exception: " << e.what() << std::endl;
//		return -1;
//	}
//
//	return 0;


	Gomoku::Game game{};


	if (!GomokuDraw::Init())
	{
		std::cerr << "Cannot init GomokuDraw" << std::endl;
		return (-1);
	}


	// Main loop
	while (GomokuDraw::Go())
	{
		if (game.state_ == Gomoku::Game::State::Start)
		{
			// Отрисовать какие-то меньюшки, туториалы и прочее.

		}
		else if (game.state_ == Gomoku::Game::State::Main)
		{
			GomokuDraw::DrawSome();

			auto cell = GomokuDraw::HandleBoardInteraction();
			game.ProcessStone(cell);

			GomokuDraw::HandleWindowClick();

			ImGui::End();
		}

		GomokuDraw::Render();
	}
	GomokuDraw::Cleanup();

	return 0;
}


