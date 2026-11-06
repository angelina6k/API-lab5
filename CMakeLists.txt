cmake_minimum_required(VERSION 3.20)

project(SFML_Wall_Bounce LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(SFML COMPONENTS Graphics Window System CONFIG REQUIRED)

add_executable(SFML_Wall_Bounce main.cpp)

target_link_libraries(SFML_Wall_Bounce PRIVATE
    SFML::Graphics
    SFML::Window
    SFML::System
)
