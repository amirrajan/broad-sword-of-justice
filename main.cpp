#include <cstdlib>
#include <string>
#include <SDL.h>
#include <SDL_render.h>
#include <SDL_image.h>
#include <SDL_ttf.h>

// Headless representation of the game.
typedef struct {
  int floor;
  int player_x;
  int player_y;
} Game;

// Headless representation of a point.
typedef struct {
  int x;
  int y;
} Point;

// Headless representation of keys that can be pressed.
enum Key { LEFT = 0, RIGHT, ESC, NUMBER_OF_KEYS };

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

// Initialization of the game.
void game_new(Game *game) {
  game->floor = 0;
  game->player_x = 0;
  game->player_y = 0;
}

// Game logic to move a player left.
void game_move_player_left(Game *game) {
  game->player_x = game->player_x - 1;
}

// Game logic to move a player right.
void game_move_player_right(Game *game) {
  game->player_x = game->player_x + 1;
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
void game_process_inputs(SDL_Event event, bool *keymap, Game *game)
{
  if (!SDL_PollEvent(&event)) { return; }

  int type = event.type;

  if (type != SDL_KEYDOWN && type != SDL_KEYUP) { return; }

  int keySym = event.key.keysym.sym;
  bool keyDown = type == SDL_KEYDOWN;

  if (keySym == 'a') { keymap[LEFT] = keyDown; }
  else if (keySym == 'd') { keymap[RIGHT] = keyDown; }
  else if (keySym == SDLK_ESCAPE) { keymap[ESC] = keyDown; }

  if (keymap[LEFT]) { game_move_player_left(game); }
  else if (keymap[RIGHT]) { game_move_player_right(game); }
}

// This takes a game and renders it on the screen.
void game_draw(SDL_Renderer *renderer, SDL_Texture *player_idle_textue, Game *game)
{
  SDL_RenderSetScale(renderer, 1, 1);
  SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
  SDL_RenderClear(renderer);
  render_texture(renderer, player_idle_textue, location_in_camera(game->player_x, game->player_y));
  SDL_RenderPresent(renderer);
}

// This will contain code to control the game.
void game_tick(Game *game)
{

}

// Entry point into the application.
int main(int argc, char *argv[])
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

  bool keymap[NUMBER_OF_KEYS];
  keymap[LEFT] = false;
  keymap[RIGHT] = false;
  keymap[ESC] = false;

  Game *game = (Game *)malloc(sizeof(Game));
  game_new(game);

  // Game loop.
  while (keymap[ESC] == false) {
    game_tick(game);
    game_process_inputs(event, keymap, game);
    game_draw(renderer, texture, game);
  }
}
