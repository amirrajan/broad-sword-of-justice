#ifndef game_h
#define game_h

#include <stdbool.h>
#include "sound.h"

#define TIME_PER_TICK 16

#define SPRITE_SIZE  64
#define WORLD_LEFT   SPRITE_SIZE / 2
#define WORLD_RIGHT  1024 - SPRITE_SIZE / 2
#define WORLD_HEIGHT 768
#define WORLD_WIDTH  1024 - WORLD_LEFT - WORLD_RIGHT

typedef enum {
  BS_NONE,
  BS_PRESS,
  BS_HOLD,
  BS_RELEASE
} buttonstate;

typedef enum {
  B_UP,
  B_DOWN,
  B_LEFT,
  B_RIGHT,
  B_JUMP,
  B_ATTACK,
  B_BLOCK,
  B_EXIT,
  NUMBEROFBUTTONS
} buttons;

typedef struct {
  double x;
  double y;
  double w;
  double h;
  double speed;
  double angle;
  bool unused;
} BSJ_Projectile;

// Headless representation of the game.
typedef struct {
  BSJ_Sounds *sounds;
  double timestep;
  double floor;
  double player_x;
  double player_y;
  double player_facing;
  double boss_x;
  double boss_y;
  double boss_facing;
  int boss_projectile_count;
  BSJ_Projectile ** boss_projectiles;
  int boss_attack_cooldown;
  int max_boss_attack_cooldown;
  double horizontal_velocity;
  double vertical_velocity;
  double jump_power;
  double max_jump_hold_frames;
  bool double_jump;
  double gravity;
  double max_horizontal_speed;
  double horizontal_acceleration;
  double friction;
  int jump_hold_frames;
  buttonstate buttons[NUMBEROFBUTTONS];
  double left_edge;
  double right_edge;
  bool is_player_attacking;
  bool is_player_charging;
  int max_player_attack_frames;
  int current_player_attack_frames;
  bool can_player_attack;
  int current_charging_frames;
  int max_charging_frames;
  bool is_player_blocking;
  int max_blocked_hits;
  int current_blocked_hits;
} BSJ_Game;

// Headless representation of a point.
typedef struct {
  int x;
  int y;
} BSJ_Point;

// Helper (put in utility file?) to quickly get the sign.
double sign(double value);

// Initialization of the game.
int game_new(BSJ_Game *game);

// This converts a point from game coordinates to canvas coordinates.
// The current resolution of the game is 1024x768 with sprites sized at 128 pixels.
BSJ_Point location_in_camera(int x, int y);

// This will contain code to control the game.
void game_tick(BSJ_Game *game);

// clean up resources in the game structure
void game_clean_up(BSJ_Game *game);

#endif // game_h
