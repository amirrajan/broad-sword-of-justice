#include "game.h"
#include "bosses.h"
#include "malloc_macros.c"
#include <stdlib.h>

// here are the arbitrary values you can assign to the metadata properties
typedef enum {
  none = 0,
  straight_projectiles,
  triple_shot_projectiles,
  spray_projectiles
} bossmetadatavalues;

// you get 100 properties in the metadata, god speed
typedef enum {
  mode = 0,
  next_mode_countdown,
  current_countdown_for_next_mode,
  next_mode,
  current_projectiles,
  max_projectiles,
  count,
  current_attack_cooldown,
  max_attack_cooldown,
  previous_attack_cycles,
  attack_cycles,
  time_til_next_attack_cycle
} bossmetadata;

#define md(NAME) game->boss_metadata[NAME]
#define mde(NAME, VALUE) game->boss_metadata[NAME] = VALUE

void game_reset_boss(BSJ_Game *game) {
  for (int i = 0; i < game->boss_projectile_count; i++) {
    BSJ_Projectile * projectile = game->boss_projectiles[i];
    projectile->unused = true;
  }

  game->is_boss_attacking = false;
  mde(current_projectiles, 0);
  mde(max_projectiles, 10);
  mde(mode, none);
  mde(next_mode, straight_projectiles);
  mde(next_mode_countdown, 60 * 3);
  mde(current_countdown_for_next_mode, 0);
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

int find_unused_projectile_index(BSJ_Game *game) {
  for (int i = 0; i < game->boss_projectile_count; i++) {
    if (game->boss_projectiles[i]->unused) { return i; }
  }

  return -1;
}

void game_tick_next_boss_mode(BSJ_Game *game) {
  if (md(current_countdown_for_next_mode) >= md(next_mode_countdown)
      && md(mode) != md(next_mode) && md(next_mode) != none) {
    mde(mode, md(next_mode));
    mde(next_mode, none);
    mde(current_countdown_for_next_mode, 0);
  }

  md(current_countdown_for_next_mode)++;
}

void game_queue_projectile_s_dx_dy(BSJ_Game *game, int speed, double dx, double dy)
{
  int unused_projectile_index = find_unused_projectile_index(game);
  if (unused_projectile_index == -1) { return; }

  BSJ_Projectile * projectile = game->boss_projectiles[unused_projectile_index];
  projectile->unused = false;
  projectile->x = game->boss_x - FLOOR_OFFSET / 4 - 15;
  projectile->y = game->boss_y + 5;
  projectile->w = 7;
  projectile->h = 1;
  projectile->speed = speed; // in pixels
  projectile->dx = dx;
  projectile->dy = dy;

  md(current_projectiles) += 1;
}

void game_queue_projectile_s(BSJ_Game *game, int speed)
{
  game_queue_projectile_s_dx_dy(game, speed, game->boss_facing, 0);
}

// first mode of the boss forces you to understand double jumping
void game_tick_boss_mode_1(BSJ_Game *game) {
  if (md(next_mode) == straight_projectiles) {
    md(attack_cycles) = 0;
    mde(max_attack_cooldown, (1 * 60) / 10);
    mde(current_attack_cooldown, max_attack_cooldown);
  }

  if (md(mode) != straight_projectiles) { return; }

  md(current_attack_cooldown) -= 1;

  if (md(current_attack_cooldown) > 0) { return; }

  game->is_boss_attacking = true;

  sound_play(game->sounds->sound_gunshot);

  game_queue_projectile_s(game, 7);

  mde(current_attack_cooldown, md(max_attack_cooldown));
}

// this boss mode forces you to learn how to block
// the boss will send out bursts of projectiles (which cannot be double jumped)
void game_tick_boss_mode_2(BSJ_Game *game)
{
  // this is the initalization of the next phase
  if (md(next_mode) == triple_shot_projectiles) {
    mde(attack_cycles, 0);
    mde(previous_attack_cycles, 0);
    mde(current_projectiles, 0);
    mde(time_til_next_attack_cycle, 0);
  }

  // don't run this script if the mode isn't this
  if (md(mode) != triple_shot_projectiles) { return; }

  // if the current attack cycle doesn't equal the previous,
  // decrement time till next attack cycle
  if (md(attack_cycles) != md(previous_attack_cycles)) {
    md(time_til_next_attack_cycle)--;
  }

  int projectile_limit = 5;
  int time_between_bursts = 30;

  // the state machine below sends out a barrage of projectile_limit
  // knives every time_between_bursts frames. It forces the player to
  // block at increments (trying to double jump over these would be
  // difficult, so you have to use block).

  if (md(time_til_next_attack_cycle) <= 0 &&
      md(previous_attack_cycles) != md(attack_cycles) &&
      md(current_projectiles) >= projectile_limit) {
    mde(previous_attack_cycles, md(attack_cycles));
    mde(current_projectiles, 0);
  }

  if(md(previous_attack_cycles) != md(attack_cycles)
     && md(current_projectiles) > projectile_limit) {
    md(time_til_next_attack_cycle)--;
  }

  if (md(current_projectiles) >= projectile_limit
      && md(previous_attack_cycles) == md(attack_cycles)) {
    mde(time_til_next_attack_cycle, 45);
    md(attack_cycles)++;
    game->is_boss_attacking = false;
  }

  if (md(current_projectiles) < projectile_limit) {
    md(current_attack_cooldown)--;
  }

  if (md(time_til_next_attack_cycle) > 0) { return; }

  if (md(current_attack_cooldown) > 0) { return; }

  if (md(current_projectiles) >= projectile_limit)  { return; }

  game->is_boss_attacking = true;

  sound_play(game->sounds->sound_gunshot);

  game_queue_projectile_s(game, 7);

  game_queue_projectile_s_dx_dy(game, 10, game->boss_facing, 0.1);

  mde(current_attack_cooldown, md(max_attack_cooldown));
}

void game_tick_boss_determine_next_mode(BSJ_Game *game)
{
  if (md(mode) == straight_projectiles &&
      md(current_projectiles) >= md(max_projectiles)) {
    game->is_boss_attacking = false;
    mde(mode, none);
    mde(next_mode, triple_shot_projectiles);
    mde(next_mode_countdown, 60 * 2);
    mde(current_countdown_for_next_mode, 0);
  }

  if (md(mode) == triple_shot_projectiles &&
      md(attack_cycles) >= 10) {
    game->is_boss_attacking = false;
    mde(mode, none);
    mde(next_mode, straight_projectiles);
    mde(next_mode_countdown, 60 * 6);
    mde(current_countdown_for_next_mode, 0);
  }
}

void game_tick_boss(BSJ_Game *game)
{
  game_tick_boss_mode_1(game);
  game_tick_boss_mode_2(game);
  game_tick_boss_projectiles(game);
  game_tick_boss_determine_next_mode(game);
  game_tick_next_boss_mode(game);
}

void game_tick_boss_projectiles(BSJ_Game *game)
{
  for (int i = 0; i < game->boss_projectile_count; i++) {
    BSJ_Projectile * projectile = game->boss_projectiles[i];

    if (!projectile->unused) {
      projectile->x += projectile->speed * projectile->dx;
      projectile->y += projectile->speed * projectile->dy;
    }

    if (projectile->x < -100) {
      projectile->unused = true;
    }
  }
}
