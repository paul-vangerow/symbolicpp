#ifndef PREGEX
#define PREGEX

#include <iostream>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <stack>
#include <chrono>

struct PregexNode {
    std::unordered_map<char, std::vector<int>> m_transitions;
    int m_node_number=0;
    bool m_is_end=false;

    void operator+=( const std::unique_ptr<PregexNode>& rhs){
        m_is_end |= rhs->m_is_end;
        
        for (auto key : rhs->m_transitions ) 
            for (auto item : key.second)
                *this += std::make_pair(key.first, item);
    }

    void operator+=( const std::pair<char, int>& rhs){
        auto it = m_transitions.find(rhs.first);
        if (it == m_transitions.end()) m_transitions.insert( std::make_pair(rhs.first, std::vector<int>{rhs.second}) );
        else it->second.push_back(rhs.second);
    }
};

struct PregexModifier {
    int begin;
    int end;
    bool optional;
    bool recursive;

    void apply( std::vector<std::unique_ptr<PregexNode>> & nodes ) {
        if (optional) *nodes.at(begin) += nodes.at(end);
        if (recursive) *nodes.at(end) += nodes.at(begin);
    }
};

class PregexSequence {
private:
    std::string m_token;
    bool m_error; // Maybe remove? Not going to do thorough error checking I think.
    std::vector<std::unique_ptr<PregexNode>> m_sequence;
    std::vector<std::unique_ptr<PregexModifier>> m_modifiers;

    int m_ptr;
public:
    PregexSequence(std::string match_string, std::string token);

    void build_sequence(std::string match_string);

    void add_set(char begin, char end, std::unique_ptr<PregexNode> & node){
        int next_node = node->m_node_number+1;
        for (char v = begin; v <= end; v++) *node += std::make_pair(v, next_node);
    }

    std::string get_token() {
        return m_token;
    }

    bool match(char c);

    void print_nodes(){
        for (auto & node : m_sequence){
            std::cout << "NODE: " << node->m_node_number << " | ";
            for (auto transition : node->m_transitions) {
                for (auto kind : transition.second) std::cout << transition.first << ": " << kind << " ";
            }
            std::cout << "END: " << node->m_is_end << "\n";
        }
    }
};

#endif