if(WIN32)
	cmake_minimum_required(VERSION 3.7)

	project(broad-sword-of-justice)
	
	set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/CMakeFiles/")
	
	set(SDL2_DIR "C:/Packages/SDL2-2.0.8")
	set(CHIPMUNK_DIR "C:/Packages/chipmunk")
	
	find_package(SDL2 REQUIRED)	
	find_package(SDL2_image REQUIRED)
	find_package(SDL2_ttf REQUIRED)
	find_package(SDL2_mixer REQUIRED)
	include_directories(${SDL2_INCLUDE_DIR} ${SDL2_IMAGE_INCLUDE_DIRS} ${SDL2_TTF_INCLUDE_DIRS} ${SDL2_MIXER_INCLUDE_DIRS})
	
	find_package(OpenGL REQUIRED)
	
	set(CHIPMUNK_INCLUDE_DIRS "C:/Packages/chipmunk/include")
	set(CHIPMUNK_LIBRARIES "C:/Packages/chipmunk/lib/chipmunk.lib")
	#find_package(chipmunk REQUIRED)
	include_directories(${CHIPMUNK_INCLUDE_DIRS})
	
	set(SOURCE
	   ${SOURCE}
	   main.c
	   game.h
	   game.c
	   game_io.c
	   game_io.h
	   helper_functions.c
	   helper_functions.h
	   sprite.h
	   sprite.c
	   sdl_helpers.h
	   sdl_helpers.c
	   sound.h
	   sound.c
	   bosses.h
	   bosses.c
	   malloc_macros.c
	)
	
	add_executable(${PROJECT_NAME} ${SOURCE})
	target_link_libraries(${PROJECT_NAME}
		${SDL2_LIBRARY} ${SDL2_IMAGE_LIBRARIES} ${SDL2_TTF_LIBRARIES} ${SDL2_MIXER_LIBRARIES} ${CHIPMUNK_LIBRARIES})
	
else()
	project(broad-sword-of-justice)

	cmake_minimum_required(VERSION 2.8)

	set(SOURCES main.cpp)

	add_executable(${PROJECT_NAME} ${SOURCES})
	configure_file(PTS75F.ttf PTS75F.ttf COPYONLY)
	configure_file(player_idle.png player_idle.png COPYONLY)
	configure_file(boss_1_idle.png boss_1_idle.png COPYONLY)

	set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake")
	
	INCLUDE(FindPkgConfig)
	PKG_SEARCH_MODULE(SDL2 REQUIRED sdl2)
	PKG_SEARCH_MODULE(SDL2IMAGE REQUIRED SDL2_image)
	PKG_SEARCH_MODULE(SDL2TTF REQUIRED SDL2_ttf)
	find_package(OpenGL REQUIRED)

	INCLUDE_DIRECTORIES(/usr/local/include/chipmunk ${SDL2_INCLUDE_DIRS} ${SDL2IMAGE_INCLUDE_DIRS} ${SDL2TTF_INCLUDE_DIRS} ${OPENGL_INCLUDE_DIRS})
	TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${SDL2_LIBRARIES} ${SDL2IMAGE_LIBRARIES} ${SDL2TTF_LIBRARIES} ${OPENGL_LIBRARIES})
endif()