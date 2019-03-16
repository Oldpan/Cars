cmake_minimum_required(VERSION 3.13)
project(Cars)

set(CMAKE_CXX_STANDARD 11)

file(GLOB Sources *.cpp )
file(GLOB Includes *.h )

add_executable(Cars ${Sources} ${Includes})
