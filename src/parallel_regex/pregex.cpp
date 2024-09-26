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
    m_ptrs.push_back(MatchPtr{});
}




// Char exclusive //

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





// MatchObject PregexSequence::match_input(IN_T token){
//     std::vector<MatchPtr> new_ptrs = { MatchPtr{} };
//     MatchObject out_obj{.out_token=m_out_token};
//     MatchPtr prev_matched;

//     for (auto ptr : m_ptrs){ // Iterate through all pointers
//         auto sequence_node = m_sequence.at(ptr.location); // Get the ptr location
//         auto ptr_transitions = sequence_node.m_transitions; // Get the pointer node transitions
//         auto find_it = ptr_transitions.find(token); // See if the match token is in the transitions
//         if (find_it == ptr_transitions.end()){ // We didn't find it, move on.
//             if (ptr.full_match_tokens.size() > prev_matched.full_match_tokens.size()) prev_matched = ptr;
//             continue; 
//         }
//         for (auto connection : find_it->second ){ // Go through all possible new transitions.
//             MatchPtr next_ptr{
//                 .tokens = ptr.tokens,
//                 .location = connection
//             }; // Valid transition, so make a new pointer

//             next_ptr.tokens.push_back(token); // Add the most recent token to the tokens list.
//             if ( m_sequence.at(connection).m_is_end ){
//                 next_ptr.full_match_tokens = next_ptr.tokens;
//                 out_obj.matched = true; // Used to indicate to KILL ALL NON-END NODES
//             } else {
//                 next_ptr.full_match_tokens = ptr.full_match_tokens;
//             }

//             new_ptrs.push_back(next_ptr); // Add to the new pointer list
//         }
//     }
//     if (out_obj.matched){
//         std::vector<MatchPtr> new_new_ptrs = { MatchPtr{} };
//         for (auto ptr : new_ptrs){
//             if (m_sequence.at(ptr.location).m_is_end){
//                 new_new_ptrs.push_back(ptr);
//             }
//         }
//         new_ptrs = new_new_ptrs;
//     }
//     std::cout << "OUT_OBJ: " << out_obj.out_token << std::endl;

//     if (new_ptrs.size() < 2) out_obj.token_sequence = prev_matched.full_match_tokens; // This bit doesnt work

//     m_ptrs = new_ptrs;

//     for (auto item : out_obj.token_sequence) {
//         std::cout << item;
//     }
//     std::cout << "\n " << out_obj.matched << "\n";

//     return out_obj;
// }

// MatchObject Pregex::match_token(IN_T in){
//     MatchObject out;
//     for (std::size_t i = 0; i < m_sequences.size(); ++i){
//         // Investigate why this happens?
//         auto output = m_sequences[i].match_input(in);
//         if (output.matched) {
//             reset_excl(i);
//             break;
//         }
//         if (output.token_sequence.size() != 0) out = output;
//     }
//     return out;
//     // If we get a match, reset and repeat other pregexes.
// }