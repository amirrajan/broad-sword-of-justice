pkill broad-sword-of-justice

if [[ ! -d ./SDL/build/ ]]; then
    echo "SDL build output doesn't exist. BUILDING!!!"
    pushd SDL
    ./configure
    make
    popd
fi

if [ ! -f ./SDL_image/libSDL2_image.la ]; then
    echo "SDL_image build output doesn't exist. BUILDING!!!"
    pushd ./SDL_image
    ./configure
    make
    popd
fi

if [ ! -f ./SDL_ttf/libSDL2_ttf.la ]; then
    echo "SDL_ttf build output doesn't exist. BUILDING!!!"
    pushd ./SDL_ttf
    ./configure
    make
    popd
fi

clang *.c -I./ \
      -I./SDL/include \
      -I./SDL_image \
      -I./SDL_ttf \
      -I./Chipmunk2d/include/chipmunk \
      -L./SDL \
      -L./SDL_image \
      -L./SDL_ttf \
      -o ./broad-sword-of-justice -lSDL2 -lSDL2_image -lSDL2_ttf
