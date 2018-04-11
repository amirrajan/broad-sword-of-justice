#include "OS.h"

#include <SDL.h>

#include <iostream>
#include <fstream>
#include <sstream>

void OS::delay(long ms) {
    SDL_Delay(ms);
}

unsigned long OS::getTicks() {
    return SDL_GetTicks();
}

void OS::showErrorMessage(std::string title, std::string message) {
    SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, title.c_str(), message.c_str(), NULL);
}

// cleanup
std::string *OS::readFile(std::string fileName) {
    FILE * file = fopen(fileName.c_str(), "rb");
    char * buffer;

    fseek(file, 0L, SEEK_END);
    long size = ftell(file);

    rewind(file);

    buffer = (char*) calloc(1, size+1);
    fread(buffer, size, 1, file);

    fclose(file);

    return new std::string(buffer);
}
