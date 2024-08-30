#include <iostream>
#include <arg_parser.h>

int main(int argc, char* argv[]) {
    ArgumentParser argparser;

    argparser.add_arg("input path", "--i");

    std::unordered_map<std::string, std::string> parsed_args;

    if ( !argparser.parse_args(argc, argv, &parsed_args) ) {
        argparser.print_help();
        return 1;
    }

    std::cout << parsed_args["input path"] << "\n";

    return 0;
}