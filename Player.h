#ifndef __PLAYER_H_
#define __PLAYER_H_

#include "Renderer.h"
#include "AnimatedSprite.h"

class Player {
public:
    Player(Renderer * renderer);
    ~Player();

    enum PlayerMoveDirection { LEFT, RIGHT, UP, DOWN, STAND };
    void render();
    void move(PlayerMoveDirection dir);

private:
    AnimatedSprite * animatedSprite;

    string state, lastState;
    double posx, posy;
    int lastTick;

};
#endif // __PLAYER_H_
