#include "game.h"
#include "malloc_macros.c"
#include <SDL.h>
#include <stdlib.h>

#include <chipmunk.h>

bool is_player_hit(BSJ_Game *game) {
  cpBB player_box;
  player_box.l = game->player_x;
  player_box.r = game->player_x + SPRITE_SIZE / 2.0;
  player_box.t = game->player_y;
  player_box.b = game->player_y - SPRITE_SIZE / 2.0;

  // check if the player is touching the boss
  cpBB boss_box;
  boss_box.l = game->boss_x;
  boss_box.r = game->boss_x + SPRITE_SIZE / 2.0;
  boss_box.t = game->boss_y;
  boss_box.b = game->boss_y - SPRITE_SIZE / 2.0;

  if ((bool)cpBBIntersects(player_box, boss_box)) { return true; };

  // check if the player is touching the projectiles
  for (int i = 0; i < game->boss_projectile_count; i++) {
    BSJ_Projectile * projectile = game->boss_projectiles[i];
    if (!projectile->unused) {
      cpBB projectile_box;
      projectile_box.l = projectile->x;
      projectile_box.r = projectile->x + projectile->w;
      projectile_box.t = projectile->y;
      projectile_box.b = projectile->y - projectile->h;
      if ((bool)cpBBIntersects(player_box, projectile_box)) { return true; };
    }
  }


  return false;
}

// Initialization of the game.
void game_new(BSJ_Game *game) {
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
  game->horizontal_acceleration = 1.2;
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

  // allocate a pool of projectiles based on the size of `boss_projectile_count`
  game->max_boss_attack_cooldown = 1 * 60;
  game->boss_attack_cooldown = game->max_boss_attack_cooldown;
  game->boss_projectile_count = 10;
  game->boss_projectiles = MALLOCSA(BSJ_Projectile, game->boss_projectile_count);
  for (int i = 0; i < game->boss_projectile_count; i++) {
    // empty projectile
    game->boss_projectiles[i] = MALLOCA(BSJ_Projectile);
    game->boss_projectiles[i]->unused = true;
  }

  game->is_player_charging = false;
  game->current_charging_frames = 0;
  game->max_charging_frames = 240;
}

// Game logic to move a player left.
void game_move_player_left(BSJ_Game *game)
{
  if (game->is_player_charging)
    game->horizontal_velocity = 0;
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
  if (game->is_player_charging)
    game->horizontal_velocity = 0;
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
  if (game->is_player_charging)
    game->vertical_velocity = 0;
  // only jump on the ground or with double jump
  else if (game->player_y <= game->floor || game->double_jump) {
    if (game->player_y > game->floor)
      game->double_jump = false;
    else
      game->double_jump = true;
    game->vertical_velocity = game->jump_power;
    game->jump_hold_frames = game->max_jump_hold_frames;
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

void game_tick_attack_inputs(BSJ_Game *game)
{
  if (game->is_player_attacking) { game->current_player_attack_frames -= 1; }

  if (game->current_player_attack_frames <= 0) {
    game->current_player_attack_frames = 0;
    game->is_player_attacking = false;
    game->can_player_attack = false;
  }
}

void game_tick_move_inputs(BSJ_Game *game)
{
  if (game->buttons[B_LEFT]) { game_move_player_left(game); }
  if (game->buttons[B_RIGHT]) { game_move_player_right(game); }
  if (game->buttons[B_JUMP] == BS_PRESS) { game_player_jump(game); }
  if (game->buttons[B_ATTACK] == BS_PRESS || game->buttons[B_ATTACK] == BS_HOLD) {
    game_player_attempt_charge(game);
    game_player_attempt_attack(game);
  } else {
    game_player_clear_charge(game);
  }
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

  int unused_projectile_index = game_index_of_unused_projectile(game);
  if (unused_projectile_index == -1) { return; }

  BSJ_Projectile * projectile = game->boss_projectiles[unused_projectile_index];
  projectile->unused = false;
  projectile->x = game->boss_x - SPRITE_SIZE / 4;
  projectile->y = game->boss_y;
  projectile->w = 7;
  projectile->h = 1;
  projectile->speed = 1; // one pixel per frame
  projectile->angle = 3.14;

  game->boss_attack_cooldown = game->max_boss_attack_cooldown;
}

void game_tick_boss_projectiles(BSJ_Game *game)
{
  for (int i = 0; i < game->boss_projectile_count; i++) {
    BSJ_Projectile * projectile = game->boss_projectiles[i];
    if (!projectile->unused) { projectile->x -= projectile->speed; }

    if (projectile->x < -100) { projectile->unused = true; }
  }
}

void game_reset(BSJ_Game *game) {
  game->player_x = 0;
  game->player_y = 800;

  for (int i = 0; i < game->boss_projectile_count; i++) {
    BSJ_Projectile * projectile = game->boss_projectiles[i];
    projectile->unused = true;
  }
}

// This will contain code to control the game.
void game_tick(BSJ_Game *game)
{
  if(is_player_hit(game)) {
    game_reset(game);
  }

  game_tick_attack_inputs(game);
  game_tick_move_inputs(game);
  game_tick_edge_collision(game);
  game_tick_horizontal_velocity(game);
  game_tick_boss(game);
  game_tick_boss_projectiles(game);

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
