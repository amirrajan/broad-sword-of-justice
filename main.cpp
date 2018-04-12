#include <cstdlib>
#include <string>
#include <SDL.h>
#include <SDL_render.h>
#include <SDL_image.h>
#include <SDL_ttf.h>

std::string * read_file(std::string fileName)
{
  FILE * file = fopen(fileName.c_str(), "rb");
  char * buffer;

  fseek(file, 0L, SEEK_END);
  long size = ftell(file);

  rewind(file);

  buffer = (char*) calloc(1, size+1);
  fread(buffer, size, 1, file);

  fclose(file);

  return new std::string(buffer);
}

SDL_Texture * create_texture_from_file(SDL_Renderer * renderer, std::string file_name) {
  SDL_Surface * surface = IMG_Load(file_name.c_str());
  SDL_SetColorKey(surface, SDL_TRUE, SDL_MapRGB(surface->format, 0x00, 0x40, 0x80) );
  return SDL_CreateTextureFromSurface(renderer, surface);
}

SDL_Texture * create_texture(SDL_Renderer * renderer, SDL_Rect * srcrect, SDL_Rect * desrect)
{
  SDL_Texture * texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGB24, SDL_TEXTUREACCESS_TARGET, srcrect->w, srcrect->h);

  SDL_SetRenderTarget(renderer, texture);
  SDL_RenderCopy(renderer, texture, srcrect, NULL);
  SDL_RenderPresent(renderer);
  SDL_SetRenderTarget(renderer, NULL);

  return texture;
}

void render_texture(SDL_Renderer * renderer, SDL_Texture * texture, int x, int y)
{
  SDL_Rect destrect;
  destrect.x = x;
  destrect.y = y;
  destrect.w = 797 / 10.0;
  destrect.h = 528 / 5.0;

  SDL_Rect cliprect;
  cliprect.x = x;
  cliprect.y = y;
  cliprect.w = 797 / 16.0;
  cliprect.h = 528 / 8.0;

  SDL_RenderCopy(renderer, texture, &cliprect, &destrect);
}

void render_texture(SDL_Renderer * renderer, SDL_Texture * texture, SDL_Rect * srcrect, SDL_Rect * destrect)
{
  SDL_RenderCopy(renderer, texture, srcrect, destrect);
}

int main(int argc, char *argv[])
{
  SDL_Window * window;
  SDL_Renderer * renderer;
  TTF_Font * font;

  SDL_Init(SDL_INIT_VIDEO);
  window = SDL_CreateWindow("Game", 50, 50, 1024, 768, SDL_WINDOW_OPENGL);
  renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
  TTF_Init();
  font = TTF_OpenFont("PTS75F.ttf", 12);

  SDL_Texture * texture = create_texture_from_file(renderer, "MegaMan1.png");
  bool quit = false;

  SDL_RenderSetScale(renderer, 1, 1);
  SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
  SDL_RenderClear(renderer);
  render_texture(renderer, texture, 0, 0);
  SDL_RenderPresent(renderer);
  getchar();
}
