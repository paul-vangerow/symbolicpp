#ifndef PREGEX
#define PREGEX

#include <iostream>
#include <unordered_map>
#include <vector>
#include <unordered_set>
#include <stack>
#include <queue>

typedef char IN_T;
typedef std::string OUT_T;

struct PregexModifier {
    bool recursive = false;
    bool optional = false;
    std::size_t context_start;
    std::size_t context_end;
};

class PregexNode {
public:
    bool m_is_end;
    std::size_t m_node_number;
    std::unordered_map<IN_T, std::unordered_set<std::size_t>> m_transitions;

    PregexNode( 
        std::unordered_set<IN_T> const & int_transitions, 
        std::size_t node_number, 
        bool is_end = false ) 
        : m_is_end(is_end)
        , m_node_number(node_number)
    {
        std::size_t next_node = m_node_number+1;
        for (auto item : int_transitions){
            m_transitions.insert( std::make_pair(item, std::unordered_set<std::size_t>{next_node}) );
        }
    }

    void node_pleq(std::unique_ptr<PregexNode> & lhs){
        for (auto & item : lhs->m_transitions){
            auto it = m_transitions.find(item.first);
            if (it != m_transitions.end()) it->second.insert(item.second.begin(), item.second.end());
            else m_transitions.insert(item);
        }
    }

    void node_oreq(std::unique_ptr<PregexNode> & lhs){
        m_is_end |= lhs->m_is_end;
    }

    bool node_neq(std::unique_ptr<PregexNode> & lhs){
        return (m_node_number != lhs->m_node_number);
    }
};

struct MatchObject {
    std::vector<IN_T> token_sequence;
    OUT_T out_token = "";
    bool matched = false;
};

struct MatchPtr {
    std::size_t location = 0;
    std::vector<IN_T> tokens;
    std::vector<IN_T> full_match_tokens;
};

class PregexSequence {
private:
    std::vector<std::unique_ptr<PregexNode>> m_sequence;
    OUT_T m_out_token;
    std::queue<MatchPtr> m_ptrs;
public:
    PregexSequence(
        std::vector<std::unordered_set<IN_T>> const & internal_transitions, // All the transitions with which to construct nodes
        std::vector<PregexModifier> const & modifiers, // All the modifiers to apply
        OUT_T out_token);

    void print_sequence() {
        for (auto & node : m_sequence){
            std::cout << node->m_node_number << " : " << node->m_is_end << " | ";
            for (auto transition : node->m_transitions){
                std::cout << transition.first << "-(";
                for (auto links : transition.second) std::cout << links << " ";
                std::cout << ") ";
            }
            std::cout << "\n";
        }
        std::cout << "\n";
    }

    MatchObject match_input(IN_T token);

    void init_ptrs(){
        m_ptrs.push(MatchPtr{});
    }

    void reset_ptrs(){
        std::queue<MatchPtr> new_ptrs;
        m_ptrs = new_ptrs;
    }

    // Generate the required components needed for initialising PregexSequence off a classic Regex Input String
    static std::pair<std::vector<PregexModifier>, std::vector<std::unordered_set<char>>> parse_input_text(std::string text);
    static void build_character_set(char begin, char end, std::unordered_set<char> & set );
};

class Pregex {
private:
    std::vector<std::unique_ptr<PregexSequence>> m_sequences;
public:
    Pregex() = default;

    MatchObject match_token(IN_T in);

    void add_char_sequence(std::string match_string, std::string out_token);
};

#endif