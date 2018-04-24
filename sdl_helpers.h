#ifndef SDL_helpers_h
#define SDL_helpers_h

#include <SDL.h>
#include <SDL_render.h>
#include <SDL_image.h>
#include "game.h"

// SDL goop.
typedef struct {
  SDL_Window * window;
  SDL_Renderer * renderer;
  SDL_Texture * resizeTarget;
  SDL_Surface * surface;
  SDL_Event * event;
  SDL_Joystick * joystick;
} SDL_Context;

SDL_Context * game_new_sdl_context();

// Helper method to create a texture from a file.
SDL_Texture * create_texture_from_file(SDL_Renderer * renderer, SDL_Surface * surface, char* file_name);

// Helper method to take a sprite, and render it onto the scene at a specific location.
void render_texture(SDL_Renderer * renderer,
		    SDL_Texture * texture,
		    BSJ_Point point,
		    int w,
		    int h,
		    double angle,
		    SDL_RendererFlip flip);

#endif // SDL_helpers_h
