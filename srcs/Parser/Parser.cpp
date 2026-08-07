#include <Parser/Parser.h>

bool mali::Parser::nextAndParse(ParserState &state, Parser &parser)
{
    return state.next() && parser(state);
}