#ifndef ARGPARSE
#define ARGPARSE

#include <iostream>

class ArgParser {
public:
    ArgParser() {
        std::cout << "Hello World!" << std::endl;
    }

    virtual void doSomething();
};

#endif