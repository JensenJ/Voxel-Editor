cmake_minimum_required(VERSION 3.20)
project(
    voxel_editor
    VERSION 1.0
    LANGUAGES C CXX
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

option(ENABLE_CLANG_TIDY "Enable clang-tidy analysis" OFF)

if (ENABLE_CLANG_TIDY)
    find_program(CLANG_TIDY_EXE NAMES clang-tidy)
    if (CLANG_TIDY_EXE)
        set(CMAKE_CXX_CLANG_TIDY 
            ${CLANG_TIDY_EXE};
            -warnings-as-errors=clang-analyzer-*;bugprone-*
        )
    else()
        message(WARNING "clang-tidy requested but not found")
	endif()
endif()

#Dependencies
include(FetchContent)

#GLM
FetchContent_Declare(
	glm
	GIT_REPOSITORY https://github.com/g-truc/glm.git
	GIT_TAG 1.0.3
)
FetchContent_MakeAvailable(glm)

#GLFW
FetchContent_Declare(
    glfw
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG 3.4
)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(glfw)

#GLAD
FetchContent_Declare(
    glad
    GIT_REPOSITORY https://github.com/Dav1dde/glad.git
    GIT_TAG        v2.0.8
    GIT_PROGRESS   TRUE
    SOURCE_SUBDIR  cmake
)

FetchContent_MakeAvailable(glad)
glad_add_library(glad_gl_core_41 STATIC REPRODUCIBLE LOADER API gl:core=4.1)

#ImGUI
FetchContent_Declare(
    imgui
    GIT_REPOSITORY https://github.com/ocornut/imgui.git
    GIT_TAG v1.92.5-docking
)
FetchContent_MakeAvailable(imgui)

add_library(imgui STATIC
    ${imgui_SOURCE_DIR}/imgui.cpp
	${imgui_SOURCE_DIR}/imgui_demo.cpp
    ${imgui_SOURCE_DIR}/imgui_draw.cpp
    ${imgui_SOURCE_DIR}/imgui_tables.cpp
    ${imgui_SOURCE_DIR}/imgui_widgets.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp
)

target_include_directories(imgui PUBLIC
    ${imgui_SOURCE_DIR}
    ${imgui_SOURCE_DIR}/backends
)

if(UNIX AND NOT APPLE)
    find_package(X11 REQUIRED)
	target_link_libraries(imgui PRIVATE
		X11::X11
		X11::Xrandr
		X11::Xcursor
		X11::Xi
)
endif()

target_link_libraries(imgui PRIVATE glfw glad_gl_core_41)

#OpenGL
find_package(OpenGL REQUIRED)

#Compilation
file(GLOB_RECURSE PROJECT_SOURCES
    CONFIGURE_DEPENDS
    ${CMAKE_SOURCE_DIR}/src/*.cpp
)

add_executable(voxel_editor ${PROJECT_SOURCES})

if(UNIX AND NOT APPLE)
    target_link_libraries(voxel_editor PRIVATE
    X11::X11
    X11::Xrandr
    X11::Xcursor
    X11::Xi
)

endif()

#Link libs
target_link_libraries(voxel_editor PRIVATE
    OpenGL::GL
    glfw
    glad_gl_core_41
    glm::glm
    imgui
)

# Copy resources directory to runtime output directory
add_custom_command(
    TARGET voxel_editor POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_SOURCE_DIR}/resources
            $<TARGET_FILE_DIR:voxel_editor>/resources
)

target_compile_definitions(voxel_editor PRIVATE GLM_ENABLE_EXPERIMENTAL)

if (WIN32)
    target_compile_definitions(voxel_editor PRIVATE NOMINMAX WIN32_LEAN_AND_MEAN)
endif()