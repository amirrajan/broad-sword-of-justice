#include "sdl_helpers.h"

#include "game.h"
#include "sprite.h"
#include "malloc_macros.c"

SDL_Context * game_new_sdl_context()
{
  MALLOC(SDL_Context, o);
  o->window = SDL_CreateWindow("Game", 50, 50, 1024, 768, SDL_WINDOW_OPENGL);
  o->renderer = SDL_CreateRenderer(o->window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
  o->surface = MALLOCA(SDL_Surface);
  o->event = MALLOCA(SDL_Event);
  if (SDL_NumJoysticks() > 0) { o->joystick = SDL_JoystickOpen(0); }
  return o;
}

// Helper method to create a texture from a file.
SDL_Texture * create_texture_from_file(SDL_Renderer * renderer, SDL_Surface * surface, char* file_name)
{
  surface = IMG_Load(file_name);
  SDL_SetColorKey(surface, SDL_TRUE, SDL_MapRGB(surface->format, 0x00, 0x40, 0x80) );
  return SDL_CreateTextureFromSurface(renderer, surface);
}

// Helper method to take a sprite, and render it onto the scene at a specific location.
void render_texture(SDL_Renderer * renderer,
		    SDL_Texture * texture,
		    BSJ_Point point,
		    int w,
		    int h,
		    double angle,
		    SDL_RendererFlip flip)
{
  SDL_Rect destrect;
  destrect.x = point.x - w/2;
  destrect.y = point.y - h/2;
  destrect.w = w;
  destrect.h = h;

  SDL_Rect cliprect;
  cliprect.x = 0;
  cliprect.y = 0;
  cliprect.w = w;
  cliprect.h = h;

  SDL_RenderCopyEx(renderer, texture, &cliprect, &destrect, angle, NULL, flip);
}
