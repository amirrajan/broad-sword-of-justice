// Headless representation of the game.
typedef struct {
  double timestep;
  double floor;
  double player_x;
  double player_y;
  double boss_x;
  double boss_y;
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
  bool keys_q;
} Game;

// Headless representation of a point.
typedef struct {
  int x;
  int y;
} Point;


// Helper (put in utility file?) to quickly get the sign.
double sign(double value)
{
  return (value > 0) - (value < 0);
}

// Initialization of the game.
void game_new(Game *game) {
  // frames per second.
  game->timestep = 1000. / 60.;
  // where the floor is located.
  game->floor = 0;
  // current position of player.
  game->player_x = 0;
  game->player_y = 0;
  game->boss_x = 700;
  game->boss_y = 0;
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
  game->keys_q = false;
}

// Game logic to move a player left.
void game_move_player_left(Game *game)
{
  game->horizontal_velocity -= game->horizontal_acceleration;
  if(game->horizontal_velocity < game->max_horizontal_speed * -1) {
    game->horizontal_velocity = game->max_horizontal_speed * -1;
  }
}

// Game logic to move a player right.
void game_move_player_right(Game *game)
{
  game->horizontal_velocity += game->horizontal_acceleration;
  if(game->horizontal_velocity > game->max_horizontal_speed) {
    game->horizontal_velocity = game->max_horizontal_speed;
  }
}

// Game logic for player jump.
void game_player_jump(Game *game)
{
  // only jump on the ground
  if (game->player_y <= game->floor) {
    game->vertical_velocity = game->jump_power;
    game->jump_hold_frames = game->max_jump_hold_frames;
  }
}

// This converts a point from game coordinates to canvas coordinates.
// The current resolution of the game is 1024x768 with sprites sized at 128 pixels.
Point location_in_camera(int x, int y)
{
  Point result;
  result.x = x;
  result.y = 768 - 128 - y;
  return result;
}

// This takes in the current inputs by SDL and maps them to methods to call within the game.
void game_process_inputs(SDL_Event event, Game *game)
{
  // poll for all events
  while (SDL_PollEvent(&event)) {
    int type = event.type;

    // we will need to handle other types of events (e.g. window closing) here later
    if (type != SDL_KEYDOWN && type != SDL_KEYUP) { continue; }

    int keySym = event.key.keysym.sym;
    bool keyDown = type == SDL_KEYDOWN;

    if (keySym == SDLK_UP) { game->keys_up = keyDown; }
    else if (keySym == SDLK_DOWN) { game->keys_down = keyDown; }
    else if (keySym == SDLK_LEFT) { game->keys_left = keyDown; }
    else if (keySym == SDLK_RIGHT) { game->keys_right = keyDown; }
    else if (keySym == SDLK_a) { game->keys_a = keyDown; }
    else if (keySym == SDLK_s) { game->keys_b = keyDown; }
    else if (keySym == SDLK_d) { game->keys_c = keyDown; }
    else if (keySym == SDLK_q) { game->keys_q = keyDown; }
  }
}

// This will contain code to control the game.
void game_tick(Game *game)
{
  // execute current inputs
  if (game->keys_left) { game_move_player_left(game); }
  if (game->keys_right) { game_move_player_right(game); }
  if (game->keys_a) { game_player_jump(game); }

  // velocity
  game->player_x += game->horizontal_velocity;
  game->player_y += game->vertical_velocity;

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
