// auto-build: fswatch ./main.cpp ./*.c | xargs -n1 -I{} make
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

// Given a sprite, a predicate for reset, x, y, and facing. Draw the sprite on the screen for its given animation step.
// If the `draw_if` predicate is false, reset the sprite.
void game_draw_sprite_or_reset(SDL_Context *context, BSJ_Sprite *sprite, bool draw_if, int x, int y, int facing)
{
  if (draw_if) {
    render_texture(context->renderer,
		   sprite->texture_tuples[sprite->current_index]->texture,
		   location_in_camera(x, y),
		   0,
		   facing == -1 ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE);
    game_increment_sprite(sprite);
  } else { game_reset_sprite(sprite); }
}

// This takes a game and renders it on the screen.
void game_draw(SDL_Context *context, BSJ_Sprites *sprites, BSJ_Game *game)
{
  SDL_RenderClear(context->renderer);

  game_draw_sprite_or_reset(context,
			    sprites->player_attack,
			    game->is_player_attacking,
			    game->player_x,
			    game->player_y,
			    game->player_facing);

  game_draw_sprite_or_reset(context,
			    sprites->player_idle,
			    !game->is_player_attacking,
			    game->player_x,
			    game->player_y,
			    game->player_facing);

  game_draw_sprite_or_reset(context,
			    sprites->boss_idle,
			    true,
			    game->boss_x,
			    game->boss_y,
			    game->boss_facing);

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

    int key_sym = event->key.keysym.sym;
    bool key_down = type == SDL_KEYDOWN;

    if (key_sym == SDLK_UP) { game->keys_up = key_down; }
    else if (key_sym == SDLK_DOWN) { game->keys_down = key_down; }
    else if (key_sym == SDLK_LEFT) { game->keys_left = key_down; }
    else if (key_sym == SDLK_RIGHT) { game->keys_right = key_down; }
    else if (key_sym == SDLK_a) { game->keys_a = key_down; }
    else if (key_sym == SDLK_s) { game->keys_b = key_down; }
    else if (key_sym == SDLK_d) { game->keys_c = key_down; }
    else if (key_sym == SDLK_c) { game->keys_exit = key_down; }
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
  while (game->keys_exit == false) {
    game_tick(game);
    game_process_inputs(context->event, game);
    game_draw(context, sprites, game);
    SDL_Delay(1000. / 60.);
  }

  return 0;
}
