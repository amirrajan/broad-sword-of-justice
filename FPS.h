#ifndef __FPS_H_
#define __FPS_H_

class FPS {
public:
    unsigned long fps, frameCounter, lastTime, currentTime;

    FPS();
    int getFPS();
};
#endif // __FPS_H_
