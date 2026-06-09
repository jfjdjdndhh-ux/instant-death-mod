cmake_minimum_required(VERSION 3.21)
project(InstantDeath VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(InstantDeath SHARED src/main.cpp)

CPMAddPackage("gh:geode-sdk/geode#main")
target_link_libraries(InstantDeath geode-sdk)
