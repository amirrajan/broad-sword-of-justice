#ifdef _WINDOWS
#include "windows.h"
#endif
#include <cstdlib>
#include <string>
#include <SDL.h>
#include <SDL_render.h>
#include <SDL_image.h>
#include <SDL_ttf.h>

// Headless representation of the game.
typedef struct {
  double timestep;
  double floor;
  double player_x;
  double player_y;
  double horizontal_momentum;
  double vertical_momentum;
  double gravity;
  double max_horizontal_momentum;
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

// Helper method to create a texture from a file.
SDL_Texture * create_texture_from_file(SDL_Renderer * renderer, SDL_Surface * surface, std::string file_name)
{
  surface = IMG_Load(file_name.c_str());
  SDL_SetColorKey(surface, SDL_TRUE, SDL_MapRGB(surface->format, 0x00, 0x40, 0x80) );
  return SDL_CreateTextureFromSurface(renderer, surface);
}

// Helper method to take a sprite, and render it onto the scene at a specific location.
void render_texture(SDL_Renderer * renderer, SDL_Texture * texture, int x, int y)
{
  SDL_Rect destrect;
  destrect.x = x;
  destrect.y = y;
  destrect.w = 128;
  destrect.h = 128;

  SDL_Rect cliprect;
  cliprect.x = 0;
  cliprect.y = 0;
  cliprect.w = 128;
  cliprect.h = 128;

  SDL_RenderCopy(renderer, texture, &cliprect, &destrect);
}

// Helper method to take a sprite, and render it onto the scene at a specific point.
void render_texture(SDL_Renderer * renderer, SDL_Texture * texture, Point point)
{
  render_texture(renderer, texture, point.x, point.y);
}

// Helper (put in utility file?) to quickly get the sign.
double sign(double value)
{
  return (value > 0) - (value < 0);
}

// Initialization of the game.
void game_new(Game *game) {
  game->timestep = 1000. / 60.;
  game->floor = 0;
  game->player_x = 0;
  game->player_y = 0;
  game->horizontal_momentum = 0;
  game->vertical_momentum = 0;
  game->gravity = 1;
  game->max_horizontal_momentum = 20;
  game->jump_hold_frames = 0;
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
  game->horizontal_momentum -= 1.8;
  if(game->horizontal_momentum < game->max_horizontal_momentum * -1) {
    game->horizontal_momentum = game->max_horizontal_momentum * -1;
  }
}

// Game logic to move a player right.
void game_move_player_right(Game *game)
{
  game->horizontal_momentum += 1.8;
  if(game->horizontal_momentum > game->max_horizontal_momentum) {
    game->horizontal_momentum = game->max_horizontal_momentum;
  }
}

// Game logic for player jump.
void game_player_jump(Game *game)
{
  // only jump on the ground
  if (game->player_y <= game->floor) {
    game->vertical_momentum = 10;
    game->jump_hold_frames = 18;
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

// This takes a game and renders it on the screen.
void game_draw(SDL_Renderer *renderer, SDL_Texture *player_idle_textue, Game *game)
{
  SDL_RenderClear(renderer);
  render_texture(renderer, player_idle_textue, location_in_camera(game->player_x, game->player_y));
  SDL_RenderPresent(renderer);
}

// This will contain code to control the game.
void game_tick(Game *game)
{
  // execute current inputs
  if (game->keys_left) { game_move_player_left(game); }
  if (game->keys_right) { game_move_player_right(game); }
  if (game->keys_a) { game_player_jump(game); }

  // momentum
  game->player_x += game->horizontal_momentum;
  game->player_y += game->vertical_momentum;

  // friction (even in air)
  if ((!game->keys_left && !game->keys_right) || (game->keys_left && game->keys_right)) {
    game->horizontal_momentum -= 1.5 * sign(game->horizontal_momentum);
    if (abs(game->horizontal_momentum) < 2) game->horizontal_momentum = 0;
  }

  // keep holding A for higher jump
  if (game->keys_a && game->jump_hold_frames > 0) {
    game->jump_hold_frames--;
    game->vertical_momentum = 10;
  } else {
    game->jump_hold_frames = 0;
  }

  // stop at floor
  if (game->player_y > game->floor) {
    game->vertical_momentum -= 10. / game->timestep;
  } else {
    game->vertical_momentum = 0;
    game->player_y = game->floor;
  }
}

// Entry point into the application.
#ifndef _WINDOWS
int main(int argc, char *argv[])
#else
  int WINAPI WinMain(HINSTANCE hinstance,
		     HINSTANCE hprevinstance,
		     LPSTR lpcmdline,
		     int ncmdshow)
#endif
{
  // Initialize all the things.
  SDL_Init(SDL_INIT_VIDEO);
  TTF_Init();

  SDL_Window * window = SDL_CreateWindow("Game", 50, 50, 1024, 768, SDL_WINDOW_OPENGL);
  SDL_Renderer * renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
  TTF_Font * font = TTF_OpenFont("PTS75F.ttf", 12);
  SDL_Surface * surface = (SDL_Surface *)malloc(sizeof(SDL_Surface));
  SDL_Texture * texture = create_texture_from_file(renderer, surface, "player_idle.png");
  SDL_Event event;

  Game *game = (Game *)malloc(sizeof(Game));
  game_new(game);

  SDL_RenderSetScale(renderer, 1, 1);
  SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);

  // Game loop.
  while (game->keys_q == false) {
    game_tick(game);
    game_process_inputs(event, game);
    game_draw(renderer, texture, game);
    SDL_Delay(1000. / 60.);
    if ((int)game->horizontal_momentum != 0) {
      SDL_Log("%f", game->horizontal_momentum);
    }
  }

  return 0;
}
