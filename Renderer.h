#ifndef __RENDERER_H
#define __RENDERER_H

#include <string>

class Rect {
public:
    int x, y, w, h;
};

class TextureImpl;
class Texture {
public:
    class TextureImpl * impl;
};

class RendererImpl;
class Renderer {
public:
    Renderer(int width, int height);
    ~Renderer();

    void flipBuffer();
    void clearBuffer();
    void renderText(std::string str, int x, int y);
    Texture * createTextureFromFile(std::string textureFileName);
    void destroyTexture(Texture * texture);
    void renderTexture(Texture * texture, Rect * srcRect, Rect * destRect);
    void renderTexture(Texture * texture, int x, int y);
    Texture * createTexture(Texture * sourceTexture, Rect * srcrect);
private:
    RendererImpl * impl;
};

#endif // __RENDERER_H
