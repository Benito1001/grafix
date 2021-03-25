# CMakeList.txt : CMake project for grafixx, include source and define
# project specific logic here.

cmake_minimum_required (VERSION 3.8)

# Add source to this project's executable.
add_executable (grafix "main.cpp" "physics.hpp" "entities.hpp" "glUtils.hpp" )
find_package(glad CONFIG REQUIRED)
target_link_libraries(grafix PRIVATE glad::glad)
find_package(glfw3 CONFIG REQUIRED)
target_link_libraries(grafix PRIVATE glfw)


# TODO: Add tests and install targets if needed.
