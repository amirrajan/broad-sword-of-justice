#ifdef _WINDOWS
#include "windows.h"
#endif
#include <cstdlib>
#include <stdarg.h>
#include <string>
#include <SDL.h>
#include <SDL_render.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include "game.c"

// Helper method to create a texture from a file.
SDL_Texture * create_texture_from_file(SDL_Renderer * renderer, SDL_Surface * surface, std::string file_name)
{
  surface = IMG_Load(file_name.c_str());
  SDL_SetColorKey(surface, SDL_TRUE, SDL_MapRGB(surface->format, 0x00, 0x40, 0x80) );
  return SDL_CreateTextureFromSurface(renderer, surface);
}

// Helper method to take a sprite, and render it onto the scene at a specific location.
void render_texture(SDL_Renderer * renderer, SDL_Texture * texture, BSJ_Point point, double angle, SDL_RendererFlip flip)
{
  SDL_Rect destrect;
  destrect.x = point.x;
  destrect.y = point.y;
  destrect.w = 128;
  destrect.h = 128;

  SDL_Rect cliprect;
  cliprect.x = 0;
  cliprect.y = 0;
  cliprect.w = 128;
  cliprect.h = 128;

  SDL_Point center;
  center.x = 0;
  center.y = 0;

  SDL_RenderCopyEx(renderer, texture, &cliprect, &destrect, angle, &center, flip);
}

// macro to define a malloc with a variable name
#define MALLOC(type, variable_name) type * variable_name = (type *)malloc(sizeof(type))

// macro to define a malloc for assignment
#define MALLOCA(type) (type *)malloc(sizeof(type))

#define MALLOCS(type, variable_name, count) type * variable_name = (type **)malloc(sizeof(type *) * count)
#define MALLOCSA(type, count) (type **)malloc(sizeof(type *) * count)

typedef struct {
  int duration;
  SDL_Texture *texture;
} IntOfSDL_Texture;

typedef struct {
  IntOfSDL_Texture ** texture_tuples;
  int current_index;
} BSJ_Sprite;

typedef struct {
  BSJ_Sprite * player_idle;
  BSJ_Sprite * player_attack;
  BSJ_Sprite * boss_idle;
} BSJ_Sprites;

typedef struct {
  SDL_Window * window;
  SDL_Renderer * renderer;
  SDL_Surface * surface;
  TTF_Font * font;
  SDL_Event * event;
} SDL_Context;

// This takes a game and renders it on the screen.
void game_draw(SDL_Context *context, BSJ_Sprites *sprites, BSJ_Game *game)
{
  SDL_RenderClear(context->renderer);
  render_texture(context->renderer,
  		 sprites->player_idle->texture_tuples[0]->texture,
  		 location_in_camera(game->player_x, game->player_y),
  		 0,
  		 game->player_facing == -1 ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE);
  render_texture(context->renderer,
  		 sprites->boss_idle->texture_tuples[0]->texture,
  		 location_in_camera(game->boss_x, game->boss_y),
  		 0,
  		 game->boss_facing == -1 ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE);
  SDL_RenderPresent(context->renderer);
}

SDL_Context * game_new_sdl_context()
{
  MALLOC(SDL_Context, o);
  o->window = SDL_CreateWindow("Game", 50, 50, 1024, 768, SDL_WINDOW_OPENGL);
  o->renderer = SDL_CreateRenderer(o->window, -1, SDL_RENDERER_ACCELERATED);
  o->surface = MALLOCA(SDL_Surface);
  o->font = TTF_OpenFont("PTS75F.ttf", 12);
  o->event = MALLOCA(SDL_Event);
  return o;
}

// This takes in the current inputs by SDL and maps them to methods to call within the game.
void game_process_inputs(SDL_Event * event, BSJ_Game *game)
{
  // poll for all events
  while (SDL_PollEvent(event)) {
    int type = event->type;

    // we will need to handle other types of events (e.g. window closing) here later
    if (type != SDL_KEYDOWN && type != SDL_KEYUP) { continue; }

    int keySym = event->key.keysym.sym;
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

  SDL_Context * context = game_new_sdl_context();
  MALLOC(BSJ_Sprites, sprites);


  MALLOC(BSJ_Game, game);
  game_new(game);

  SDL_RenderSetScale(context->renderer, 1, 1);
  SDL_SetRenderDrawColor(context->renderer, 0, 0, 0, 255);

  // Game loop.
  while (game->keys_q == false) {
    game_tick(game);
    game_process_inputs(context->event, game);
    game_draw(context, sprites, game);
    SDL_Delay(1000. / 60.);
    // if ((int)game->horizontal_velocity != 0) {
    //   SDL_Log("%f", game->horizontal_velocity);
    // }
  }

  return 0;
}
