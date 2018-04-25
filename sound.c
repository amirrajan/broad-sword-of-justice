#include "sound.h"
#include <stdio.h>
#include "malloc_macros.c"

#define BSJ_TryLoadMusic(name) \
  sounds->music_##name = Mix_LoadMUS("./audio/music_"#name".ogg"); \
  if(sounds->music_##name == NULL) { \
    printf("Unable to load Ogg music file: %s\n", Mix_GetError()); return 1; } \

#define BSJ_TryLoadSound(name) \
  sounds->sound_##name = Mix_LoadWAV("./audio/sound_"#name".ogg"); \
  if(sounds->sound_##name == NULL) { \
    printf("Unable to load Ogg sound file: %s\n", Mix_GetError()); return 1; } \

int sound_init(BSJ_Sounds *sounds) {
  int initted = Mix_Init(MIX_INIT_OGG | MIX_INIT_MP3);
  Mix_OpenAudio(22050, MIX_DEFAULT_FORMAT, 2, 1024);

  BSJ_TryLoadMusic(boss_alley);
  BSJ_TryLoadMusic(boss_docks);
  BSJ_TryLoadMusic(justice);
  BSJ_TryLoadMusic(justice_lite);
  BSJ_TryLoadMusic(justice_lite_3_cracks);
  BSJ_TryLoadMusic(justice_loop);
  BSJ_TryLoadSound(justice_death);
  BSJ_TryLoadSound(dodge);
  BSJ_TryLoadSound(gunshot);
  BSJ_TryLoadSound(hurt0);
  BSJ_TryLoadSound(hurt1);
  BSJ_TryLoadSound(jump);
  BSJ_TryLoadSound(charge);
  BSJ_TryLoadSound(kill_this_fool);
  BSJ_TryLoadSound(swing);

  return 0;
}

int load_music(const char* filename, Mix_Music *out) {
  out = Mix_LoadMUS(filename);
  if (out == NULL) {
    printf("Unable to load Ogg music file: %s\n", Mix_GetError());
    return 1;
  }
  return 0;
}

int load_sound(const char* filename, Mix_Chunk *out) {
  out = Mix_LoadWAV(filename);
  if (out == NULL) {
    printf("Unable to load Ogg sfx file: %s\n", Mix_GetError());
    return 1;
  }
  return 0;
}

void sound_clean_up(BSJ_Sounds *sounds) {
  Mix_FreeMusic(sounds->music_boss_alley);
  Mix_FreeMusic(sounds->music_boss_docks);
  Mix_FreeMusic(sounds->music_justice);
  Mix_FreeMusic(sounds->music_justice_lite_3_cracks);
  Mix_FreeMusic(sounds->music_justice_lite);
  Mix_FreeMusic(sounds->music_justice_loop);
  Mix_FreeChunk(sounds->sound_justice_death);
  Mix_FreeChunk(sounds->sound_dodge);
  Mix_FreeChunk(sounds->sound_gunshot);
  Mix_FreeChunk(sounds->sound_hurt0);
  Mix_FreeChunk(sounds->sound_hurt1);
  Mix_FreeChunk(sounds->sound_jump);
  Mix_FreeChunk(sounds->sound_kill_this_fool);
  Mix_FreeChunk(sounds->sound_charge);
  Mix_FreeChunk(sounds->sound_swing);

  Mix_CloseAudio();
}

int music_play(Mix_Music* music) {
  if (Mix_PlayMusic(music, -1) == -1) {
    printf("Unable to play Ogg file: %s\n", Mix_GetError());
    return 1;
  }
  return 0;
}

void music_stop() {
  Mix_HaltMusic();
}

void sound_stop() {
  Mix_HaltChannel(0);
  Mix_HaltChannel(1);
  Mix_HaltChannel(2);
  Mix_HaltChannel(3);
  Mix_HaltChannel(4);
  Mix_HaltChannel(5);
  Mix_HaltChannel(6);
  Mix_HaltChannel(7);
  Mix_HaltChannel(8);
  Mix_HaltChannel(9);
  Mix_HaltChannel(10);
  Mix_HaltChannel(11);
  Mix_HaltChannel(13);
  Mix_HaltChannel(14);
  Mix_HaltChannel(15);
}

int sound_play(Mix_Chunk* sound) {
  if (Mix_PlayChannel(-1, sound, 0) == -1) {
    printf("Unable to play sound effect: %s\n", Mix_GetError());
    return 1;
  }
  return 0;
}
