//
// Created by 17641238 on 18.01.2021.
//

#include "GomokuDraw.h"
#include <unordered_map>
#include "imgui_internal.h"
#include "imgui_little/imgui.h"
#include "imgui_little/imgui_impl_glfw.h"
#include "imgui_little/imgui_impl_opengl3.h"
#include <iostream>
#include <optional>
#include "TextWithColors.hpp"
#include <climits>

// About Desktop OpenGL function loaders:
//  Modern desktop OpenGL doesn't have a standard portable header file to load OpenGL function pointers.
//  Helper libraries are often used for this purpose! Here we are supporting a few common ones (gl3w, glew, glad).
//  You may use another loader/header of your choice (glext, glLoadGen, etc.), or chose to manually implement your own.
#if defined(IMGUI_IMPL_OPENGL_LOADER_GL3W)
#include <GL/gl3w.h>            // Initialize with gl3wInit()
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLEW)
#include <GL/glew.h>            // Initialize with glewInit()
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLAD)
#include <glad/glad.h>          // Initialize with gladLoadGL()
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLAD2)
#include <glad/gl.h>            // Initialize with gladLoadGL(...) or gladLoaderLoadGL()
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLBINDING2)
#define GLFW_INCLUDE_NONE       // GLFW including OpenGL headers causes ambiguity or multiple definition errors.
#include <glbinding/Binding.h>  // Initialize with glbinding::Binding::initialize()
#include <glbinding/gl/gl.h>
using namespace gl;
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLBINDING3)
#define GLFW_INCLUDE_NONE       // GLFW including OpenGL headers causes ambiguity or multiple definition errors.
#include <glbinding/glbinding.h>// Initialize with glbinding::initialize()
#include <glbinding/gl/gl.h>
using namespace gl;
#else
#include IMGUI_IMPL_OPENGL_LOADER_CUSTOM
#endif

// Include glfw3.h after our OpenGL definitions
#include <GLFW/glfw3.h>

// [Win32] Our example includes a copy of glfw3.lib pre-compiled with VS2010 to maximize ease of testing and compatibility with old VS compilers.
// To link with VS2010-era libraries, VS2015+ requires linking with legacy_stdio_definitions.lib, which we do using this pragma.
// Your own project should not be affected, as you are likely to link with a newer binary of GLFW that is adequate for your version of Visual Studio.
#if defined(_MSC_VER) && (_MSC_VER >= 1900) && !defined(IMGUI_DISABLE_WIN32_FUNCTIONS)
#pragma comment(lib, "legacy_stdio_definitions")
#endif

#define STB_IMAGE_IMPLEMENTATION
#include "imgui_little/stb_image.h"

// Simple helper function to load an image into a OpenGL texture with common settings
bool LoadTextureFromFile(const char* filename, GLuint* out_texture, int* out_width, int* out_height)
{
	// Load from file
	int image_width = 0;
	int image_height = 0;
	unsigned char* image_data = stbi_load(filename, &image_width, &image_height, nullptr, 4);
	if (image_data == nullptr)
		return false;

	// Create a OpenGL texture identifier
	GLuint image_texture;
	glGenTextures(1, &image_texture);
	glBindTexture(GL_TEXTURE_2D, image_texture);

	// Setup filtering parameters for display
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // This is required on WebGL for non power-of-two textures
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); // Same

	// Upload pixels into texture
#if defined(GL_UNPACK_ROW_LENGTH) && !defined(__EMSCRIPTEN__)
	glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
#endif
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image_width, image_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image_data);
	stbi_image_free(image_data);

	*out_texture = image_texture;
	*out_width = image_width;
	*out_height = image_height;

	return true;
}

struct textureHelper
{
	textureHelper() = delete;

	explicit textureHelper(const std::string &path)
	{
		bool ret = LoadTextureFromFile(path.c_str(), &my_image_texture, &image_width, &image_height);
		if (!ret) throw std::runtime_error("Cannot load texture");
	}

	int image_width = 0;
	int image_height = 0;
	GLuint my_image_texture = 0;
};

// Global GomokuDraw context variables
GLFWwindow* window;
std::unordered_map<std::string, textureHelper> textures;
ImGui::FileBrowser fileDialogBoardPos;
ImGui::FileBrowser fileDialogGame;

namespace GomokuDraw
{
	void glfw_error_callback(int error, const char* description)
	{
		fprintf(stderr, "Glfw Error %d: %s\n", error, description);
	}

	static const char* gameModes[] = { "42", "Classic", "Omok" };
	static int gameModeId = 0;

	static const char* gameTimes[] = { "1 minute", "3 minutes", "5 minutes", "10 minutes", "15 minutes" };
	static int gameTimeId = 0;

	bool Init()
	{
		// Setup window
		glfwSetErrorCallback(glfw_error_callback);
		if (!glfwInit())
			return false;

		// Decide GL+GLSL versions
#ifdef __APPLE__
		// GL 3.2 + GLSL 150
		const char* glsl_version = "#version 150";
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
		glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // Required on Mac
#else
		// GL 3.0 + GLSL 130
		const char* glsl_version = "#version 130";
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
		//glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
		//glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // 3.0+ only
#endif

		glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
		// Create window with graphics context
		window = glfwCreateWindow(1280, 720, "gomoku", nullptr, nullptr);
		if (window == nullptr)
			return false;

		glfwMakeContextCurrent(window);
		glfwSwapInterval(1); // Enable vsync

		// Initialize OpenGL loader
#if defined(IMGUI_IMPL_OPENGL_LOADER_GL3W)
		bool err = gl3wInit() != 0;
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLEW)
		bool err = glewInit() != GLEW_OK;
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLAD)
		bool err = gladLoadGL() == 0;
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLAD2)
    	bool err = gladLoadGL(glfwGetProcAddress) == 0; // glad2 recommend using the windowing library loader instead of the (optionally) bundled one.
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLBINDING2)
    	bool err = false;
    	glbinding::Binding::initialize();
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLBINDING3)
    	bool err = false;
    	glbinding::initialize([](const char* name) { return (glbinding::ProcAddress)glfwGetProcAddress(name); });
#else
    	bool err = false; // If you use IMGUI_IMPL_OPENGL_LOADER_CUSTOM, your loader is likely to requires some form of initialization.
#endif
		if (err)
		{
			std::cerr << "Failed to initialize OpenGL loader!" << std::endl;
			return false;
		}

		// Setup Dear ImGui context
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO();
		//io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
		//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

		// Setup Dear ImGui style
		ImGui::StyleColorsDark();

		// Setup Platform/Renderer backends
		ImGui_ImplGlfw_InitForOpenGL(window, true);
		ImGui_ImplOpenGL3_Init(glsl_version);

		// Prepare textures
		textures.emplace("background", "textures/main.png");
		textures.emplace("fantom_stone_blue", "textures/light_blue.png");
		textures.emplace("fantom_stone_red", "textures/light_red.png");
		textures.emplace("stone_blue", "textures/blue.png");
		textures.emplace("stone_red", "textures/red.png");
		textures.emplace("forbidden", "textures/forbidden.png");

		// Set default filedialog parameters
		fileDialogGame.SetTitle("Select game file...");
		fileDialogGame.SetTypeFilters({ ".pgn" });
		fileDialogGame.SetPwd(getenv("HOME"));

		fileDialogBoardPos.SetTitle("Select game position file...");
		fileDialogBoardPos.SetTypeFilters({ ".gg" });
		fileDialogBoardPos.SetPwd(getenv("HOME"));

		return true;
	}

	static void MakeNextObjectInActive()
	{
		ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
		ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
	}

	static void MakeNextObjectActive()
	{
		ImGui::PopItemFlag();
		ImGui::PopStyleVar();
	}


	bool Go()
	{
		auto ret = !glfwWindowShouldClose(window);

		if (ret)
		{
			glfwPollEvents();

			// Start the Dear ImGui frame
			ImGui_ImplOpenGL3_NewFrame();
			ImGui_ImplGlfw_NewFrame();
			ImGui::NewFrame();
		}

		return ret;
	}

	void DrawSome()
	{
		ImGui::SetNextWindowSize(ImVec2{1280, 720});
		ImGui::SetNextWindowPos(ImVec2{0, 0});
		ImGui::Begin("Main Window", nullptr,
				ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBackground
						| ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoSavedSettings
						| ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoBringToFrontOnFocus);


		ImGui::GetWindowDrawList()->AddImage((void*)(intptr_t)textures.at("background").my_image_texture, ImVec2{0, 0}, ImVec2{1280, 720});

#if DEBUG
		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
#endif
	}

	constexpr float textureCellSide = 28.0;
	constexpr float upperLeftBoard_x = 50.0;
	constexpr float upperLeftBoard_y = 60.0;

	constexpr float downRightBoard_x = 620.0;
	constexpr float downRightBoard_y = 630.0;

	constexpr float boardSizePixels = 570.0;
	constexpr int cellSide = 30;

	// Mouse MUST be inside the board !
	std::pair<int, int> MouseCoordinatesToStonePosition(float x_, float y_)
	{
		return {
			19 - (int)(y_ - upperLeftBoard_y) / cellSide - 1,
			(int)(x_ - upperLeftBoard_x) / cellSide
		};
	}

	std::pair<float, float> StonePositionToPrintCoorinates(std::pair<int, int> stone)
	{
		return {
				upperLeftBoard_x + (stone.second * cellSide),
			downRightBoard_y - (stone.first * cellSide) - 30
		};
	}

	bool MouseInsideBoard()
	{
		const auto &mPt = ImGui::GetMousePos();

		return upperLeftBoard_x <= mPt.x && mPt.x < downRightBoard_x
			   && upperLeftBoard_y <= mPt.y && mPt.y < downRightBoard_y;
	}

	void DrawStone(float xCoordinate, float yCoordinate, int type)
	{
		if (type == 1)
			ImGui::GetWindowDrawList()->AddImage(
				(void*)(intptr_t)textures.at("fantom_stone_red").my_image_texture,
                // Координата верхнего левого угла
				ImVec2{ xCoordinate, yCoordinate },
                // Координата нижнего правого угла
				ImVec2{xCoordinate + textureCellSide, yCoordinate + textureCellSide});
		else if (type == 2)
			ImGui::GetWindowDrawList()->AddImage(
				(void*)(intptr_t)textures.at("stone_red").my_image_texture,
				ImVec2{xCoordinate, yCoordinate},
				ImVec2{xCoordinate + textureCellSide, yCoordinate + textureCellSide});
		else if (type == 3)
			ImGui::GetWindowDrawList()->AddImage(
				(void*)(intptr_t)textures.at("fantom_stone_blue").my_image_texture,
				ImVec2{xCoordinate, yCoordinate},
				ImVec2{xCoordinate + textureCellSide, yCoordinate + textureCellSide});
		else if (type == 4)
			ImGui::GetWindowDrawList()->AddImage(
				(void*)(intptr_t)textures.at("stone_blue").my_image_texture,
				ImVec2{xCoordinate, yCoordinate},
				ImVec2{xCoordinate + textureCellSide, yCoordinate + textureCellSide});
	}

	void ForbiddenCursor()
	{
		ImGui::SetMouseCursor(ImGuiMouseCursor_None);

		ImGui::GetWindowDrawList()->AddImage(
				(void*)(intptr_t)textures.at("forbidden").my_image_texture,
				// Координата верхнего левого угла
				ImVec2{ImGui::GetMousePos().x, ImGui::GetMousePos().y},
				// Координата нижнего правого угла
				ImVec2{ImGui::GetMousePos().x + 15, ImGui::GetMousePos().y + 15});
	}

	void DrawStones(const Gomoku::Board &bs)
	{
		// Draw all stones on the board
		for (int row = 0; row < Gomoku::Board::cells_in_line; row++)
			for (int col = 0; col < Gomoku::Board::cells_in_line; col++)
			{
				auto stoneType = bs.At(row, col);

				if (stoneType == Gomoku::Board::Side::White)
				{
					auto placeToDraw = GomokuDraw::StonePositionToPrintCoorinates({row, col});
					GomokuDraw::DrawStone(placeToDraw.first, placeToDraw.second, 2);
				}
				else if (bs.At(row, col) == Gomoku::Board::Side::Black)
				{
					auto placeToDraw = GomokuDraw::StonePositionToPrintCoorinates({row, col});
					GomokuDraw::DrawStone(placeToDraw.first, placeToDraw.second, 4);
				}
			}
	}

	void Render()
	{
		const ImVec4 clear_color = ImVec4(1.45f, 0.55f, 0.60f, 1.00f);
		// Rendering
		ImGui::Render();
		int display_w, display_h;
		glfwGetFramebufferSize(window, &display_w, &display_h);
		glViewport(0, 0, display_w, display_h);
		glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
		glClear(GL_COLOR_BUFFER_BIT);
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		glfwSwapBuffers(window);
	}

	void Cleanup()
	{
		// Cleanup
		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();

		glfwDestroyWindow(window);
		glfwTerminate();
	}

	static const char* items[] = { "Human", "AI1", "AI2", "AI3" };
	static int player1 = 0;
	static int player2 = 0;
	static bool enableEngine = true;

	void DrawPlayer(const Gomoku::Game &game, bool isWhite)
	{
		int count = game.board_.GetCapturePoints(!isWhite ? Gomoku::Board::Side::White : Gomoku::Board::Side::Black);
		ImGui::BeginGroup();
		{
			{
				ImDrawList* draw_list = ImGui::GetWindowDrawList();
				ImVec2 pos = ImGui::GetCursorScreenPos();
				for (int i = 0; i < 5; i++) {
					ImVec2 marker_min = ImVec2(pos.x + 21 * i , pos.y);
					ImVec2 marker_max = ImVec2(pos.x + 21 * i + 15, pos.y + 15);
					int color = i > 4 - count ? 0 : 255;
					draw_list->AddRectFilled(marker_min, marker_max, IM_COL32(isWhite ? color : 0, 0, isWhite ? 0 : color, 255));
				}
				// ImGui::PushTextWrapPos(ImGui::GetCursorPos().x + wrap_width);
				ImGui::Dummy(ImVec2(15.0f, 4.0f));
			}
			ImGui::Text("    ");

			if (game.state_ != Gomoku::Game::State::Main)
				MakeNextObjectInActive();

			if (isWhite)
				ImGui::Combo("    ##1", &player1, items, IM_ARRAYSIZE(items)); 
			else
				ImGui::Combo("    ##2", &player2, items, IM_ARRAYSIZE(items));

			if (game.state_ != Gomoku::Game::State::Main)
				MakeNextObjectActive();

			ImDrawList* draw_list = ImGui::GetWindowDrawList();
			static float wrap_width = 70.0f;
			ImGui::Text("Time left");
			ImVec2 pos = ImGui::GetCursorScreenPos();
			ImVec2 marker_min = ImVec2(pos.x, pos.y);
			ImVec2 marker_max = ImVec2(pos.x + wrap_width, pos.y + 30);
			draw_list->AddRectFilled(marker_min, marker_max, IM_COL32(170, 100, 50, 255));

			// TODO проверить, что тут нормальное условие в ифе
			if ((isWhite && game.board_.WhiteMove()) || (!isWhite && !game.board_.WhiteMove()))
				draw_list->AddRect(marker_min, marker_max, IM_COL32(255, 255, 0, 255));
			ImGui::PushTextWrapPos(ImGui::GetCursorPos().x + wrap_width);
			ImGui::Dummy(ImVec2(15.0f, 4.0f));
			// ImGui::Dummy(ImVec2(15.0f, 1.0f));
			// ImGui::SameLine();

			if (isWhite)
			    ImGui::Text(" %s", game.clock_.GetTimeLeftWhite().c_str());
            else
                ImGui::Text(" %s", game.clock_.GetTimeLeftBlack().c_str());

			ImGui::PopTextWrapPos();
			ImGui::Dummy(ImVec2(15.0f, 10.0f));

			ImGui::Text("Last move time:");
			pos = ImGui::GetCursorScreenPos();
			marker_min = ImVec2(pos.x, pos.y);
			marker_max = ImVec2(pos.x + wrap_width, pos.y + 25);
			draw_list->AddRectFilled(marker_min, marker_max, IM_COL32(50, 166, 20, 255));
			ImGui::Dummy(ImVec2(15.0f, 2.0f));
			ImGui::PushTextWrapPos(ImGui::GetCursorPos().x + wrap_width);
			if (isWhite)
				ImGui::Text(" %s", game.clock_.GetTimeSpentWhite().c_str());
			else
				ImGui::Text(" %s", game.clock_.GetTimeSpentBlack().c_str());

			ImGui::PopTextWrapPos();
		}
		ImGui::EndGroup();
	}


	void DrawButtons(Gomoku::Game &game)
	{
		ImGui::BeginGroup();
		{
		    // Start button
			auto tmp = game.state_;
			if (tmp == Gomoku::Game::State::GameInProcess
				|| tmp == Gomoku::Game::State::GameEndedWhiteWin
			   	|| tmp == Gomoku::Game::State::GameEndedBlackWin
			  	|| tmp == Gomoku::Game::State::GameEndedDraw
			  	)
				MakeNextObjectInActive();
			if (ImGui::Button("start"))
				game.Go(items[player1], items[player2], gameModes[gameModeId], gameTimes[gameTimeId]);
			if (tmp == Gomoku::Game::State::GameInProcess
				|| tmp == Gomoku::Game::State::GameEndedWhiteWin
				|| tmp == Gomoku::Game::State::GameEndedBlackWin
				|| tmp == Gomoku::Game::State::GameEndedDraw)
				MakeNextObjectActive();

            // Pause button
			ImGui::SameLine();
			if (tmp == Gomoku::Game::State::Main || tmp == Gomoku::Game::State::GameInPause
				|| tmp == Gomoku::Game::State::GameEndedWhiteWin
				|| tmp == Gomoku::Game::State::GameEndedBlackWin
				|| tmp == Gomoku::Game::State::GameEndedDraw)
				MakeNextObjectInActive();
			if (ImGui::Button("pause"))
				game.Pause();
			if (tmp == Gomoku::Game::State::Main || tmp == Gomoku::Game::State::GameInPause
				|| tmp == Gomoku::Game::State::GameEndedWhiteWin
				|| tmp == Gomoku::Game::State::GameEndedBlackWin
				|| tmp == Gomoku::Game::State::GameEndedDraw)
				MakeNextObjectActive();

			// Stop button
			ImGui::SameLine();
			if (tmp == Gomoku::Game::State::Main)
				MakeNextObjectInActive();
			if (ImGui::Button("stop"))
				game.Stop();
			if (tmp == Gomoku::Game::State::Main)
				MakeNextObjectActive();

			// Restart button
			ImGui::SameLine();
			if (ImGui::Button("restart"))
			{
				game.Stop();
				game.Go(items[player1], items[player2], gameModes[gameModeId], gameTimes[gameTimeId]);
			}

			// Takeback button
			ImGui::SameLine();
			if (ImGui::Button("takeback"))
				game.TakeBack();

		}
		ImGui::EndGroup();
	}


	/**
	 * Returns the path to the current user's desktop.
	 */
	static char *path2desktop()
	{
		static char real_public_path[PATH_MAX + 1] = {0};
		if (real_public_path[0])
			return real_public_path;
		strcpy(real_public_path, getenv("HOME"));
		memcpy(real_public_path + strlen(real_public_path), "/Desktop", 8);
		return real_public_path;
	}

	void DrawFilesButtons(Gomoku::Game &game)
	{
		ImGui::Dummy(ImVec2(20.0f, 129.0f));
		ImGui::BeginGroup();
		if (ImGui::Button("save fen"))
		{
			std::stringstream fn;
			auto t = std::time(nullptr);
			auto tm = *std::localtime(&t);

			fn << path2desktop()
				 << "/gomoku_"
				 << std::put_time(&tm, "%d-%m-%Y%H-%M-%S")
				 << ".gg";

			std::cerr << "path: " << fn.str() << std::endl;

			std::ofstream ofs { fn.str() };

			if (ofs.is_open())
				ofs << game.board_ << std::endl;
			else
				std::cerr << "Cannot open file: " << fn.str() << std::endl;
		}

		ImGui::Dummy(ImVec2(20.0f, 4.0f));

		if (ImGui::Button("load fen"))
			fileDialogBoardPos.Open();

		fileDialogBoardPos.Display();

		if(fileDialogBoardPos.HasSelected())
		{
			std::ifstream ifs { fileDialogBoardPos.GetSelected().string() };

			if (ifs.is_open())
				ifs >> game.board_;
			else
				std::cerr << "Cannot open file: " << fileDialogBoardPos.GetSelected().string() <<  std::endl;

			fileDialogBoardPos.ClearSelected();
		}


		ImGui::EndGroup();
		ImGui::SameLine();
		ImGui::Dummy(ImVec2(20.0f, 1.0f));
		ImGui::SameLine();
		ImGui::BeginGroup();

		if (ImGui::Button("save pgn"))
		{
			try
			{
				std::stringstream fn;
				auto t = std::time(nullptr);
				auto tm = *std::localtime(&t);

				fn
					<< path2desktop()
					<< "/game_" << std::put_time(&tm, "%d-%m-%Y%H-%M-%S")
					<< ".pgn";

				std::ofstream ofs { fn.str() };

				if (ofs.is_open())
					ofs << game.board_.ToPgnString() << std::endl;
				else
					std::cerr << "Cannot open file: " << fn.str() << std::endl;
			}
			catch (std::exception &e)
			{
				std::cerr << "exception: " << e.what() << std::endl;
			}
		}

		ImGui::Dummy(ImVec2(20.0f, 4.0f));
		if (ImGui::Button("load pgn"))
			fileDialogGame.Open();
		fileDialogGame.Display();

		if(fileDialogGame.HasSelected())
		{
			std::ifstream ifs { fileDialogGame.GetSelected().string() };

			if (ifs.is_open())
			{
				pgn::Game g;

				ifs >> g;
				std::cerr << "Loaded game" << std::endl << g << std::endl;

				std::vector<std::pair<int, int>> moves;

				for (const auto & move: g.moves())
				{
					if (move.white().valid())
						moves.push_back(Gomoku::Board::StringToMove(move.white().str()));
					if (move.black().valid())
						moves.push_back(Gomoku::Board::StringToMove(move.black().str()));
				}

				game.board_ = Gomoku::Board(moves);
			}
			else
				std::cerr << "Cannot open file: " << fileDialogBoardPos.GetSelected().string() <<  std::endl;

			fileDialogGame.ClearSelected();
		}
		ImGui::EndGroup();
	}

	void DrawGameMenu(Gomoku::Game &game)
	{
		ImGui::SetNextWindowSize(ImVec2{1259 - 660, 349 - 40});
		ImGui::SetNextWindowPos(ImVec2{660, 40});
		ImGui::Begin("Game2", nullptr);

		// static bool isDisable = false;
		static float f0 = 1.0f, f1 = 2.0f, f2 = 3.0f;
		ImGui::PushItemWidth(100);
		if (game.state_ != Gomoku::Game::State::Main)
			MakeNextObjectInActive();

		ImGui::BeginGroup();
		ImGui::Combo("mode <-- Game --> time", &gameModeId, gameModes, IM_ARRAYSIZE(gameModes));
		ImGui::SameLine();
		ImGui::Combo("", &gameTimeId, gameTimes, IM_ARRAYSIZE(gameTimes));
		ImGui::EndGroup();

		if (game.state_ != Gomoku::Game::State::Main)
			MakeNextObjectActive();

		ImGui::Dummy(ImVec2(15.0f, 15.0f));
		ImGui::BeginGroup();
		{
			GomokuDraw::DrawPlayer(game, true);
			ImGui::SameLine();
			GomokuDraw::DrawPlayer(game,false);
			ImGui::Dummy(ImVec2(110.0f, 20.0f));

			// GomokuDraw::DrawSteps(game);
			// ImGui::Dummy(ImVec2(130.0f, 20.0f));
			ImGui::Text("Games status: ");
			ImGui::SameLine();

			if (game.state_ == Gomoku::Game::State::Main)
			{
				ImGui::Text("Not started");
			}
			else if (game.state_ == Gomoku::Game::State::GameEndedWhiteWin)
			{
				ImGui::TextColored(ImVec4(0.75f, 0.0f, 0.0f, 1.0f), "Red Wins");
			}
			else if (game.state_ == Gomoku::Game::State::GameEndedBlackWin)
			{
				ImGui::TextColored(ImVec4(0.0f, 0.0f, 1.0f, 1.0f), "Blue Wins");
			}
			else if (game.state_ == Gomoku::Game::State::GameEndedDraw)
			{
				ImGui::TextColored(ImVec4(1.0f, 0.0f, 1.0f, 1.0f), "Draw");
			}
			else
			{
				ImGui::Text("In progress");
			}
			ImGui::Text("");
			GomokuDraw::DrawButtons(game);
		}
		ImGui::EndGroup();
		ImGui::SameLine();
		ImGui::Dummy(ImVec2(110.0f, 1.0f));
		ImGui::SameLine();
		ImGui::BeginGroup();
		{
			// Arrow buttons with Repeater
			if (ImGui::Checkbox("Analysis", &enableEngine))
			{
				if (enableEngine) // Engine enabled
				{

				}
				else
				{

				}
			}
			if (enableEngine)
            {
                if (game.state_ == Gomoku::Game::State::GameEndedBlackWin
                    || game.state_ == Gomoku::Game::State::GameEndedWhiteWin
                    || game.state_ == Gomoku::Game::State::GameEndedDraw)
                    ImGui::Text("-");
                else
                    ImGui::Text("%d", Gomoku::Engine::StaticPositionAnalize(game.board_));
            }


			GomokuDraw::DrawFilesButtons(game);
		}

		ImGui::PopItemWidth();
		ImGui::EndGroup();

		ImGui::End();
	}

	void DrawGameMoves(const Gomoku::Board &bs)
	{
		ImGui::SetNextWindowSize(ImVec2{1259 - 660, 679 - 370});
		ImGui::SetNextWindowPos(ImVec2{660, 370}); // 1259, 679
		ImGui::Begin("Game", nullptr);

		int helper = 0;
		for (int i = 0; i < bs.GetMovesList().size(); i += 2)
		{
			if (i + 1 < bs.GetMovesList().size())
				TextWithColors( "{e6e600}% 3d. {ffffff}% 3s % 3s  ",
						i/2 + 1,
						Gomoku::Board::MoveToString(bs.GetMovesList()[i]).c_str(),
						Gomoku::Board::MoveToString(bs.GetMovesList()[i + 1]).c_str());
			else
				TextWithColors( "{e6e600}% 3d. {ffffff}% 3s ...",
						i/2 + 1,
						Gomoku::Board::MoveToString(bs.GetMovesList()[i]).c_str());

			helper++;
			ImGui::SameLine();
			if (helper == 5)
			{
				ImGui::NewLine();
				helper = 0;
			}
		}

		ImGui::End();
	}
}