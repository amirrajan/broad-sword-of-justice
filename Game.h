#ifndef __Game_H
#define __Game_H

#include "Player.h"
#include "Input.h"
#include "Renderer.h"
#include "FPS.h"

#include <SDL.h>
#include <SDL_ttf.h>
#include <SDL_ttf.h>

class Game {
public:
    Game();
    ~Game();

    int loop();
private:
    bool quit;

    // Game subsystems
    Renderer * renderer;
    Player * player;
    Input * input;
    FPS * fps;

    void processInputs();
    void render();
};
#endif // __Game_H
