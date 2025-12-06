cmake_minimum_required(VERSION 3.10)
project(SinWave)
set(CMAKE_CXX_STANDARD 17)

add_definitions(-DTRACY_ENABLE)

include_directories(lib lib/glad/include lib/glfw/include lib/glm )

add_subdirectory(lib/glfw)

add_library(glad STATIC lib/glad/src/glad.c)

set(SOURCES main.cpp)

add_executable(SinWave ${SOURCES})

target_link_libraries(SinWave
    glad
    glfw
    ${CMAKE_DL_LIBS}
)