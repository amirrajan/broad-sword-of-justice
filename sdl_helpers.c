// SDL goop.
typedef struct {
  SDL_Window * window;
  SDL_Renderer * renderer;
  SDL_Surface * surface;
  TTF_Font * font;
  SDL_Event * event;
} SDL_Context;

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
