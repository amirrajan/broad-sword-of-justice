#include "sdl_helpers.h"
#include "sprite.h"
#include "game_io.h"
#include <SDL.h>
#include <SDL_render.h>
#include <SDL_image.h>

// Given a sprite, a predicate for reset, x, y, and facing. Draw the sprite on the screen for its given animation step.
// If the `draw_if` predicate is false, reset the sprite.
void game_draw_sprite_or_reset(SDL_Context *context,
			       BSJ_Sprite *sprite,
			       bool draw_if,
			       int x,
			       int y,
			       int w,
			       int h,
			       int facing)
{
  if (draw_if) {
    render_texture(context->renderer,
		   sprite->texture_tuples[sprite->current_index]->texture,
		   location_in_camera(x, y),
		   w,
		   h,
		   0,
		   facing == -1 ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE);
    game_increment_sprite(sprite);
  } else { game_reset_sprite(sprite); }
}

// This takes a game and renders it on the screen.
void game_draw(SDL_Context *context, BSJ_Sprites *sprites, BSJ_Game *game)
{
  // draw the game on the a texture for resizing
  SDL_SetRenderTarget(context->renderer, context->resizeTarget);
  SDL_RenderClear(context->renderer);

  game_draw_sprite_or_reset(context,
          sprites->bg_1,
          game->level == 1,
          sprites->bg_1->w / 2,
          sprites->bg_1->h / 2 - FLOOR_OFFSET,
          sprites->bg_1->w,
          sprites->bg_1->h,
          false);

  game_draw_sprite_or_reset(context,
          sprites->bg_2,
          game->level == 2,
          sprites->bg_2->w / 2,
          sprites->bg_2->h / 2 - FLOOR_OFFSET,
          sprites->bg_2->w,
          sprites->bg_2->h,
          false);

  game_draw_sprite_or_reset(context,
			    sprites->boss_idle,
			    true,
			    game->boss_x,
			    game->boss_y,
			    sprites->boss_idle->w,
			    sprites->boss_idle->h,
			    game->boss_facing);

  game_draw_sprite_or_reset(context,
			    sprites->player_block,
			    game->is_player_blocking,
			    game->player_x,
			    game->player_y,
			    sprites->player_block->w,
			    sprites->player_block->h,
			    game->player_facing);

  game_draw_sprite_or_reset(context,
			    sprites->player_charge,
			    game->is_player_charging && !game->is_player_attacking,
			    game->player_x,
			    game->player_y,
			    sprites->player_charge->w,
			    sprites->player_charge->h,
			    game->player_facing);

  game_draw_sprite_or_reset(context,
			    sprites->player_attack,
			    game->is_player_attacking,
			    game->player_x,
			    game->player_y,
			    sprites->player_attack->w,
			    sprites->player_attack->h,
			    game->player_facing);

  game_draw_sprite_or_reset(context,
			    sprites->player_idle,
			    !game->is_player_attacking && !game->is_player_charging && !game->is_player_blocking,
			    game->player_x,
			    game->player_y,
			    sprites->player_idle->w,
			    sprites->player_idle->h,
			    game->player_facing);

  for (int i = 0; i < game->boss_projectile_count; i++) {
    game_draw_sprite_or_reset(context,
			      sprites->boss_projectile,
			      !game->boss_projectiles[i]->unused,
			      game->boss_projectiles[i]->x,
			      game->boss_projectiles[i]->y,
			      sprites->boss_projectile->w,
			      sprites->boss_projectile->h,
			      game->boss_facing);
  }

  // reset the render target and draw the texture at scale
  SDL_SetRenderTarget(context->renderer, NULL);
  SDL_RenderClear(context->renderer);
  SDL_Rect destrect;
  SDL_RenderGetViewport(context->renderer, &destrect);
  destrect.x = 0;
  destrect.y = 0;
  SDL_RenderCopyEx(context->renderer, context->resizeTarget, NULL, &destrect, 0, NULL, SDL_FLIP_NONE);

  SDL_RenderPresent(context->renderer);
}

// Got tired of using my keyboard, so I hooked up an iBuffalo game pad to do play testing.
void game_process_gamepad_inputs(SDL_Context *context, BSJ_Game *game)
{
  int type = context->event->type;
  if (type == SDL_JOYAXISMOTION) {
    long magnitude_x = SDL_JoystickGetAxis(context->joystick, 0);
    long magnitude_y = SDL_JoystickGetAxis(context->joystick, 1);
    // directional is not being pressed,
    // aside: can't use `abs` on int.min returns a negative number... TIL
    if (magnitude_x < 10000 && magnitude_x > -10000) {
      if (game->buttons[B_LEFT] != BS_NONE) { game->buttons[B_LEFT] = BS_RELEASE; }
      if (game->buttons[B_RIGHT] != BS_NONE) { game->buttons[B_RIGHT] = BS_RELEASE; }
    } else if (sign(magnitude_x) == 1) {
      if (game->buttons[B_LEFT] != BS_NONE) { game->buttons[B_LEFT] = BS_RELEASE; }
      if (game->buttons[B_RIGHT] == BS_NONE) { game->buttons[B_RIGHT] = BS_PRESS; }
    } else if (sign(magnitude_x) == -1) {
      if (game->buttons[B_LEFT] == BS_NONE) { game->buttons[B_LEFT] = BS_PRESS; }
      if (game->buttons[B_RIGHT] != BS_NONE) { game->buttons[B_RIGHT] = BS_RELEASE; }
    }

    // directional is not being pressed,
    // aside: can't use `abs` on int.min returns a negative number... TIL
    if (magnitude_y < 10000 && magnitude_y > -10000) {
      game->buttons[B_UP] = BS_RELEASE;
      game->buttons[B_DOWN] = BS_RELEASE;
    } else if (sign(magnitude_y) == 1) {
      game->buttons[B_UP] = BS_RELEASE;
      game->buttons[B_DOWN] = BS_PRESS;
    } else if (sign(magnitude_y) == -1) {
      game->buttons[B_UP] = BS_PRESS;
      game->buttons[B_DOWN] = BS_RELEASE;
    }
  }

  if (type == SDL_JOYBUTTONDOWN || type == SDL_JOYBUTTONUP) {
    // B
    if (SDL_JoystickGetButton(context->joystick, 1)) {
      if (game->buttons[B_JUMP] == BS_NONE) {
	game->buttons[B_JUMP] = BS_PRESS;
      }
    } else if (game->buttons[B_JUMP] != BS_NONE) {
	game->buttons[B_JUMP] = BS_RELEASE;
    }

    // B
    if (SDL_JoystickGetButton(context->joystick, 0)) {
      if (game->buttons[B_ATTACK] == BS_NONE) {
	game->buttons[B_ATTACK] = BS_PRESS;
      }
    } else if (game->buttons[B_ATTACK] != BS_NONE) {
	game->buttons[B_ATTACK] = BS_RELEASE;
    }

    // R
    if (SDL_JoystickGetButton(context->joystick, 5)) {
      if (game->buttons[B_BLOCK] == BS_NONE) {
	game->buttons[B_BLOCK] = BS_PRESS;
      }
    } else if (game->buttons[B_BLOCK] != BS_NONE) {
	game->buttons[B_BLOCK] = BS_RELEASE;
    }
  }

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
    game_process_gamepad_inputs(context, game);
    game_process_keyboard_inputs(context, game);
  }
}
