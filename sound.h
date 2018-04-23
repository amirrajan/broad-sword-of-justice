#ifndef sound_h
#define sound_h

#include <SDL.h>
#include <SDL_mixer.h>

// Structure that holds all the sounds in the game.
typedef struct BSJ_Sounds {
  Mix_Music * music_boss_alley;
  Mix_Music * music_boss_docks;
  Mix_Music * music_justice;
  Mix_Music * music_justice_lite;
  Mix_Music * music_justice_loop;
  Mix_Chunk * sound_dodge;
  Mix_Chunk * sound_gunshot;
  Mix_Chunk * sound_hurt0;
  Mix_Chunk * sound_hurt1;
  Mix_Chunk * sound_jump;
  Mix_Chunk * sound_kill_this_fool;
  Mix_Chunk * sound_swing;
} BSJ_Sounds;

int sound_init();
int load_music(const char* filename, Mix_Music *out);
int load_sound(const char* filename, Mix_Chunk *out);
void sound_clean_up();

int music_play(Mix_Music* music);
void music_stop();

int sound_play(Mix_Chunk* sound);

#endif // sound_h