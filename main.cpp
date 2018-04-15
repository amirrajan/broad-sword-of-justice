#ifdef _WINDOWS
#include "windows.h"
#endif
#include <cstdlib>
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
void render_texture(SDL_Renderer * renderer, SDL_Texture * texture, Point point, double angle, SDL_RendererFlip flip)
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

  SDL_RenderCopyEx(renderer, texture, &cliprect, &destrect, angle, NULL, flip);
}

// This takes a game and renders it on the screen.
void game_draw(SDL_Renderer *renderer, SDL_Texture *player_idle_texture, SDL_Texture *boss_idle_textue, Game *game)
{
  SDL_RenderClear(renderer);
  render_texture(renderer,
		 player_idle_texture,
		 location_in_camera(game->player_x, game->player_y),
		 0,
		 game->player_facing == -1 ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE);
  render_texture(renderer,
		 boss_idle_textue,
		 location_in_camera(game->boss_x, game->boss_y),
		 0,
		 game->boss_facing == -1 ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE);
  SDL_RenderPresent(renderer);
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
  SDL_Texture * player_idle = create_texture_from_file(renderer, surface, "player_idle.png");
  SDL_Texture * boss_idle_texture = create_texture_from_file(renderer, surface, "boss_1_idle.png");
  SDL_Event event;

  Game *game = (Game *)malloc(sizeof(Game));
  game_new(game);

  SDL_RenderSetScale(renderer, 1, 1);
  SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);

  // Game loop.
  while (game->keys_q == false) {
    game_tick(game);
    game_process_inputs(event, game);
    game_draw(renderer, player_idle, boss_idle_texture, game);
    SDL_Delay(1000. / 60.);
    if ((int)game->horizontal_velocity != 0) {
      SDL_Log("%f", game->horizontal_velocity);
    }
  }

  return 0;
}
