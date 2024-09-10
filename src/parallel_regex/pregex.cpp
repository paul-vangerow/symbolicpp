#include "pregex.h"

PregexSequence::PregexSequence(std::string match_string, std::string token) : m_token(token), m_error(false) {
    // Build a sequence by first creating all nodes and modifiers
    build_sequence(match_string);

    // And then processing the modifiers on the nodes.
    for (auto & mod : m_modifiers) mod->apply(m_sequence);

    print_nodes();
}

void PregexSequence::build_sequence(std::string match_string) {
    int current_node = 0;
    std::stack<int> context_stack;
    int context_start = 0;
    bool check_matches = false;

    for (int i = 0; i < match_string.size(); i++){
        char current_char = match_string.at(i);
        switch(current_char) {
            case '(': { // Create a context.
                context_stack.push(current_node);
                break;
            }
            case '/': { // Escape the next Modifier.
                std::cout << "VAL \n";
                current_char = match_string.at(++i);
            }
            default: { // Build a set or normal node
                auto new_node = std::unique_ptr<PregexNode>(new PregexNode{ .m_node_number=current_node++});
                if (current_char == '[') { // Build a set
                    for (int offset = i+4; i < match_string.size(); offset+=3){
                        if (match_string.at(offset-2) != '-') {
                            m_error = true;
                            return;
                        }
                        add_set( match_string.at(offset-3), match_string.at(offset-1), new_node );
                        if (match_string.at(offset) == ']') {
                            i = offset;
                            break;
                        } 
                    }
                } else { // Construct a normal node.
                    add_set(current_char, current_char, new_node);
                }
                m_sequence.push_back( std::move(new_node) );
                break;
            }
            case ')': { // Close a context.
                current_char = match_string.at(++i);
                context_start = context_stack.top();
                context_stack.pop();
                check_matches = true;
            }
            case '+': { // Add a modifier to the current context with 'Recursion' property
                if ( !check_matches || (check_matches && current_char == '+')) {
                    auto modifier = new PregexModifier{.recursive=true, .begin=context_start, .end=current_node};
                    m_modifiers.push_back( std::unique_ptr<PregexModifier>(modifier) );
                    break;
                }
            }
            case '?': { // Add a modifier to the current context with 'Optional' property
                if ( !check_matches || (check_matches && current_char == '?')) {
                    auto modifier = new PregexModifier{.optional=true, .begin=context_start, .end=current_node};
                    m_modifiers.push_back( std::unique_ptr<PregexModifier>(modifier) );
                    break;
                }
            }
            case '*': { // Add a modifier to the current context with 'Optional' and 'Recursion' properties.
                if ( !check_matches || (check_matches && current_char == '*')) {
                    auto modifier = new PregexModifier{.recursive=true, .optional=true, .begin=context_start, .end=current_node};
                    m_modifiers.push_back( std::unique_ptr<PregexModifier>(modifier) );
                    break;
                }
            }
        }
        check_matches = false;
        context_start=current_node-1;
    }
    auto new_node = std::unique_ptr<PregexNode>(new PregexNode{ .m_node_number=current_node, .m_is_end=true});
    m_sequence.push_back( std::move(new_node) );
}