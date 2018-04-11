#include "FPS.h"

#include "OS.h"

FPS::FPS() {
    this->frameCounter = 0;
    this->fps = 0;
    this->lastTime = 0;
}

int FPS::getFPS() {
    frameCounter++;
    if (OS::getTicks() >= lastTime + 1000) {
        lastTime = OS::getTicks();
        fps = frameCounter;
        frameCounter = 0;
    }

    return fps;
}
