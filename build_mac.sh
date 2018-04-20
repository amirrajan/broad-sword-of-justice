clang *.c -I./ \
      -I./SDL/include \
      -I./SDL_image \
      -I./SDL_ttf \
      -I./Chipmunk2d/include/chipmunk \
      -L./SDL \
      -L./SDL_image \
      -L./SDL_ttf \
      -o ./broad-sword-of-justice -lSDL2 -lSDL2_image -lSDL2_ttf
