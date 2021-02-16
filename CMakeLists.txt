cmake_minimum_required(VERSION 3.15)
project(gomoku)

set(CMAKE_CXX_STANDARD 20)


# <IMGUI>
find_package (
        glfw3 REQUIRED
)

find_package(OpenGL REQUIRED)
find_package(Boost COMPONENTS system filesystem REQUIRED)
include_directories(${OPENGL_INCLUDE_DIR})

find_package(GLEW REQUIRED)
if (GLEW_FOUND)
    include_directories(${GLEW_INCLUDE_DIRS})
    #link_libraries(${GLEW_LIBRARIES})
endif()

include_directories(imgui/examples/libs/gl3w)
include_directories(little_imgui)


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

# </IMGUI>

# <PGNLIB>

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
        pgnlib/PGNTagList.h
		Game.cpp Game.h)
# </PGNLIB>


#configure_file(${CMAKE_CURRENT_SOURCE_DIR}/textures ${CMAKE_CURRENT_BINARY_DIR} COPYONLY)
#configure_file(${CMAKE_CURRENT_SOURCE_DIR}/test_files ${CMAKE_CURRENT_BINARY_DIR} COPYONLY)
#
#file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/textures DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
#file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/test_files DESTINATION ${CMAKE_CURRENT_BINARY_DIR})



add_executable(${PROJECT_NAME} main.cpp GomokuDraw.cpp GomokuDraw.h
        ${IMGUI_SOURCES}
        ${PGNLIB_SRC}
        Board.cpp Board.h tests.cpp ChessClock.cpp ChessClock.h Players/IPlayer.cpp Players/IPlayer.h Players/AI1.cpp Players/AI1.h Players/Human.cpp Players/Human.h Engine.cpp Engine.h Players/AI2.cpp Players/AI2.h)

target_include_directories(${PROJECT_NAME} PUBLIC imgui_little)

enable_testing()
find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIR})

target_link_libraries(${PROJECT_NAME}
        glfw
        ${OPENGL_LIBRARIES}
        GLEW::GLEW
        ${Boost_LIBRARIES}
        # TEST
        ${GTEST_LIBRARY}
        ${GTEST_MAIN_LIBRARY}
        )


add_test(
        NAME runUnitTests
        COMMAND runUnitTests
)

if(CMAKE_BUILD_TYPE MATCHES Debug)
    target_compile_definitions(${PROJECT_NAME} PUBLIC DEBUG)
endif()

if (${ONLYTEST})
	target_compile_definitions(${PROJECT_NAME} PUBLIC ONLYTEST=1)
endif()