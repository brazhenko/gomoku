cmake_minimum_required(VERSION 3.15)
project(gomoku)

set(CMAKE_CXX_STANDARD 20)


# <IMGUI>
find_package (
        glfw3 REQUIRED
)

find_package(OpenGL REQUIRED)
include_directories(${OPENGL_INCLUDE_DIR})

find_package(GLEW REQUIRED)
if (GLEW_FOUND)
    include_directories(${GLEW_INCLUDE_DIRS})
    #link_libraries(${GLEW_LIBRARIES})
endif()

include_directories(imgui)
include_directories(imgui/backends)
include_directories(imgui/examples)
include_directories(imgui/examples/libs/gl3w)
include_directories(/usr/local/include)
include_directories(/opt/local/include)
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
        imgui_little/stb_image.h)

# </IMGUI>




add_executable(${PROJECT_NAME} main.cpp
        ${IMGUI_SOURCES}
        )

target_link_libraries(${PROJECT_NAME}
        glfw
        ${OPENGL_LIBRARIES}
        GLEW::GLEW
        )