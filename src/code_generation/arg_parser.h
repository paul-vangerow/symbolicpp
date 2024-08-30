#ifndef ARGPARSE
#define ARGPARSE

#include <unordered_map>
#include <iostream>

class ArgumentParser{
private:
    std::unordered_map<std::string, std::string > m_arg_map;
    std::unordered_map<std::string, std::string > m_arg_optional;
public:
    ArgumentParser(){}

    void add_arg(std::string prefix, std::string name, bool optional = false);
    bool parse_args(int argc, char* argv[], std::unordered_map<std::string, std::string>* arg_map);
    void print_help();
};

#endif