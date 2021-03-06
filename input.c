#include "sdl_helpers.h"
#include "sprite.h"
#include "input.h"
#include <SDL.h>
#include <SDL_render.h>
#include <SDL_image.h>

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
