#ifndef __Input_H
#define __Input_H

class Input {
public:
    Input();

    void process();

    bool windowClosed;

    enum Key { LEFT = 0, RIGHT, ESC, NUMBER_OF_KEYS };
    bool keyMap[NUMBER_OF_KEYS];

private:

    void reset();
};
#endif // __Input_H
