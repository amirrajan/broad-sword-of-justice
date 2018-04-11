project(broad-sword-of-justice)

cmake_minimum_required(VERSION 2.8)

set(SOURCES
AnimatedSprite.cpp
FPS.cpp
Game.cpp
Input.cpp
main.cpp
OS.cpp
Player.cpp
Renderer.cpp
Sprite.cpp
Utils.cpp
json-parser/json.c
)

add_executable(${PROJECT_NAME} ${SOURCES})
configure_file(PTS75F.ttf PTS75F.ttf COPYONLY)
configure_file(spriteAnimations.json spriteAnimations.json COPYONLY)
configure_file(sprite.png sprite.png COPYONLY)
configure_file(MegaMan1.png MegaMan1.png COPYONLY)

INCLUDE(FindPkgConfig)
PKG_SEARCH_MODULE(SDL2 REQUIRED sdl2)
PKG_SEARCH_MODULE(SDL2IMAGE REQUIRED SDL2_image)
PKG_SEARCH_MODULE(SDL2TTF REQUIRED SDL2_ttf)
find_package(OpenGL REQUIRED)

INCLUDE_DIRECTORIES(${SDL2_INCLUDE_DIRS} ${SDL2IMAGE_INCLUDE_DIRS} ${SDL2TTF_INCLUDE_DIRS} ${OPENGL_INCLUDE_DIRS})
TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${SDL2_LIBRARIES} ${SDL2IMAGE_LIBRARIES} ${SDL2TTF_LIBRARIES} ${OPENGL_LIBRARIES})
