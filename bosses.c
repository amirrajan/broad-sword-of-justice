#include "game.h"
#include "bosses.h"
#include "malloc_macros.c"
#include <stdlib.h>

void game_init_boss(BSJ_Game *game) {
  // allocate a pool of projectiles based on the size of `boss_projectile_count`
  game->max_boss_attack_cooldown = (1 * 60) / 4;
  game->boss_attack_cooldown = game->max_boss_attack_cooldown;
  game->boss_projectile_count = 100;
  game->boss_projectiles = MALLOCSA(BSJ_Projectile, game->boss_projectile_count);
  for (int i = 0; i < game->boss_projectile_count; i++) {
    // empty projectile
    game->boss_projectiles[i] = MALLOCA(BSJ_Projectile);
    game->boss_projectiles[i]->unused = true;
  }
  game->current_projectiles_for_boss = 0;
  game->max_projectiles_for_boss = 10;
}

int game_index_of_unused_projectile(BSJ_Game *game) {
  for (int i = 0; i < game->boss_projectile_count; i++) {
    if (game->boss_projectiles[i]->unused) { return i; }
  }

  return -1;
}

void game_tick_boss(BSJ_Game *game)
{
  // knife throwing boss
  game->boss_attack_cooldown -= 1;
  if (game->boss_attack_cooldown > 0) { return; }
  if (game->current_projectiles_for_boss >= game->max_projectiles_for_boss) {
    return;
  }

  int unused_projectile_index = game_index_of_unused_projectile(game);
  if (unused_projectile_index == -1) { return; }

  BSJ_Projectile * projectile = game->boss_projectiles[unused_projectile_index];
  projectile->unused = false;
  projectile->x = game->boss_x - SPRITE_SIZE / 4;
  projectile->y = game->boss_y;
  projectile->w = 7;
  projectile->h = 1;
  projectile->speed = 10; // one pixel per frame
  projectile->angle = 3.14;

  game->boss_attack_cooldown = game->max_boss_attack_cooldown;
  game->current_projectiles_for_boss++;
}

void game_tick_boss_projectiles(BSJ_Game *game)
{
  for (int i = 0; i < game->boss_projectile_count; i++) {
    BSJ_Projectile * projectile = game->boss_projectiles[i];
    if (!projectile->unused) { projectile->x -= projectile->speed; }
    if (projectile->x < -100) {
      projectile->unused = true;
      game->current_projectiles_for_boss--;
      if (game->current_projectiles_for_boss < 0) {
	game->current_projectiles_for_boss = 0;
      }
    }
  }
}
