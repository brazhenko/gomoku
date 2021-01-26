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
#include "imgui.h"

#include "gtest/gtest.h"

#include "imfilebrowser.hpp"

int main()
{
#if DEBUG
	testing::InitGoogleTest();
	if (RUN_ALL_TESTS()) return (-1);
#endif

//	try
//	{
//		std::ifstream pgnfile("./sample.pgn");
//
//		pgn::TagList tl;
//		tl.insert(pgn::Tag("Game", "Gomoku"));
//		pgn::MoveList ml;
//		pgn::GameResult gr;
//
//		ml.push_back(pgn::Move(pgn::Ply("i14"), pgn::Ply("e5"), 1));
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

	ImGui::FileBrowser fileDialog;

	// (optional) set browser properties
	fileDialog.SetTitle("Select game file...");
	fileDialog.SetTypeFilters({ ".pgn" });
	fileDialog.SetPwd(getenv("HOME"));

	// Main loop
	while (GomokuDraw::Go())
	{

		if (game.state_ == Gomoku::Game::State::Start)
		{
			// Отрисовать какие-то меньюшки, туториалы и прочее.
		}
		// Главный экран с движком и игрой
		else if (game.state_ == Gomoku::Game::State::Main)
		{
			GomokuDraw::DrawSome();

			for (const auto &stone : game.postedStones)
				GomokuDraw::DrawStone(stone.first.placeX_, stone.first.placeY_, stone.second);

			if (!fileDialog.IsOpened())
			{
				auto cell = GomokuDraw::HandleBoardInteraction();
				game.ProcessStone(cell);
			}

			GomokuDraw::HandleWindowClick();

			ImGui::SetNextWindowSize(ImVec2{1259 - 660, 349 - 40});
			ImGui::SetNextWindowPos(ImVec2{660, 40});
			ImGui::Begin("Game2", nullptr);

			if (ImGui::Button("clear board"))
				game.Reset();

			// open file dialog when user clicks this button
			if(ImGui::Button("open file dialog"))
				fileDialog.Open();

			ImGui::End();

			ImGui::SetNextWindowSize(ImVec2{1259 - 660, 679 - 370});
			ImGui::SetNextWindowPos(ImVec2{660, 370}); // 1259, 679
			ImGui::Begin("Game", nullptr);

			for (int i = 0; i < game.board_.GetMovesList().size(); i++)
			{
				ImGui::Text("%d. %s  ", i + 1,
						Gomoku::BoardState::MoveToString(game.board_.GetMovesList()[i]).c_str());
				ImGui::SameLine();
				if ((i + 1) % 8 == 0)
					ImGui::NewLine();
			}

			ImGui::End();



			ImGui::End();
		}


		if(fileDialog.HasSelected())
		{
			std::cout << "Selected filename" << fileDialog.GetSelected().string() << std::endl;
			fileDialog.ClearSelected();
		}


		fileDialog.Display();

		if(fileDialog.HasSelected())
		{
			std::cout << "Selected filename" << fileDialog.GetSelected().string() << std::endl;
			fileDialog.ClearSelected();
		}

		GomokuDraw::Render();
	}
	GomokuDraw::Cleanup();

	return 0;
}


