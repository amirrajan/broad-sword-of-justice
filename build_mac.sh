c++ ./*.c main.cpp -Wl \
    -I./ \
    -I./SDL2/Headers \
    -I./SDL2_image \
    -I./SDL2_ttf \
    -lSDL2 \
    -lSDL2_image \
    -lSDL2_ttf \
    -I./chipmunk/include/chipmunk \
    -o ./broad-sword-of-justice
