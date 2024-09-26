#include "pregex.h"

PregexSequence::PregexSequence(
    std::vector<std::unordered_set<IN_T>> const & internal_transitions,
    std::vector<PregexModifier> const & modifiers, 
    OUT_T out_token)
    : m_out_token(out_token)
{
    // Construct all nodes
    int node_number = 0;
    for (auto item : internal_transitions){
        m_sequence.push_back( PregexNode(item, node_number++) );
    }
    // Construct final node
    m_sequence.push_back( PregexNode( std::unordered_set<IN_T>{}, node_number, true ) );

    // Apply modifiers to nodes
    for (auto mod : modifiers){
        auto & end_node = m_sequence.at(mod.context_end);
        auto & begin_node = m_sequence.at(mod.context_start);
        if (mod.optional){
            begin_node |= end_node;
            if (end_node != m_sequence.back())  begin_node += end_node;
        }
        if (mod.recursive){
            m_sequence.at(mod.context_end) += m_sequence.at(mod.context_start);
        }
    }
    init_ptrs();
}

// ---*--- Only when IN_T is a char does this apply. ---*--- //

void PregexSequence::build_character_set(char begin, char end, std::unordered_set<char> & set ){
    for (char i = begin; i <= end; i++) set.insert(i);
}

std::pair<std::vector<PregexModifier>, std::vector<std::unordered_set<char>>> PregexSequence::parse_input_text(std::string text){
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
                characters.push_back(items);
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
                    modifiers.push_back( 
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
                characters.push_back( std::unordered_set<char>{c} );

                begin = end;
                end++;
            } break;
        }
    }
    return std::make_pair( modifiers, characters );
}

void Pregex::add_char_sequence(std::string match_string, std::string out_token){
    auto v = PregexSequence::parse_input_text(match_string);
    PregexSequence sequence(v.second, v.first, out_token);
    sequence.print_sequence();
    m_sequences.push_back(sequence);
}

// ---*---- Char only section over ---*--- //

MatchObject PregexSequence::match_input(IN_T token){
    MatchObject output;
    std::queue<MatchPtr> new_ptrs;

    while (m_ptrs.size()){
        auto front_element = m_ptrs.front();
        m_ptrs.pop();

        auto front_node = m_sequence.at(front_element.location);
        auto front_transitions = front_node.m_transitions;
        auto front_find_token = front_transitions.find(token);

        // This node is dead end. Process possible matches. SHOULD ONLY GET ONE!
        if (front_find_token == front_transitions.end()) {
            // This is a finalising match
            if (front_node.m_is_end) {
                output.token_sequence = front_element.full_match_tokens;
                output.out_token = m_out_token;

                new_ptrs.push( MatchPtr{} );
                break;
            }

            // Create a new start pointer
            continue;
        }
        // Add the new token
        front_element.tokens.push_back(token);

        // We have found a match, go through all possible links:
        for (auto link : front_find_token->second){
            auto link_node_end = m_sequence.at(link).m_is_end;

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

MatchObject Pregex::match_token(IN_T in){
    MatchObject out;
    auto out_obj = m_sequences.at(0).match_input(in);
    std::cout << out_obj;
    return MatchObject{};
}