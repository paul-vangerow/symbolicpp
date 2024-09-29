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
    lexer.add_char_sequence("[0-9]+(.[0-9]+)?", "NUM");
    lexer.add_char_sequence("MOD", "MOD");
    lexer.add_char_sequence("[a-zA-Z]+", "VAR");
    lexer.add_char_sequence("\\+", "ADD");
    lexer.add_char_sequence("=", "EQ");

    for (auto c : (parsed_args["input string"]+" ")){
        auto match_obj = lexer.match_token(c);
        std::cout << match_obj;
    }

    return 0;
}