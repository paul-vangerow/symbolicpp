#include <iostream>
#include "arg_parser/arg_parser.h"
#include "parallel_regex/pregex.h"

// Make this generate lexer code based on a .yaml specification (create own YAML parser) //

int main(int argc, char* argv[]) {
    ArgumentParser argparser;

    argparser.add_arg("input string", "--s");

    std::unordered_map<std::string, std::string> parsed_args;

    if ( !argparser.parse_args(argc, argv, &parsed_args) ) {
        argparser.print_help();
        return 1;
    }

    std::cout << parsed_args["input string"] << "\n";

    ParallelRegex tokeniser;
    tokeniser.add_expression("steve", "s");
    tokeniser.add_expression("garry", "g");
    tokeniser.add_expression("jim", "j");
    tokeniser.add_expression("alex", "a");

    for (auto c : parsed_args["input string"] ){
        if ( auto v = tokeniser.match_character(c) ) std::cout << v.value() << std::endl;
    }

    return 0;
}