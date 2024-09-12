#include "pregex.h"

PregexSequence::PregexSequence(std::string match_string, std::string token) : m_token(token), m_error(false), m_ptr(0) {
    // Build a sequence by first creating all nodes and modifiers
    build_sequence(match_string);

    // And then processing the modifiers on the nodes.
    for (auto & mod : m_modifiers) mod->apply(m_sequence);
}

bool PregexSequence::match(char c){ // Need to implement exploration of multiple paths
    auto & current_node = m_sequence.at(m_ptr);
    auto it = current_node->m_transitions.find(c);
    if (it != current_node->m_transitions.end()){
        m_ptr = it->second.front();
        return m_sequence.at(m_ptr)->m_is_end;
    } else m_ptr = 0;
    return false;
}

void PregexSequence::build_sequence(std::string match_string) {
    int current_node = 0;

    int context_start = 0;
    std::stack<int> context_stack;

    bool escape = false;

    for (int i = 0; i < match_string.size(); i++){
        char current_char = match_string[i];
        char c = match_string[i];

        if (c == '('){ // Create a context.
            context_stack.push(current_node);
        } else 
        if (c == '/'){ // Escape the next Modifier.
            escape = true;
            continue;
        } else
        if (c == ')'){ // Close a context
            context_start = context_stack.top();
            context_stack.pop();
            continue;
        } else
        if (c == '?' && !escape){ // Apply a ? modifier
            auto modifier = new PregexModifier{.optional=true, .begin=context_start, .end=current_node};
            m_modifiers.push_back( std::unique_ptr<PregexModifier>(modifier) );
        } else
        if (c == '+' && !escape){ // Apply a + modifier
            auto modifier = new PregexModifier{.recursive=true, .begin=context_start, .end=current_node};
            m_modifiers.push_back( std::unique_ptr<PregexModifier>(modifier) );
        } else
        if (c == '*' && !escape){ // Apply a * modifier
            auto modifier = new PregexModifier{.optional=true, .recursive=true, .begin=context_start, .end=current_node};
            m_modifiers.push_back( std::unique_ptr<PregexModifier>(modifier) );
        } else { // Build a new node
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
        }
        context_start=current_node-1;
        escape = false;
    }
    auto new_node = std::unique_ptr<PregexNode>(new PregexNode{ .m_node_number=current_node, .m_is_end=true});
    m_sequence.push_back( std::move(new_node) );
}

std::optional<std::string> ParallelRegex::match_character(char c){
    // Requirements for Parallel parsing //
    // 1) Expressions are in priority order. It should try to match the longest string in priority.
    // 2) Expressions should be able to become valid multiple times and hold their previous values,
    //    this is important for match items that have optionals. It should always match the longest
    //    expression possible, but should default back in case the expression is reset.
    // 3) Expressions are reset if all their branches conclude. Branch exploration should be able to
    //    occur simultaneously and should be reset independently. If all branches are at the same 
    //    point they should be merged back together. <-- this may result in more problems if I'm
    //    not careful.


    for (auto & expr : m_expressions){

    }
}