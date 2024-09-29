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

    Pregex lexer;
    lexer.add_char_sequence("ABC(ABCABD)?", "CC"); // Doesn't match the dot (Likely a context issue) <-- Context applying modifiers weird
    // lexer.add_char_sequence("\\+", "ADD"); // Reset behaviour off
    // lexer.add_char_sequence("=", "EQ");

    for (auto c : (parsed_args["input string"]+" ")){
        auto match_obj = lexer.match_token(c);
    }

    return 0;
}