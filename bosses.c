#include "game.h"
#include "bosses.h"
#include "malloc_macros.c"
#include <stdlib.h>

typedef enum {
  none = 0,
  straight_projectiles,
  triple_shot_projectiles,
  spray_projectiles
} bossmetadatavalues;

// you get 100 properties in the metadata, god speed
typedef enum {
  mode = 0,
  current_projectiles,
  max_projectiles,
  count,
  attack_cooldown,
  max_attack_cooldown
} bossmetadata;

#define md(NAME) game->boss_metadata[NAME]

void game_reset_boss(BSJ_Game *game) {
  for (int i = 0; i < game->boss_projectile_count; i++) {
    BSJ_Projectile * projectile = game->boss_projectiles[i];
    projectile->unused = true;
  }

  md(current_projectiles) = 0;
  md(max_projectiles) = 20;
  md(mode) = straight_projectiles;
  md(max_attack_cooldown) = (1 * 60) / 10;
  md(attack_cooldown) = md(max_attack_cooldown);
}

void game_init_boss(BSJ_Game *game) {
  // allocate a pool of projectiles based on the size of `boss_projectile_count`
  game->boss_projectile_count = 100;
  game->boss_projectiles = MALLOCSA(BSJ_Projectile, game->boss_projectile_count);
  for (int i = 0; i < game->boss_projectile_count; i++) {
    game->boss_projectiles[i] = MALLOCA(BSJ_Projectile);
  }
  game_reset_boss(game);
}

int game_index_of_unused_projectile(BSJ_Game *game) {
  for (int i = 0; i < game->boss_projectile_count; i++) {
    if (game->boss_projectiles[i]->unused) { return i; }
  }

  return -1;
}

void game_tick_boss_mode_1(BSJ_Game *game) {
  if (md(current_projectiles) >= md(max_projectiles) &&
      md(mode) == straight_projectiles) {
    md(mode) = triple_shot_projectiles;
  }

  if (md(mode) != straight_projectiles) { return; }

  md(attack_cooldown) -= 1;
  if (md(attack_cooldown) > 0) { return; }

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

  md(current_projectiles) += 1;

  md(attack_cooldown) = md(max_attack_cooldown);
}

void game_tick_boss_mode_2(BSJ_Game *game) {

}

void game_tick_boss(BSJ_Game *game)
{
  game_tick_boss_mode_1(game);
  game_tick_boss_mode_2(game);

  game_tick_boss_projectiles(game);
}

void game_tick_boss_projectiles(BSJ_Game *game)
{
  for (int i = 0; i < game->boss_projectile_count; i++) {
    BSJ_Projectile * projectile = game->boss_projectiles[i];
    if (!projectile->unused) { projectile->x -= projectile->speed; }
    if (projectile->x < -100) {
      projectile->unused = true;
    }
  }
}
