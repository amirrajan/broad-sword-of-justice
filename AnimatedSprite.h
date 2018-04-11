#ifndef __ANIMATION_H_
#define __ANIMATION_H_

#include "Renderer.h"
#include "Sprite.h"

#include <vector>

using namespace std;

class Frame {
public:
    int delay;
    Sprite * sprite;
};

class Animation {
public:
    string          stateName;
    vector<Frame*>   * frames;
};

class AnimatedSprite {
public:
    AnimatedSprite(Renderer * renderer, string spriteAnimationConfFile);
    ~AnimatedSprite();

    void render(int x, int y, string state);
    void play(string state);

private:
    Renderer                        * renderer;
    Texture                         * spriteAtlas;

    vector<Animation*>              * animations;

    unsigned int                    frameCounter;
    int                             lastTick;

    void loadSpriteAnimationData(string fileName);
    Animation * getAnimation(string stateName);
};


#endif // __ANIMATION_H_
