#ifndef __SPRITE_H
#define __SPRITE_H

#include "Renderer.h"

class Sprite {
public:
    Sprite(Renderer *renderer, Texture *texture);
    ~Sprite();

    void render(int x, int y);
private:
    Renderer                        * renderer;
    Texture                         * texture;

    int width, height;
};
#endif // __SPRITE_H
