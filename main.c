// auto-build: fswatch ./*.c | xargs -n1 -I{} sh ./build_mac.sh
//             fswatch ./broad-sword-of-justice | xargs -n1 -I{} ./broad-sword-of-justice
#ifdef _WINDOWS
#include "windows.h"
#define SDL_MAIN_HANDLED
#endif
#include <stdlib.h>
#include <SDL.h>
#include <SDL_render.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include "malloc_macros.c"
#include "helper_functions.h"
#include "game.h"
#include "sdl_helpers.h"
#include "sprite.h"
#include "game_io.h"

#ifdef __cplusplus
  extern "C"
#endif
int main(int argc, char *argv[])
{
  // Initialize all the things.
  SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK);
  TTF_Init();

  SDL_Context * context = game_new_sdl_context();
  BSJ_Sprites * sprites = game_init_sprites(context);

  MALLOC(BSJ_Game, game);
  game_new(game);

  SDL_RenderSetScale(context->renderer, 1, 1);
  SDL_SetRenderDrawColor(context->renderer, 0, 0, 0, 255);

  unsigned int accumulator = 0;
  unsigned int last = SDL_GetTicks();

  // Game loop.
  while (!game->buttons[B_EXIT]) {
    // framerate independant ticks (avoid SDL_Delay)
    unsigned int ticks = SDL_GetTicks();
    accumulator += ticks - last;
    last = ticks;
    if (accumulator > TIME_PER_TICK)
    {
      accumulator -= TIME_PER_TICK;
      game_process_inputs(context, game);
      game_tick(game);
    }

    game_draw(context, sprites, game);
  }

  return 0;
}
