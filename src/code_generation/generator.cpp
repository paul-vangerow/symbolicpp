#include <iostream>
#include "arg_parser/arg_parser.h"
#include "parallel_regex/pregex.h"

int main(int argc, char* argv[]) {
    ArgumentParser argparser;

    argparser.add_arg("input string", "--s");

    std::unordered_map<std::string, std::string> parsed_args;

    if ( !argparser.parse_args(argc, argv, &parsed_args) ) {
        argparser.print_help();
        return 1;
    }

    std::cout << parsed_args["input string"] << "\n";

    PregexSequence parse_steve("steve", "s");
    PregexSequence parse_garry("garry", "g");
    PregexSequence parse_jim("jim", "j");
    PregexSequence parse_alex("alex", "a");

    for (auto c : parsed_args["input string"] ){ // If one matches, reset the rest. ALLOW parallel explorations
        if (parse_steve.match(c)) std::cout << parse_steve.get_token() << " ";
        if (parse_garry.match(c)) std::cout << parse_garry.get_token() << " ";
        if (parse_jim.match(c)) std::cout << parse_jim.get_token() << " ";
        if (parse_alex.match(c)) std::cout << parse_alex.get_token() << " ";
    }

    // PregexSequence parse_number("[0-9]+(.[0-9]+)?", "number");
    // PregexSequence parse_eol("\n", "eol");
    // PregexSequence parse_tab("\t", "tab");
    // PregexSequence parse_space(" ", "tab");

    return 0;
}