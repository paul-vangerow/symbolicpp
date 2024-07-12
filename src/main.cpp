#include <iostream>
#include <argparser.hpp>

int main(int argc, char* argv[]) {
    std::cout << "Hello World" << std::endl;

    ArgParser argument;

    argument.doSomething();

    return 0;
}