#include <chipmunk.h>

#define SPRITE_SIZE  128
#define WORLD_HEIGHT 768
#define WORLD_WIDTH  1024

// Headless representation of the game.
typedef struct {
  double timestep;
  double floor;
  double player_x;
  double player_y;
  double player_facing;
  double boss_x;
  double boss_y;
  double boss_facing;
  double horizontal_velocity;
  double vertical_velocity;
  double jump_power;
  double max_jump_hold_frames;
  double gravity;
  double max_horizontal_speed;
  double horizontal_acceleration;
  double friction;
  int jump_hold_frames;
  bool keys_up;
  bool keys_down;
  bool keys_left;
  bool keys_right;
  bool keys_a;
  bool keys_b;
  bool keys_c;
  bool keys_exit;
  double left_edge;
  double right_edge;
} BSJ_Game;

// Headless representation of a point.
typedef struct {
  int x;
  int y;
} BSJ_Point;


// Helper (put in utility file?) to quickly get the sign.
double sign(double value)
{
  return (value > 0) - (value < 0);
}

bool is_player_hit(BSJ_Game *game) {
  cpBB player_box;
  player_box.l = game->player_x;
  player_box.r = game->player_x + SPRITE_SIZE / 2.0;
  player_box.t = game->player_y;
  player_box.b = game->player_y - SPRITE_SIZE / 2.0;

  cpBB boss_box;
  boss_box.l = game->boss_x;
  boss_box.r = game->boss_x + SPRITE_SIZE / 2.0;
  boss_box.t = game->boss_y;
  boss_box.b = game->boss_y - SPRITE_SIZE / 2.0;

  return (bool)cpBBIntersects(player_box, boss_box);
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
  game->boss_x = 700;
  game->boss_y = 0;
  game->boss_facing = -1;
  // how quickly the player is moving horizontally (negative value means left, positive value means right).
  game->horizontal_velocity = 0;
  // how quickly the player is moving vertically (the floor is at 0, a negative value here means they are above the floor).
  game->vertical_velocity = 0;
  // the initial power of the jump.
  game->jump_power = 10;
  // how many frames that initial jump power is sustained before gravity takes over.
  game->max_jump_hold_frames = 18;
  // gravity.
  game->gravity = 10;
  // how quickly the player will stop moving horizontally.
  game->friction = 1.5;
  // top speed of player.
  game->max_horizontal_speed = 20;
  // how quickly the player increases to their top speed.
  game->horizontal_acceleration = 1.8;
  // this represents the number of frames that jump has been held for (relates to max_jump_hold_frames).
  game->jump_hold_frames = 0;
  // keys that are currently being held down.
  game->keys_up = false;
  game->keys_down = false;
  game->keys_left = false;
  game->keys_right = false;
  game->keys_a = false;
  game->keys_b = false;
  game->keys_c = false;
  game->keys_exit = false;
  // definition of the edges of the world
  game->left_edge = 0;
  game->right_edge = WORLD_WIDTH - SPRITE_SIZE;
}

// Game logic to move a player left.
void game_move_player_left(BSJ_Game *game)
{
  game->horizontal_velocity -= game->horizontal_acceleration;
  game->player_facing = -1;
  if(game->horizontal_velocity < game->max_horizontal_speed * -1) {
    game->horizontal_velocity = game->max_horizontal_speed * -1;
  }
}

// Game logic to move a player right.
void game_move_player_right(BSJ_Game *game)
{
  game->horizontal_velocity += game->horizontal_acceleration;
  game->player_facing = 1;
  if(game->horizontal_velocity > game->max_horizontal_speed) {
    game->horizontal_velocity = game->max_horizontal_speed;
  }
}

// Game logic for player jump.
void game_player_jump(BSJ_Game *game)
{
  // only jump on the ground
  if (game->player_y <= game->floor) {
    game->vertical_velocity = game->jump_power;
    game->jump_hold_frames = game->max_jump_hold_frames;
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

// This will contain code to control the game.
void game_tick(BSJ_Game *game)
{
  if(is_player_hit(game)) {
    game->player_x = 0;
    game->player_y = 800;
  }

  // execute current inputs
  if (game->keys_left) { game_move_player_left(game); }
  if (game->keys_right) { game_move_player_right(game); }
  if (game->keys_a) { game_player_jump(game); }

  // velocity
  game->player_x += game->horizontal_velocity;
  game->player_y += game->vertical_velocity;

  // edge collision
  if (game->player_x < game->left_edge) {
    game->player_x = game->left_edge;
  }

  if (game->player_x > game->right_edge) {
    game->player_x = game->right_edge;
  }

  // friction (even in air)
  if ((!game->keys_left && !game->keys_right) || (game->keys_left && game->keys_right)) {
    game->horizontal_velocity -= game->friction * sign(game->horizontal_velocity);
    if (abs(game->horizontal_velocity) < 2) game->horizontal_velocity = 0;
  }

  // keep holding A for higher jump
  if (game->keys_a && game->jump_hold_frames > 0) {
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
