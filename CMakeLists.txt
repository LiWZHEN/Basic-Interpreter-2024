cmake_minimum_required(VERSION 3.16)
project(code)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(
    Basic
    Basic/Utils
    StanfordCPPLib
)

file(GLOB SOURCES
     "Basic/*.cpp"
     "Basic/Utils/*.cpp"
     "StanfordCPPLib/*.cpp"
)

add_executable(code ${SOURCES})
