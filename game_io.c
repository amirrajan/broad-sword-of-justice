#include "sdl_helpers.h"
#include "sprite.h"
#include "game_io.h"
#include <SDL.h>
#include <SDL_render.h>
#include <SDL_image.h>
#include <SDL_ttf.h>

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
  }


void game_process_keyboard_inputs(SDL_Context *context, BSJ_Game *game)
{
  int type = context->event->type;

  if (type == SDL_KEYDOWN && context->event->key.repeat == 0) {
      int key_sym = context->event->key.keysym.sym;
      if (key_sym == SDLK_UP) { game->buttons[B_UP] = BS_PRESS; }
      else if (key_sym == SDLK_DOWN) { game->buttons[B_DOWN] = BS_PRESS; }
      else if (key_sym == SDLK_LEFT) { game->buttons[B_LEFT] = BS_PRESS; }
      else if (key_sym == SDLK_RIGHT) { game->buttons[B_RIGHT] = BS_PRESS; }
      else if (key_sym == SDLK_a) { game->buttons[B_JUMP] = BS_PRESS; }
      else if (key_sym == SDLK_s) { game->buttons[B_ATTACK] = BS_PRESS; }
      else if (key_sym == SDLK_d) { game->buttons[B_BLOCK] = BS_PRESS; }
      else if (key_sym == SDLK_c) { game->buttons[B_EXIT] = BS_PRESS; }
    } else if (type == SDL_KEYUP && context->event->key.repeat == 0) {
      int key_sym = context->event->key.keysym.sym;
      if (key_sym == SDLK_UP) { game->buttons[B_UP] = BS_RELEASE; }
      else if (key_sym == SDLK_DOWN) { game->buttons[B_DOWN] = BS_RELEASE; }
      else if (key_sym == SDLK_LEFT) { game->buttons[B_LEFT] = BS_RELEASE; }
      else if (key_sym == SDLK_RIGHT) { game->buttons[B_RIGHT] = BS_RELEASE; }
      else if (key_sym == SDLK_a) { game->buttons[B_JUMP] = BS_RELEASE; }
      else if (key_sym == SDLK_s) { game->buttons[B_ATTACK] = BS_RELEASE; }
      else if (key_sym == SDLK_d) { game->buttons[B_BLOCK] = BS_RELEASE; }
      else if (key_sym == SDLK_c) { game->buttons[B_EXIT] = BS_RELEASE; }
    } else if (type == SDL_QUIT) {
      game->buttons[B_EXIT] = BS_PRESS;
    }
}

// This takes in the current inputs by SDL and maps them to methods to call within the game.
void game_process_inputs(SDL_Context *context, BSJ_Game *game)
{
  // advance state of keys
  for (int i = 0; i < NUMBEROFBUTTONS; i++)
  {
    if (game->buttons[i] == BS_PRESS)
      game->buttons[i] = BS_HOLD;
    if (game->buttons[i] == BS_RELEASE)
      game->buttons[i] = BS_NONE;
  }

  // poll for all events
  while (SDL_PollEvent(context->event)) {
    game_process_keyboard_inputs(context, game);
  }
}
