#include "Sprite.h"

Sprite::Sprite(Renderer *renderer, Texture *texture)
{
    this->renderer = renderer;
    this->texture = texture;
}

Sprite::~Sprite()
{

}

void Sprite::render(int x, int y)
{
    this->renderer->renderTexture(this->texture, x, y);
}
