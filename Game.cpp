#include "Game.h"
#include "Renderer.h"
#include "Input.h"
#include "FPS.h"
#include "Player.h"
#include "OS.h"

#include <sstream>
#include <string>
#include <cstdio>
#include <iostream>
#include <cstdlib>
#include <string.h>

using namespace std;

Game::Game() {
    this->quit = false;
    this->renderer = new Renderer(640, 480);
    this->player = new Player(this->renderer);
    this->input = new Input();
    this->fps = new FPS();
}

Game::~Game() {
    delete fps;
    delete input;
    delete player;
    delete renderer;
}

int Game::loop() {
    while (!quit) {
        processInputs();
        render();
        SDL_Delay(1000. / 60.);
    }
    return 0;
}

void Game::render() {
    renderer->clearBuffer();
    player->render();
    stringstream bufstream;
    bufstream << "FPS: " << fps->getFPS();
    renderer->renderText(bufstream.str(), 10, 10);
    renderer->flipBuffer();
}

void Game::processInputs() {
    input->process();

    if (input->windowClosed || input->keyMap[input->ESC]) {
        quit = true;
    }

    if (input->keyMap[input->LEFT]) {
        player->move(player->LEFT);
    } else if (input->keyMap[input->RIGHT]) {
        player->move(player->RIGHT);
    } else {
        player->move(player->STAND);
    }
}
