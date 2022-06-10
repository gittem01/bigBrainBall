cmake_minimum_required(VERSION 3.0)
project(eFingBall VERSION 2.0.0)

set(CMAKE_CXX_STANDARD 17)
add_definitions(-DGL_SLIENCE_DEPRECATION)

add_subdirectory(${CMAKE_SOURCE_DIR}/external/box2d/src)
add_subdirectory(${CMAKE_SOURCE_DIR}/external/SDL)
add_subdirectory(${CMAKE_SOURCE_DIR}/external/SDL_image)
add_subdirectory(${CMAKE_SOURCE_DIR}/external/SDL_ttf)


include_directories(${CMAKE_SOURCE_DIR}/include)
include_directories(${CMAKE_SOURCE_DIR}/external/SDL/include)
include_directories(${CMAKE_SOURCE_DIR}/external/SDL_image)
include_directories(${CMAKE_SOURCE_DIR}/external/SDL_ttf)
include_directories(${CMAKE_SOURCE_DIR}/external/box2d/include)


file(GLOB sourceFiles ${CMAKE_SOURCE_DIR}/src/*.cpp)
file(GLOB headerFiles ${CMAKE_SOURCE_DIR}/include/*.h)
#add_library(mines ${sourceFiles} ${headerFiles})

add_executable(balLup Source.cpp ${sourceFiles})

target_link_libraries(balLup box2d)
target_link_libraries(balLup SDL2main)
target_link_libraries(balLup SDL2-static)
target_link_libraries(balLup SDL2_image::SDL2_image)
target_link_libraries(balLup SDL2_ttf::SDL2_ttf)