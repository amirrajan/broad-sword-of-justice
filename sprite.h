#ifndef sprite_h
#define sprite_h

#include "sdl_helpers.h"

// Tuple representing a texture and duration before drawing the next sprite.
typedef struct {
  int duration;
  SDL_Texture *texture;
} IntOfSDL_Texture;

// Structure that holds the array of sprites that define an animation.
typedef struct {
  IntOfSDL_Texture ** texture_tuples;
  int current_index;
  int current_duration;
  int count;
  int w;
  int h;
} BSJ_Sprite;

// Structure that holds all the sprites in the game.
typedef struct {
  BSJ_Sprite * player_idle;
  BSJ_Sprite * player_walk;
  BSJ_Sprite * player_jump;
  BSJ_Sprite * player_attack;
  BSJ_Sprite * player_charge;
  BSJ_Sprite * player_dash;
  BSJ_Sprite * player_block;
  BSJ_Sprite * boss_idle;
  BSJ_Sprite * boss_projectile;

    BSJ_Sprite * win_bg_1;
  BSJ_Sprite * bg_1;
  BSJ_Sprite * bg_2;
  BSJ_Sprite * bg_2_foreground;
} BSJ_Sprites;

// using game_new_bsj_sprite first parameter is the context second
// parameter is the number of sprites the rest of the parameters are
// filename, frame duration pairs, the pair count MUST match the
// second parameter. I REPEAT, IT MUST MATCH OR HORRIBLE SEG FAULTS
// WILL HAPPEN.
BSJ_Sprite * game_new_sprite(SDL_Context * context,
			     int w,
			     int h,
			     int number_of_textures, ...);

void game_increment_sprite(BSJ_Sprite * sprite);

void game_reset_sprite(BSJ_Sprite * sprite);

// This is the sprite definition for the game. Take note that
// `game_new_bsj_sprite` is a variadic function that requires the
// correct length to be passed in.
BSJ_Sprites * game_init_sprites(SDL_Context * context);

#endif // sprite_h
