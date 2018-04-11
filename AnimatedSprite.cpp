#include "AnimatedSprite.h"

#include "Renderer.h"
#include "OS.h"
#include "json-parser/json.h"

#include <iostream>
#include <cstdlib>
#include <fstream>

AnimatedSprite::AnimatedSprite(Renderer * renderer, string spriteAnimationConfFile) {
    this->renderer = renderer;
    this->frameCounter = 0;
    this->lastTick = 0;
    this->loadSpriteAnimationData(spriteAnimationConfFile);
}

AnimatedSprite::~AnimatedSprite() {
    for (vector<Animation*>::iterator it = this->animations->begin(); it != this->animations->end() ; it++) {
        Animation * animation = *it;
        for (vector<Frame*>::iterator fit = animation->frames->begin(); fit != animation->frames->end(); fit++) {
            Frame * frame = *fit;
            delete frame->sprite;
            delete frame;
        }
        delete animation;
    }
    delete this->animations;
    renderer->destroyTexture(this->spriteAtlas);
}

void AnimatedSprite::render(int x, int y, string state) {
    Animation * frameState = getAnimation(state);

    if (frameCounter >= (frameState->frames->size())) {
        frameCounter = 0;
    }

    Frame * frame = frameState->frames->at(frameCounter);

    frame->sprite->render(x, y);
}

void AnimatedSprite::play(string state) {

    Animation * frameState = getAnimation(state);

    int currentTick = OS::getTicks();

    if (frameCounter >= (frameState->frames->size())) {
        frameCounter = 0;
    }

    if (currentTick > (lastTick + frameState->frames->at(frameCounter)->delay)) {
        frameCounter++;
        lastTick = currentTick;
    }
}

// make this a std::map
Animation * AnimatedSprite::getAnimation(string stateName) {
    Animation * frameState = NULL;

    for (vector<Animation*>::iterator it = this->animations->begin() ; it != this->animations->end() ; it++) {
        if ((*it)->stateName.compare(stateName) == 0) {
            frameState = *it;
        }
    }

    if (!frameState) {
        OS::showErrorMessage("Error", "Frame State not found");
        exit(0);
    }

    return frameState;
}

void AnimatedSprite::loadSpriteAnimationData(string fileName) {
    this->animations = new std::vector<Animation*>();

    string *buffer = OS::readFile(fileName);

    json_value * json = json_parse(buffer->c_str(), buffer->length());
    if (!json) {
        OS::showErrorMessage("Json Error", "Unable to parse JSON.");
        exit(0);
    }

    this->spriteAtlas = renderer->createTextureFromFile(json->u.object.values[0].value->u.string.ptr);

    json_value * states = json->u.object.values[1].value;
    for (unsigned int i = 0; i < states->u.object.length; i++) {
        json_object_entry entry = states->u.object.values[i];
        json_value * value = entry.value;

        Animation * animation = new Animation();

        animation->stateName = entry.name;
        animation->frames = new vector<Frame*>();

        for (unsigned int j = 0; j < value->u.array.length; j++) {
            json_value sValue = *value->u.array.values[j];

            Frame * frame = new Frame();
            frame->delay = sValue["frameDelay"].u.integer;

            Rect rect;

            rect.x = sValue["marginLeft"].u.integer;
            rect.y = sValue["marginTop"].u.integer;
            rect.w = sValue["spriteWidth"].u.integer;
            rect.h = sValue["spriteHeight"].u.integer;

            Texture * texture = this->renderer->createTexture(this->spriteAtlas, &rect);
            Sprite * sprite = new Sprite(this->renderer, texture);

            frame->sprite = sprite;
            animation->frames->push_back(frame);
        }

        this->animations->push_back(animation);
    }

    json_value_free(json);
}
