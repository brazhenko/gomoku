//
// Created by 17641238 on 18.01.2021.
//

#include "GomokuDraw.h"
#include <unordered_map>
#include "imgui_little/imgui.h"
#include "imgui_little/imgui_impl_glfw.h"
#include "imgui_little/imgui_impl_opengl3.h"
#include <iostream>
#include <optional>

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
	textureHelper() = default;

	explicit textureHelper(const std::string &path)
	{
		bool ret = LoadTextureFromFile(path.c_str(), &my_image_texture, &image_width, &image_height);
		if (!ret) throw std::runtime_error("Cannot load texture");
	}

	int image_width = 0;
	int image_height = 0;
	GLuint my_image_texture = 0;
};

GLFWwindow* window;
std::unordered_map<std::string, textureHelper> textures;

namespace GomokuDraw
{
	void glfw_error_callback(int error, const char* description)
	{
		fprintf(stderr, "Glfw Error %d: %s\n", error, description);
	}

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


		textures["example"] = textureHelper("../MyImage01.jpg");
		textures["background"] = textureHelper("textures/main.png");

		textures["fantom_stone_blue"] = textureHelper("textures/light_blue.png");
		textures["fantom_stone_red"] = textureHelper("textures/light_red.png");

		textures["stone_blue"] = textureHelper("textures/blue.png");
		textures["stone_red"] = textureHelper("textures/red.png");

		textures["forbidden"] = textureHelper("textures/forbidden.png");

		return true;
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


		ImGui::GetWindowDrawList()->AddImage((void*)(intptr_t)textures["background"].my_image_texture, ImVec2{0, 0}, ImVec2{1280, 720});

		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

	}

	int HandleWindowClick()
	{
		// std::cout << ImGui::IsMouseClicked(0) << std::endl;
		return 0;
	}

	constexpr float textureCellSide = 28.0;

	std::optional<Gomoku::Cell> HandleBoardInteraction()
	{
		// board 570x570 pixels
		constexpr float upperLeftBoard_x = 50.0;
		constexpr float upperLeftBoard_y = 60.0;

		constexpr float downRightBoard_x = 620.0;
		constexpr float downRightBoard_y = 630.0;

		constexpr float boardSizePixels = 570.0;

		const auto &mPt = ImGui::GetMousePos();

		constexpr int cellSide = 30;


		static const char *letterCoordinates = "abcdefghijklmnopqrs";

		const float xCoordinate = mPt.x - float((int)(mPt.x - upperLeftBoard_x) % cellSide) + 1;
		const float yCoordinate = mPt.y - float((int)(mPt.y - upperLeftBoard_y) % cellSide) + 1;

		if (upperLeftBoard_x <= mPt.x && mPt.x < downRightBoard_x
			&& upperLeftBoard_y <= mPt.y && mPt.y < downRightBoard_y)
		{
//			ImGui::GetWindowDrawList()->AddImage(
//					(void*)(intptr_t)textures.at("fantom_stone_red").my_image_texture,
//					// Координата верхнего левого угла
//					ImVec2{xCoordinate, yCoordinate},
//					// Координата нижнего правого угла
//					ImVec2{xCoordinate + textureCellSide, yCoordinate + textureCellSide});

//			std::cerr << "Cell: " << letterCoordinates[(int)(mPt.x - upperLeftBoard_x) / cellSide]
//				<< 19 - (int)(mPt.y - upperLeftBoard_y) / cellSide << std::endl;

			return Gomoku::Cell((int)(mPt.x - upperLeftBoard_x) / cellSide,
					19 - (int)(mPt.y - upperLeftBoard_y) / cellSide - 1,
					xCoordinate,
					yCoordinate,
					ImGui::IsMouseClicked(0));

		}

		return std::nullopt;
	}

	void DrawStone(float xCoordinate, float yCoordinate, int type)
	{
		if (type == 1)
			ImGui::GetWindowDrawList()->AddImage(
				(void*)(intptr_t)textures.at("fantom_stone_red").my_image_texture,
				// Координата верхнего левого угла
				ImVec2{xCoordinate, yCoordinate},
				// Координата нижнего правого угла
				ImVec2{xCoordinate + textureCellSide, yCoordinate + textureCellSide});
		else if (type == 2)
			ImGui::GetWindowDrawList()->AddImage(
					(void*)(intptr_t)textures.at("stone_red").my_image_texture,
					// Координата верхнего левого угла
					ImVec2{xCoordinate, yCoordinate},
					// Координата нижнего правого угла
					ImVec2{xCoordinate + textureCellSide, yCoordinate + textureCellSide});
		else if (type == 3)
			ImGui::GetWindowDrawList()->AddImage(
					(void*)(intptr_t)textures.at("fantom_stone_blue").my_image_texture,
					// Координата верхнего левого угла
					ImVec2{xCoordinate, yCoordinate},
					// Координата нижнего правого угла
					ImVec2{xCoordinate + textureCellSide, yCoordinate + textureCellSide});
		else if (type == 4)
			ImGui::GetWindowDrawList()->AddImage(
					(void*)(intptr_t)textures.at("stone_blue").my_image_texture,
					// Координата верхнего левого угла
					ImVec2{xCoordinate, yCoordinate},
					// Координата нижнего правого угла
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
}