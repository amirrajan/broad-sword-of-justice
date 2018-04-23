pkill broad-sword-of-justice

if [[ ! -d ./SDL/build/ ]]; then
    echo "SDL build output doesn't exist. BUILDING!!!"
    pushd SDL
    ./configure
    make
    sudo make install
    popd
fi

if [ ! -f ./SDL_image/libSDL2_image.la ]; then
    echo "SDL_image build output doesn't exist. BUILDING!!!"
    pushd ./SDL_image
    ./configure
    make
    sudo make install
    popd
fi

if [ ! -f ./SDL_ttf/libSDL2_ttf.la ]; then
    echo "SDL_ttf build output doesn't exist. BUILDING!!!"
    pushd ./SDL_ttf
    ./configure
    make
    sudo make install
    popd
fi

if [[ ! -d ./SDL_mixer/build/ ]]; then
    echo "SDL_mixer build output doesn't exist. BUILDING!!!"
    pushd ./SDL_mixer
    ./configure
    make
    sudo make install
    popd
fi

if [ ! -f ./Chipmunk2D/src/libchipmunk.7.0.2.dylib ]; then
    echo "Chipmunk build output doesn't exist. BUILDING!!!"
    pushd ./Chipmunk2D
    cmake .
    make
    sudo make install
    popd
fi

clang *.c -I./ \
      -I./SDL/include \
      -I./SDL_image \
      -I./SDL_ttf \
      -I./SDL_mixer \
      -I./Chipmunk2D/include/chipmunk \
      -o ./broad-sword-of-justice -lSDL2 -lSDL2_image -lSDL2_ttf -lSDL2_mixer

echo "lit."
