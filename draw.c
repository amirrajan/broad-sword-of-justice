#include "sdl_helpers.h"
#include "sprite.h"
#include "draw.h"
#include <SDL.h>
#include <SDL_render.h>
#include <SDL_image.h>

void draw_level_bg(SDL_Context *context, BSJ_Sprites *sprites, BSJ_Game *game)
{
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
}

void draw_level_fg(SDL_Context *context, BSJ_Sprites *sprites, BSJ_Game *game)
{
  game_draw_sprite_or_reset(context,
    sprites->bg_2_foreground,
    game->level == 2,
    sprites->bg_2_foreground->w / 2,
    sprites->bg_2_foreground->h / 2 - FLOOR_OFFSET,
    sprites->bg_2_foreground->w,
    sprites->bg_2_foreground->h,
    false);
}

void draw_player(SDL_Context *context, BSJ_Sprites *sprites, BSJ_Game *game)
{
  if (game->is_player_blocking) {
    game_draw_sprite_or_reset(context,
      sprites->player_block,
      true,
      game->player_x,
      game->player_y,
      sprites->player_block->w,
      sprites->player_block->h,
      game->player_facing);
  }
  else if (game->is_player_charging && !game->is_player_attacking) {
    game_draw_sprite_or_reset(context,
      sprites->player_charge,
      true,
      game->player_x,
      game->player_y,
      sprites->player_charge->w,
      sprites->player_charge->h,
      game->player_facing);
  }
  else if (game->is_player_attacking) {
    game_draw_sprite_or_reset(context,
      sprites->player_attack,
      true,
      game->player_x,
      game->player_y,
      sprites->player_attack->w,
      sprites->player_attack->h,
      game->player_facing);
  }
  else if (game->player_y > SPRITE_SIZE / 2)
  {
    game_draw_sprite_or_reset(context,
      sprites->player_jump,
      true,
      game->player_x,
      game->player_y,
      sprites->player_jump->w,
      sprites->player_jump->h,
      game->player_facing);
  }
  else if (abs(game->horizontal_velocity) > 0)
  {
    game_draw_sprite_or_reset(context,
      sprites->player_walk,
      true,
      game->player_x,
      game->player_y,
      sprites->player_walk->w,
      sprites->player_walk->h,
      game->player_facing);
  }
  else
  {
    game_draw_sprite_or_reset(context,
      sprites->player_idle,
      true,
      game->player_x,
      game->player_y,
      sprites->player_idle->w,
      sprites->player_idle->h,
      game->player_facing);
  }
}

draw_boss(SDL_Context *context, BSJ_Sprites *sprites, BSJ_Game *game)
{
  game_draw_sprite_or_reset(context,
    sprites->boss_idle,
    true,
    game->boss_x,
    game->boss_y,
    sprites->boss_idle->w,
    sprites->boss_idle->h,
    game->boss_facing);

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
}

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

void draw_flash(SDL_Context *context, BSJ_Sprites *sprites, BSJ_Game *game)
{
  // copy pasta!!!!
  if (game->frame_count >= 7.1 * 60 && game->frame_count <= 7.2 * 60) {
    game_draw_sprite_or_reset(context,
			      sprites->flash,
			      true,
			      sprites->bg_1->w / 2,
			      sprites->bg_1->h / 2 - FLOOR_OFFSET,
			      sprites->bg_1->w,
			      sprites->bg_1->h,
			      false);
  }

  // copy pasta!!!!
  if (game->frame_count >= 11.7 * 60 && game->frame_count <= 11.9 * 60) {
    game_draw_sprite_or_reset(context,
			      sprites->flash,
			      true,
			      sprites->bg_1->w / 2,
			      sprites->bg_1->h / 2 - FLOOR_OFFSET,
			      sprites->bg_1->w,
			      sprites->bg_1->h,
			      false);
  }

  // copy pasta!!!!
  if (game->frame_count >= 14.7 * 60 && game->frame_count <= 14.9 * 60) {
    game_draw_sprite_or_reset(context,
			      sprites->flash,
			      true,
			      sprites->bg_1->w / 2,
			      sprites->bg_1->h / 2 - FLOOR_OFFSET,
			      sprites->bg_1->w,
			      sprites->bg_1->h,
			      false);
  }
}

void draw_player_statue(SDL_Context *context, BSJ_Sprites *sprites, BSJ_Game *game)
{
    game_draw_sprite_or_reset(context,
      sprites->player_stone,
      true,
      384 / 2,
      0,
      sprites->player_stone->w,
      sprites->player_stone->h,
      1);
}

void draw_intro_scene(SDL_Context *context, BSJ_Sprites *sprites, BSJ_Game *game)
{
  if (game->scene != S_INTRO) { return; }

  draw_player_statue(context, sprites, game);
  draw_flash(context, sprites, game);
}

void draw_boss_scene(SDL_Context *context, BSJ_Sprites *sprites, BSJ_Game *game) {
  if (game->scene != S_BOSS_1) { return; }

  draw_level_bg(context, sprites, game);
  draw_player(context, sprites, game);
  draw_boss(context, sprites, game);
  draw_level_fg(context, sprites, game);
}

// This takes a game and renders it on the screen.
void game_draw(SDL_Context *context, BSJ_Sprites *sprites, BSJ_Game *game)
{
  // draw the game on the a texture for resizing
  SDL_SetRenderTarget(context->renderer, context->resizeTarget);
  SDL_RenderClear(context->renderer);

  draw_intro_scene(context, sprites, game);
  draw_boss_scene(context, sprites, game);

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
