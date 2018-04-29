#include "sdl_helpers.h"

#include "game.h"
#include "sprite.h"
#include "malloc_macros.c"

SDL_Context * game_new_sdl_context()
{
  MALLOC(SDL_Context, o);

  o->window = SDL_CreateWindow("Broadsword of Justice",
    SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
    GAME_WIDTH * 2, GAME_HEIGHT * 2,
    SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_FULLSCREEN_DESKTOP);

  o->renderer = SDL_CreateRenderer(o->window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
  o->resizeTarget = SDL_CreateTexture(o->renderer, SDL_PIXELFORMAT_RGBA8888,
    SDL_TEXTUREACCESS_TARGET, GAME_WIDTH, GAME_HEIGHT);
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
		    SDL_RendererFlip flip,
		    double offset_x,
		    double offset_y)
{
  SDL_Rect destrect;
  destrect.x = (point.x + offset_x) - w/2;
  destrect.y = (point.y + offset_y) - h/2;
  destrect.w = w;
  destrect.h = h;

  SDL_Rect cliprect;
  cliprect.x = 0;
  cliprect.y = 0;
  cliprect.w = w;
  cliprect.h = h;

  SDL_RenderCopyEx(renderer, texture, &cliprect, &destrect, angle, NULL, flip);
}
