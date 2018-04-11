#ifndef __OS_H_
#define __OS_H_

#include <string>

class OS {
public:
    static void delay(long ms);
    static unsigned long getTicks();
    static void showErrorMessage(std::string title, std::string message);
    static std::string *readFile(std::string fileName);
};
#endif // __OS_H_
