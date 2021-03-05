cmake_minimum_required(VERSION 3.15)
project(gomoku)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address")

# DOXYGEN
find_package(Doxygen REQUIRED)
set(DOXYGEN_INPUT ${CMAKE_SOURCE_DIR})

# Preparation
add_custom_target(make_doc COMMAND ${DOXYGEN_EXECUTABLE} WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
add_custom_target(delete_old_doc COMMAND rm -rf ${CMAKE_CURRENT_BINARY_DIR}/html)
add_custom_target(done_documentation COMMAND mv -f ${CMAKE_CURRENT_SOURCE_DIR}/html ${CMAKE_CURRENT_BINARY_DIR} DEPENDS delete_old_doc make_doc)
add_custom_target(copy_textures COMMAND cp -r ${CMAKE_CURRENT_SOURCE_DIR}/textures ${CMAKE_CURRENT_BINARY_DIR})
add_custom_target(copy_test_files COMMAND cp -r ${CMAKE_CURRENT_SOURCE_DIR}/test_files ${CMAKE_CURRENT_BINARY_DIR})

# IMGUI
find_package (glfw3 REQUIRED)
find_package(OpenGL REQUIRED)
include_directories(imgui/examples/libs/gl3w)
include_directories(little_imgui)
include_directories(${OPENGL_INCLUDE_DIR})

find_package(Boost COMPONENTS system filesystem REQUIRED)

find_package(GLEW REQUIRED)
if (GLEW_FOUND)
	include_directories(${GLEW_INCLUDE_DIRS})
endif()

SET(IMGUI_SOURCES
	imgui_little/imgui.cpp
	imgui_little/imgui.h
	imgui_little/imconfig.h
	imgui_little/imgui_internal.h
	imgui_little/imgui_demo.cpp
	imgui_little/imgui_tables.cpp
	imgui_little/imgui_draw.cpp
	imgui_little/imgui_widgets.cpp
	imgui_little/imgui_impl_opengl3.cpp
	imgui_little/imgui_impl_glfw.cpp
	imgui_little/stb_image.h
	imgui_little/imfilebrowser.hpp
	imgui_little/TextWithColors.hpp
)
# /IMGUI

# PGNLIB
include_directories(pgnlib)
SET(PGNLIB_SRC
        pgnlib/PGNCommentText.cpp
        pgnlib/PGNCommentText.h
        pgnlib/PGNException.cpp
        pgnlib/PGNException.h
        pgnlib/PGNGame.cpp
        pgnlib/PGNGame.h
        pgnlib/PGNGameCollection.cpp
        pgnlib/PGNGameCollection.h
        pgnlib/PGNGameResult.cpp
        pgnlib/PGNGameResult.h
        pgnlib/PGNMove.cpp
        pgnlib/PGNMove.h
        pgnlib/PGNMoveList.cpp
        pgnlib/PGNMoveList.h
        pgnlib/PGNParser.cpp
        pgnlib/PGNParser.h
        pgnlib/PGNPiece.cpp
        pgnlib/PGNPiece.h
        pgnlib/PGNPly.cpp
        pgnlib/PGNPly.h
        pgnlib/PGNPosition.cpp
        pgnlib/PGNPosition.h
        pgnlib/PGNSquare.cpp
        pgnlib/PGNSquare.h
        pgnlib/PGNTag.cpp
        pgnlib/PGNTag.h
        pgnlib/PGNTagList.cpp
        pgnlib/PGNTagList.h)
# /PGNLIB


enable_testing()
find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIR})

add_executable(${PROJECT_NAME}
		main.cpp
		Game.cpp
		Game.h
		GomokuDraw.cpp
		GomokuDraw.h
		${IMGUI_SOURCES}
		${PGNLIB_SRC}
		Board.cpp
		Board.h
		tests.cpp
		ChessClock.cpp
		ChessClock.h
		Players/IPlayer.cpp
		Players/IPlayer.h
		Players/AI1.cpp
		Players/AI1.h
		Players/Human.cpp
		Players/Human.h
		Engine.cpp
		Engine.h
		Players/AI2.cpp
		Players/AI2.h)

target_include_directories(${PROJECT_NAME} PUBLIC imgui_little)

target_link_libraries(${PROJECT_NAME}
        glfw
        ${OPENGL_LIBRARIES}
        GLEW::GLEW
        ${Boost_LIBRARIES}
        # TEST
        ${GTEST_LIBRARY}
        ${GTEST_MAIN_LIBRARY}
        )

add_dependencies(${PROJECT_NAME} done_documentation copy_textures copy_test_files)


if(CMAKE_BUILD_TYPE MATCHES Debug)
    target_compile_definitions(${PROJECT_NAME} PUBLIC DEBUG)
endif()

if (${ONLYTEST})
	target_compile_definitions(${PROJECT_NAME} PUBLIC ONLYTEST=1)
endif()