#include "Renderer.h"

#include <cstdlib>
#include <string>

#include <SDL.h>
#include <SDL_render.h>
#include <SDL_image.h>
#include <SDL_ttf.h>

// helper
static void createSDLRectFromRect(SDL_Rect * sdlRect, Rect * rect);

class TextureImpl {
public:
    SDL_Texture * texture;
};

class RendererImpl {
public:
    SDL_Window * window;
    SDL_Renderer * renderer;

    TTF_Font * font;
};

Renderer::Renderer(int width, int height) {
    this->impl = new RendererImpl();

    if (SDL_Init(SDL_INIT_VIDEO)) {
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Init error.", "Unable to initialize SDL.", NULL);
        exit(0);
    }

    if (!(this->impl->window = SDL_CreateWindow("Game", 50, 50, width, height, SDL_WINDOW_OPENGL))) {
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Init error.", "Unable to create window.", NULL);
        exit(0);
    }

    if (!(this->impl->renderer = SDL_CreateRenderer(this->impl->window, -1, SDL_RENDERER_ACCELERATED))) {
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Init error.", "Unable to create renderer.", NULL);
        exit(0);
    }

    if (TTF_Init()) {
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Init error.", "Unable to initialize font.", NULL);
        exit(0);
    }

    if (!(this->impl->font = TTF_OpenFont("PTS75F.ttf", 12))) {
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Init error.", "Unable to load font.", NULL);
        exit(0);
    }
}

Renderer::~Renderer() {
    SDL_DestroyRenderer(this->impl->renderer);
    SDL_DestroyWindow(this->impl->window);
    delete this->impl;
    TTF_Quit();
    SDL_Quit();
}

void Renderer::renderTexture(Texture * texture, Rect * srcrect, Rect * destrect) {
    SDL_Rect sdlSrcRect, sdlDestRect;
    createSDLRectFromRect(&sdlSrcRect, srcrect);
    createSDLRectFromRect(&sdlDestRect, destrect);
    SDL_RenderCopy(this->impl->renderer, texture->impl->texture, &sdlSrcRect, &sdlDestRect);
}

void Renderer::renderTexture(Texture * texture, int x, int y)
{
    SDL_Rect destrect;

    destrect.x = x;
    destrect.y = y;

    SDL_QueryTexture(texture->impl->texture, NULL, NULL, &destrect.w, &destrect.h);
    SDL_RenderCopy(this->impl->renderer, texture->impl->texture, NULL, &destrect);
}

Texture * Renderer::createTexture(Texture * sourceTexture, Rect * srcrect)
{
    SDL_Texture * texture = SDL_CreateTexture(this->impl->renderer,
                                              SDL_PIXELFORMAT_RGB24,
                                              SDL_TEXTUREACCESS_TARGET,
                                              srcrect->w, srcrect->h);

    SDL_Rect sdlSrcrect;

    createSDLRectFromRect(&sdlSrcrect, srcrect);

    SDL_SetRenderTarget(this->impl->renderer, texture);
    SDL_RenderCopy(this->impl->renderer, sourceTexture->impl->texture, &sdlSrcrect, NULL);
    SDL_RenderPresent(this->impl->renderer);
    SDL_SetRenderTarget(this->impl->renderer, NULL);

    Texture * retTexture = new Texture();
    retTexture->impl = new TextureImpl();
    retTexture->impl->texture = texture;

    return retTexture;
}

static void createSDLRectFromRect(SDL_Rect * sdlRect, Rect * rect) {
    sdlRect->x = rect->x;
    sdlRect->y = rect->y;
    sdlRect->w = rect->w;
    sdlRect->h = rect->h;
}

void Renderer::renderText(std::string str, int x, int y) {
    SDL_Surface * surface;
    SDL_Texture * texture;
    SDL_Color color;
    SDL_Rect rect;
    int w, h;

    color.r = 255;
    color.g = 255;
    color.b = 255;
    color.a = 255;

    surface = TTF_RenderText_Solid(this->impl->font, str.c_str(), color);
    texture = SDL_CreateTextureFromSurface(this->impl->renderer, surface);

    SDL_QueryTexture(texture, NULL, NULL, &w, &h);

    rect.x = x;
    rect.y = y;
    rect.w = w;
    rect.h = h;

    SDL_RenderCopy(this->impl->renderer, texture, NULL, &rect);
    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);
}

Texture * Renderer::createTextureFromFile(std::string textureFileName) {
    SDL_Surface * surface = IMG_Load(textureFileName.c_str());
    if (!surface) {
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Image load error.", "Unable to load image.", NULL);
        exit(0);
    }

    /* set color masking */
    SDL_SetColorKey(surface, SDL_TRUE , SDL_MapRGB(surface->format, 0x00, 0x40, 0x80) );

    Texture * texture = new Texture();
    texture->impl = new TextureImpl();
    texture->impl->texture = SDL_CreateTextureFromSurface(this->impl->renderer, surface);
    return texture;
}

void Renderer::destroyTexture(Texture * texture) {
    SDL_DestroyTexture(texture->impl->texture);
    delete texture->impl;
    delete texture;
}

void Renderer::clearBuffer() {
    SDL_RenderSetScale(this->impl->renderer, 3, 3);
    SDL_SetRenderDrawColor(this->impl->renderer, 0, 0, 0, 255);
    SDL_RenderClear(this->impl->renderer);
}

void Renderer::flipBuffer() {
    SDL_RenderPresent(this->impl->renderer);
}

