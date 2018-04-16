#ifdef _WINDOWS
#include "windows.h"
#endif
#include <cstdlib>
#include <stdarg.h>
#include <string>
#include <SDL.h>
#include <SDL_render.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include "malloc_macros.c"
#include "game.c"
#include "sdl_helpers.c"
#include "sprite.c"

// This takes a game and renders it on the screen.
void game_draw(SDL_Context *context, BSJ_Sprites *sprites, BSJ_Game *game)
{
  SDL_RenderClear(context->renderer);
  render_texture(context->renderer,
  		 sprites->player_idle->texture_tuples[0]->texture,
  		 location_in_camera(game->player_x, game->player_y),
  		 0,
  		 game->player_facing == -1 ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE);
  render_texture(context->renderer,
  		 sprites->boss_idle->texture_tuples[0]->texture,
  		 location_in_camera(game->boss_x, game->boss_y),
  		 0,
  		 game->boss_facing == -1 ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE);
  SDL_RenderPresent(context->renderer);
}

// This takes in the current inputs by SDL and maps them to methods to call within the game.
void game_process_inputs(SDL_Event * event, BSJ_Game *game)
{
  // poll for all events
  while (SDL_PollEvent(event)) {
    int type = event->type;

    // we will need to handle other types of events (e.g. window closing) here later
    if (type != SDL_KEYDOWN && type != SDL_KEYUP) { continue; }

    int keySym = event->key.keysym.sym;
    bool keyDown = type == SDL_KEYDOWN;

    if (keySym == SDLK_UP) { game->keys_up = keyDown; }
    else if (keySym == SDLK_DOWN) { game->keys_down = keyDown; }
    else if (keySym == SDLK_LEFT) { game->keys_left = keyDown; }
    else if (keySym == SDLK_RIGHT) { game->keys_right = keyDown; }
    else if (keySym == SDLK_a) { game->keys_a = keyDown; }
    else if (keySym == SDLK_s) { game->keys_b = keyDown; }
    else if (keySym == SDLK_d) { game->keys_c = keyDown; }
    else if (keySym == SDLK_q) { game->keys_q = keyDown; }
  }
}

// Entry point into the application.
#ifndef _WINDOWS
int main(int argc, char *argv[])
#else
  int WINAPI WinMain(HINSTANCE hinstance,
		     HINSTANCE hprevinstance,
		     LPSTR lpcmdline,
		     int ncmdshow)
#endif
{
  // Initialize all the things.
  SDL_Init(SDL_INIT_VIDEO);
  TTF_Init();

  SDL_Context * context = game_new_sdl_context();
  BSJ_Sprites * sprites = game_init_sprites(context);

  MALLOC(BSJ_Game, game);
  game_new(game);

  SDL_RenderSetScale(context->renderer, 1, 1);
  SDL_SetRenderDrawColor(context->renderer, 0, 0, 0, 255);

  // Game loop.
  while (game->keys_q == false) {
    game_tick(game);
    game_process_inputs(context->event, game);
    game_draw(context, sprites, game);
    SDL_Delay(1000. / 60.);
  }

  return 0;
}
