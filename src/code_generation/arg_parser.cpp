#include <arg_parser.h>

void ArgumentParser::add_arg(std::string name, std::string prefix, bool optional){
    auto argument = std::make_pair( prefix, name );

    if (!optional) m_arg_map.emplace(argument);
    else m_arg_optional.emplace(argument);
}

void ArgumentParser::print_help(){
    std::string msg = "USAGE: code_generator ";
    for (auto item : m_arg_map) msg += (item.first + " " + item.second + " ");
    for (auto item : m_arg_optional) msg += ( "( " + item.first + " " + item.second + " )");
    std::cout << msg << "\n";
}

bool ArgumentParser::parse_args(int argc, char* argv[], std::unordered_map<std::string, std::string>* arg_map){
    if ( ((argc - 1) / 2) != m_arg_map.size() ) return false;

    m_arg_map.insert(m_arg_optional.begin(), m_arg_optional.end());

    for (int arg_no = 1; arg_no < argc; arg_no+=2){
        if ( auto it = m_arg_map.find( std::string(argv[arg_no])); it != m_arg_map.end()){
            arg_map->emplace(std::make_pair(it->second, std::string(argv[arg_no+1])));
        } else return false;
    }

    return true;
}