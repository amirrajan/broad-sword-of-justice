## How to get this running

Clone the repo.

Run the following:

```
cd SDL
./configure
make

cd ../SDL_image
./configure
make

cd ../SDL_ttf
./configure
make

cd ../Chipmunk2D
cmake .
make

cd ..
sh ./build_mac.sh
./broad-sword-of-justice
```

From here on out, all you have to do is run `./build_mac.sh`.
