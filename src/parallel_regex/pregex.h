#ifndef PREGEX
#define PREGEX

#include <iostream>
#include <unordered_map>
#include <vector>
#include <unordered_set>
#include <stack>
#include <queue>

struct PregexModifier {
    bool recursive = false;
    bool optional = false;
    std::size_t context_start;
    std::size_t context_end;
};

template <class IN_T, class OUT_T>
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

template <class IN_T, class OUT_T>
struct MatchObject {
    std::vector<IN_T> token_sequence;
    OUT_T out_token = "";
    bool matched = false;
};

template <class IN_T, class OUT_T>
class PregexSequence {
private:
    struct MatchPtr {
        std::size_t location = 0;
        std::vector<IN_T> tokens;
        std::vector<IN_T> full_match_tokens;
    };

    std::vector<std::unique_ptr<PregexNode<IN_T, OUT_T>>> m_sequence;
    OUT_T m_out_token;
    std::queue<MatchPtr> m_ptrs;
public:
    PregexSequence(
        std::vector<std::unordered_set<IN_T>> const & internal_transitions,
        std::vector<PregexModifier> const & modifiers, 
        OUT_T out_token)
        : m_out_token(out_token)
    {
        // Construct all nodes
        std::size_t node_number = 0;
        for (auto item : internal_transitions){
            m_sequence.emplace_back( new PregexNode<IN_T, OUT_T>(item, node_number++) ); // probably can replace with emplace back
        }
        // Construct final node
        m_sequence.emplace_back( new PregexNode<IN_T, OUT_T>( std::unordered_set<IN_T>{}, node_number, true ) );

        // Apply modifiers to nodes
        for (auto mod : modifiers){
            auto & end_node = m_sequence.at(mod.context_end);
            auto & begin_node = m_sequence.at(mod.context_start);
            if (mod.optional){
                begin_node->node_oreq(end_node);
                if (end_node != m_sequence.back()) begin_node->node_pleq(end_node);
            }
            if (mod.recursive) end_node->node_pleq(begin_node);
        }
    }

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

    MatchObject<IN_T, OUT_T> match_input(IN_T token){
        MatchObject<IN_T, OUT_T> output;
        std::queue<MatchPtr> new_ptrs;

        if (!m_ptrs.size()) init_ptrs();

        while (m_ptrs.size()){
            auto front_element = m_ptrs.front();
            m_ptrs.pop();

            auto & front_node = m_sequence.at(front_element.location);
            auto & front_transitions = front_node->m_transitions;
            auto front_find_token = front_transitions.find(token);

            // bool is_end = front_node.m_is_end;
            bool token_not_found = front_find_token == front_transitions.end();

            if (token_not_found) {
                // We have reached an end without futher matches. This is a completion candidate.
                if (front_element.full_match_tokens.size()) {
                    output.token_sequence = front_element.full_match_tokens;
                    output.out_token = m_out_token;
                    m_ptrs.push( MatchPtr{} );
                }
                continue;
            }
        
            // Add the new token
            front_element.tokens.push_back(token); // maybe emplace if token has a copy constructor

            // We have found a match, go through all possible links:
            for (auto link : front_find_token->second){
                auto link_node_end = m_sequence.at(link)->m_is_end;

                // Make note if there is a match on the transition node.
                output.matched |= link_node_end;

                // Create the new linked node
                MatchPtr new_link_ptr = front_element;
                new_link_ptr.location = link;

                if (link_node_end) new_link_ptr.full_match_tokens = new_link_ptr.tokens;
                
                new_ptrs.push(new_link_ptr);
            }
        }
        m_ptrs = new_ptrs;

        return output;
    }

    void init_ptrs(){
        m_ptrs.push(MatchPtr{});
    }

    void reset_ptrs(){
        std::queue<MatchPtr> new_ptrs;
        m_ptrs = new_ptrs;
    }

    // Generate the required components needed for initialising PregexSequence off a classic Regex Input String
    static std::pair<std::vector<PregexModifier>, std::vector<std::unordered_set<char>>> parse_input_text(std::string text){
        std::vector<PregexModifier> modifiers;
        std::vector<std::unordered_set<char>> characters;

        std::stack<std::size_t> context_stack;

        bool escape = false;
        bool build_set = false;

        size_t begin = 0;
        size_t end = 0;

        std::string set_accumulator = "";

        for(auto c : text){
            switch(c){
                case '[': { // Start a set
                    build_set = true;
                } break;
                case ']': { // Finish a set
                    build_set = false;
                    std::unordered_set<char> items;
                    for (int i = 0; i < set_accumulator.size(); i+=3){
                        build_character_set(set_accumulator.at(i), set_accumulator.at(i+2), items);
                    }
                    characters.emplace_back(std::move(items));
                    set_accumulator = "";
                    begin = end;
                    end++;
                } break;
                case '\\': { // Escape next character
                    escape = true;
                } break;
                case '(': { // Open a context
                    context_stack.push(begin);
                } break;
                case ')': { // Close a context
                    begin = context_stack.top()+1;
                    context_stack.pop();
                    break;
                }
                case '+':
                case '?':
                case '*': {
                    if (!escape && !build_set) {
                        modifiers.emplace_back( 
                        PregexModifier{
                            .context_start=begin, 
                            .context_end=end, 
                            .optional=(c=='?'||c=='*'),
                            .recursive=(c=='+'||c=='*')
                        } );
                        break;
                    }
                    escape = false;
                }
                default: {
                    if (build_set) {
                        set_accumulator+=c;
                        continue;
                    }
                    characters.emplace_back( std::unordered_set<char>{c} );

                    begin = end;
                    end++;
                } break;
            }
        }
        return std::make_pair( modifiers, characters );
    }
    static void build_character_set(char begin, char end, std::unordered_set<char> & set ){
        for (char i = begin; i <= end; i++) set.insert(i);
    }
};

template <class IN_T, class OUT_T>
class Pregex {
private:
    std::vector<std::unique_ptr<PregexSequence<IN_T, OUT_T>>> m_sequences;
public:
    Pregex() = default;

    MatchObject<IN_T, OUT_T> match_token(IN_T in){
        MatchObject<IN_T, OUT_T> out;
        std::size_t matched_point = m_sequences.size();
        for (std::size_t it = 0; it < m_sequences.size(); it++){
            auto out_obj = m_sequences.at(it)->match_input(in);

            if (out_obj.matched) matched_point = std::min(matched_point, it);

            // if a character is matched just don't match any others??
            if (out_obj.token_sequence.size())  out = out_obj;
        }
        for (std::size_t erase_it = matched_point+1; erase_it < m_sequences.size(); erase_it++){
            m_sequences.at(erase_it)->reset_ptrs();
        }

        return out;
    }

    void add_char_sequence(std::string match_string, std::string out_token){
        auto v = PregexSequence<IN_T, OUT_T>::parse_input_text(match_string);
        auto sequence = std::make_unique<PregexSequence<IN_T, OUT_T>>(v.second, v.first, out_token);
        sequence->print_sequence();
        m_sequences.push_back(std::move(sequence));
    }
};

#endif