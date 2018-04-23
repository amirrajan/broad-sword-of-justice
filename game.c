#include "game.h"
#include "bosses.h"
#include "malloc_macros.c"
#include <SDL.h>
#include <stdlib.h>

#include <chipmunk.h>

cpBB game_player_box(BSJ_Game * game)
{
  cpBB player_box;
  player_box.l = game->player_x;
  player_box.r = game->player_x + SPRITE_SIZE / 2.0;
  player_box.t = game->player_y;
  player_box.b = game->player_y - SPRITE_SIZE / 2.0;

  return player_box;
}

bool game_is_player_hit_by_boss(BSJ_Game *game)
{

  // check if the player is touching the boss
  cpBB boss_box;
  boss_box.l = game->boss_x;
  boss_box.r = game->boss_x + SPRITE_SIZE / 2.0;
  boss_box.t = game->boss_y;
  boss_box.b = game->boss_y - SPRITE_SIZE / 2.0;

  return (bool)cpBBIntersects(game_player_box(game), boss_box);
}

BSJ_Projectile * game_is_player_hit_by_projectile(BSJ_Game *game)
{
  cpBB player_box = game_player_box(game);

  for (int i = 0; i < game->boss_projectile_count; i++) {
    BSJ_Projectile * projectile = game->boss_projectiles[i];
    if (!projectile->unused) {
      cpBB projectile_box;
      projectile_box.l = projectile->x;
      projectile_box.r = projectile->x + projectile->w;
      projectile_box.t = projectile->y;
      projectile_box.b = projectile->y - projectile->h;
      if ((bool)cpBBIntersects(player_box, projectile_box)) { return projectile; };
    }
  }

  return NULL;
}

bool game_is_player_hit(BSJ_Game *game)
{
  return game_is_player_hit_by_boss(game) || game_is_player_hit_by_projectile(game) != NULL;
}

bool game_is_boss_hit(BSJ_Game *game) {
  if (!game->is_player_attacking) return false;

  if (game->current_player_attack_frames > 2) return false;

  if (game->player_facing == 1 &&
      (game->boss_x - game->player_x) < 100 &&
      (game->boss_x - game->player_x) > 0) {
    return true;
  }

  if (game->player_facing == -1 &&
      (game->player_x - game->boss_x) < 100 &&
      (game->player_x - game->boss_x) > 0) {
    return true;
  }

  return false;
}

// Initialization of the game.
int game_new(BSJ_Game *game) {
  // load sounds
  game->sounds = MALLOCA(BSJ_Sounds);
  if (sound_init(game->sounds) != 0)
    return 1;
  music_play(game->sounds->music_boss_alley);

  // frames per second.
  game->timestep = 1000. / 60.;
  // where the floor is located.
  game->floor = 0;
  // current position of player.
  game->player_x = 0;
  game->player_y = 800;
  game->player_facing = 1;
  game->boss_x = WORLD_RIGHT;
  game->boss_y = 0;
  game->boss_facing = -1;
  // how quickly the player is moving horizontally (negative value means left, positive value means right).
  game->horizontal_velocity = 0;
  // how quickly the player is moving vertically (the floor is at 0, a negative value here means they are above the floor).
  game->vertical_velocity = 0;
  // the initial power of the jump.
  game->jump_power = 10;
  // how many frames that initial jump power is sustained before gravity takes over.
  game->max_jump_hold_frames = 12;
  // if double-jump is available
  game->double_jump = false;
  // gravity.
  game->gravity = 10;
  // how quickly the player will stop moving horizontally.
  game->friction = 1.5;
  // top speed of player.
  game->max_horizontal_speed = 10;
  // how quickly the player increases to their top speed.
  game->horizontal_acceleration = 0.5;
  // this represents the number of frames that jump has been held for (relates to max_jump_hold_frames).
  game->jump_hold_frames = 0;
  // keys that are currently being held down.
  for (int i = 0; i < NUMBEROFBUTTONS; i++)
    game->buttons[i] = BS_NONE;
  // definition of the edges of the world
  game->left_edge = WORLD_LEFT;
  game->right_edge = WORLD_RIGHT;
  // attack frame definitions
  game->is_player_attacking = false;
  // number of frames the attack lasts
  game->max_player_attack_frames = 18;
  // current attack frame (when this hit's the max, the player is no longer attacking)
  game->current_player_attack_frames = 0;

  // charging specifications
  game->is_player_charging = false;
  game->current_charging_frames = 0;
  game->max_charging_frames = 240;

  // blocking specifications
  game->is_player_blocking = false;
  game->max_blocked_hits = 3;
  game->current_blocked_hits = 0;

  game_init_boss(game);

  return 0;  
}

bool game_can_player_move(BSJ_Game *game)
{
  return !game->is_player_charging &&
         !game->is_player_blocking &&
         !game->is_player_attacking;
}

// Game logic to move a player left.
void game_move_player_left(BSJ_Game *game)
{
  if (!game_can_player_move(game))
    game->horizontal_velocity *= 0.9;
  else if (game->horizontal_velocity > 0)
    game->horizontal_velocity -= 2 * game->horizontal_acceleration;
  else
    game->horizontal_velocity -= game->horizontal_acceleration;

  game->player_facing = -1;

  if(game->horizontal_velocity < game->max_horizontal_speed * -1) {
    game->horizontal_velocity = game->max_horizontal_speed * -1;
  }
}

// Game logic to move a player right.
void game_move_player_right(BSJ_Game *game)
{
  if (!game_can_player_move(game))
    game->horizontal_velocity *= 0.9;
  else if (game->horizontal_velocity < 0)
    game->horizontal_velocity += 2 * game->horizontal_acceleration;
  else
    game->horizontal_velocity += game->horizontal_acceleration;
  game->player_facing = 1;
  if(game->horizontal_velocity > game->max_horizontal_speed) {
    game->horizontal_velocity = game->max_horizontal_speed;
  }
}

// Game logic for player jump.
void game_player_jump(BSJ_Game *game)
{
  if (!game_can_player_move(game))
    game->vertical_velocity = 0;
  // only jump on the ground or with double jump
  else if (game->player_y <= game->floor || game->double_jump) {
    if (game->player_y > game->floor)
      game->double_jump = false;
    else
      game->double_jump = true;
    game->vertical_velocity = game->jump_power;
    game->jump_hold_frames = game->max_jump_hold_frames;

    sound_play(game->sounds->sound_jump);
  }
}

void game_player_attempt_attack(BSJ_Game *game) {
  if (!game->can_player_attack) { return; }
  if (game->is_player_attacking) { return; }
  game->is_player_attacking = true;
  game->current_player_attack_frames = game->max_player_attack_frames;
}

void game_player_clear_charge(BSJ_Game *game) {
  game->is_player_charging = false;
  game->current_charging_frames = 0;
  game->can_player_attack = false;
}

void game_player_attempt_charge(BSJ_Game *game) {
  game->is_player_charging = true;
  game->current_charging_frames++;
  if (game->current_charging_frames > game->max_charging_frames) {
    game->can_player_attack = true;
    game->is_player_charging = false;
    game->current_charging_frames = 0;
  }
}

// This converts a point from game coordinates to canvas coordinates.
// The current resolution of the game is 1024x768 with sprites sized at 128 pixels.
BSJ_Point location_in_camera(int x, int y)
{
  BSJ_Point result;
  result.x = x;
  result.y = WORLD_HEIGHT - SPRITE_SIZE - y;
  return result;
}

void game_tick_horizontal_velocity(BSJ_Game *game) {
  game->player_x += game->horizontal_velocity;
  game->player_y += game->vertical_velocity;

  if ((!game->buttons[B_LEFT] && !game->buttons[B_RIGHT]) || (game->buttons[B_LEFT] && game->buttons[B_RIGHT])) {
    game->horizontal_velocity -= game->friction * sign(game->horizontal_velocity);
    if (fabs(game->horizontal_velocity) < 2) game->horizontal_velocity = 0;
  }
}

void game_tick_edge_collision(BSJ_Game *game)
{
  if (game->player_x < game->left_edge) { game->player_x = game->left_edge; }
  if (game->player_x > game->right_edge) { game->player_x = game->right_edge; }
}

void game_tick_attack(BSJ_Game *game)
{
  if (game->is_player_attacking) { game->current_player_attack_frames -= 1; }

  if (game->current_player_attack_frames <= 0) {
    game->current_player_attack_frames = 0;
    game->is_player_attacking = false;
    game->can_player_attack = false;
  }
}

void game_player_attempt_block(BSJ_Game *game)
{
  game->is_player_blocking = true;
}

void game_player_clear_block(BSJ_Game *game)
{
  game->is_player_blocking = false;
  game->current_blocked_hits = 0;
}

void game_tick_buttons(BSJ_Game *game)
{
  if (game->buttons[B_LEFT]) { game_move_player_left(game); }
  if (game->buttons[B_RIGHT]) { game_move_player_right(game); }
  if (game->buttons[B_JUMP] == BS_PRESS) { game_player_jump(game); }
  if (game->buttons[B_ATTACK]) {
    game_player_attempt_charge(game);
    game_player_attempt_attack(game);
  } else {
    game_player_clear_charge(game);
  }

  if (game->buttons[B_BLOCK]) {
    game_player_attempt_block(game);
  } else {
    game_player_clear_block(game);
  }
}

void game_reset(BSJ_Game *game) {
  game->player_x = 0;
  game->player_y = 800;
  game_reset_boss(game);
}

void game_process_blocks(BSJ_Game *game)
{
  if (!game->is_player_blocking) return;
  if (game->current_blocked_hits >= game->max_blocked_hits) return;

  BSJ_Projectile * projectile = game_is_player_hit_by_projectile(game);

  if (projectile == NULL) return;

  projectile->unused = true;
  game->current_blocked_hits++;
}

void game_tick_vertical_velocity(BSJ_Game *game)
{
  // keep holding A for higher jump
  if (game->buttons[B_JUMP] && game->jump_hold_frames > 0) {
    game->jump_hold_frames--;
    game->vertical_velocity = game->jump_power;
  } else {
    game->jump_hold_frames = 0;
  }

  // stop at floor
  if (game->player_y > game->floor) {
    game->vertical_velocity -= game->gravity / game->timestep;
  } else {
    game->vertical_velocity = 0;
    game->player_y = game->floor;
  }
}

// This will contain code to control the game.
void game_tick(BSJ_Game *game)
{
  game_process_blocks(game);

  if(game_is_player_hit(game)) {
    sound_play(game->sounds->sound_hurt0);
    game_reset(game);
  }

  if(game_is_boss_hit(game)) {
    game_reset(game);
  }

  game_tick_buttons(game);
  game_tick_attack(game);
  game_tick_edge_collision(game);
  game_tick_horizontal_velocity(game);
  game_tick_vertical_velocity(game);
  game_tick_boss(game);
}

void game_clean_up(BSJ_Game *game) {
  sound_clean_up(game->sounds);
}