#include "Input.h"

#include <SDL.h>

#include <iostream>

Input::Input() {
    this->windowClosed = false;
    for (int i = 0; i < NUMBER_OF_KEYS; i++) {
        this->keyMap[i] = false;
    }
}

void Input::process() {
    SDL_Event event;
    if (SDL_PollEvent(&event)) {
        int type = event.type;

        if (type == SDL_KEYDOWN || type == SDL_KEYUP) {
            int keySym = event.key.keysym.sym;
            bool keyDown = type == SDL_KEYDOWN;

            if (keySym == SDLK_LEFT) {
                keyMap[LEFT] = keyDown;
            } else if (keySym == SDLK_RIGHT) {
                keyMap[RIGHT] = keyDown;
            } else if (keySym == SDLK_ESCAPE) {
                keyMap[ESC] = keyDown;
            }
        }

        if (type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_CLOSE) {
            windowClosed = true;
        }
    }
}
