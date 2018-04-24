#include "sprite.h"

#include "game.h"
#include "sdl_helpers.h"
#include "malloc_macros.c"
#include <stdarg.h>

// using game_new_bsj_sprite first parameter is the context second
// parameter is the number of sprites the rest of the parameters are
// filename, frame duration pairs, the pair count MUST match the
// second parameter. I REPEAT, IT MUST MATCH OR HORRIBLE SEG FAULTS
// WILL HAPPEN.
BSJ_Sprite * game_new_sprite(SDL_Context * context,
			     int w,
			     int h,
			     int number_of_textures, ...)
{
  MALLOC(BSJ_Sprite, sprite);
  sprite->texture_tuples = MALLOCSA(IntOfSDL_Texture, number_of_textures);
  sprite->current_index = 0;
  sprite->current_duration = 0;
  sprite->count = number_of_textures;
  sprite->w = w;
  sprite->h = h;

  for (int index = 0; index < number_of_textures; index++) {
    sprite->texture_tuples[index] = MALLOCA(IntOfSDL_Texture);
  }

  va_list file_duration_pairs;
  va_start(file_duration_pairs, number_of_textures);
  int texture_index = 0;
  for (int index = 0; index < number_of_textures * 2; index++) {
    if(index % 2 == 0) {
      char * file_path = va_arg(file_duration_pairs, char*);
      context->surface = IMG_Load(file_path);
      SDL_SetColorKey(context->surface, SDL_TRUE, SDL_MapRGB(context->surface->format, 0x00, 0x40, 0x80));
      sprite->texture_tuples[texture_index]->texture = SDL_CreateTextureFromSurface(context->renderer, context->surface);
      sprite->texture_tuples[texture_index]->texture =
        create_texture_from_file(context->renderer,
		                         context->surface,
		                         file_path);
    } else {
      int duration = va_arg(file_duration_pairs, int);
      sprite->texture_tuples[texture_index]->duration = duration;
      texture_index++;
    }
  }
  va_end(file_duration_pairs);


  return sprite;
}

void game_increment_sprite(BSJ_Sprite * sprite)
{
  sprite->current_duration++;
  if (sprite->current_duration >= sprite->texture_tuples[sprite->current_index]->duration) {
    sprite->current_duration = 0;
    sprite->current_index++;
  }

  if (sprite->current_index >= sprite->count) {
    sprite->current_index = 0;
  }
}

void game_reset_sprite(BSJ_Sprite * sprite)
{
  sprite->current_duration = 0;
  sprite->current_index = 0;
}

// This is the sprite definition for the game. Take note that
// `game_new_bsj_sprite` is a variadic function that requires the
// correct length to be passed in.
BSJ_Sprites * game_init_sprites(SDL_Context * context)
{
  MALLOC(BSJ_Sprites, sprites);

  // using game_new_bsj_sprite first parameter is the context second
  // parameter is the number of sprites the rest of the parameters are
  // filename, frame duration pairs, the pair count MUST match the
  // second parameter. I REPEAT, IT MUST MATCH OR HORRIBLE SEG FAULTS
  // WILL HAPPEN.
  sprites->player_idle =
    game_new_sprite(context, 128, 128,
			1,                    // number of sprites that represent this animation
			"player_idle.png", 1  // filename plus duration
			);

  // the frame durations are closely tied to:
  // game->max_player_attack_frames = 18;
  // if the sum of the durations, don't match up to to the max_player_attack_frames, then things will look weird.
  sprites->player_attack =
    game_new_sprite(context, 128, 128,
			4,                        // number of sprites that represent this animation
		  "player_attack1.png", 3,  // filename plus duration
			"player_attack2.png", 3,  // filename plus duration
			"player_attack3.png", 3,  // filename plus duration
			"player_attack4.png", 51  // filename plus duration
			);

  sprites->player_charge =
    game_new_sprite(context, 128, 128,
			1,                        // number of sprites that represent this animation
		        "player_charge.png", 1   // filename plus duration
			);

  sprites->player_block =
    game_new_sprite(context, 128, 128,
			1,                        // number of sprites that represent this animation
		        "player_block.png", 1   // filename plus duration
			);

  sprites->boss_idle =
    game_new_sprite(context, 128, 128,
			1,
			"enemy2_idle.png", 1
			);

  sprites->boss_projectile =
    game_new_sprite(context, 14, 3,
			1,
			"knife.png", 1
			);

  sprites->bg_1 =
    game_new_sprite(context, 384, 220,
      1,
      "bg_1.png", 1
    );
  sprites->bg_2 =
    game_new_sprite(context, 384, 220,
      1,
      "bg_2.png", 1
    );

  return sprites;
}
