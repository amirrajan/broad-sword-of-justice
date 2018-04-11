#include "Player.h"

#include "OS.h"

#include <vector>
#include <iostream>

Player::Player(Renderer * renderer) {
    this->animatedSprite = new AnimatedSprite(renderer, "spriteAnimations.json");
    this->state = "standRight";
    this->posx = 30;
    this->posy = 30;
    this->lastTick = OS::getTicks();
}

Player::~Player() {
    delete animatedSprite;
}

void Player::render() {
    animatedSprite->render(this->posx, this->posy, this->state);
}

void Player::move(PlayerMoveDirection dir) {
    double deltaMs = OS::getTicks() - lastTick;
    this->lastTick = OS::getTicks();
    double distanceTaken = deltaMs / 10; // 1000 ms / 100 px = deltaMs ms / x px -> x px = deltaMs / 10

    this->lastState = this->state;

    if (dir == STAND) {
        if (this->lastState == "right") {
            this->state = "standRight";
        }

        if (this->lastState == "left") {
            this->state = "standLeft";
        }
    }

    if (dir == RIGHT) {
        this->posx += distanceTaken;
        this->state = "right";
    }

    if (dir == LEFT) {
        this->posx -= distanceTaken;
        this->state = "left";
    }

    this->animatedSprite->play(this->state);
}
