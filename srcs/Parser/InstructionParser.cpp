#include <Parser/InstructionParser.h>
#include <Parser/FunctionParser.h>
#include <Parser/CommandParser.h>

namespace mali
{
    bool InstructionParser::operator()(ParserState &state)
    {
        while(state.ok())
        {
            if (state.checkTokenType("function"))
            {
                FunctionParser parser;
                if (!parser(state))
                    return false;
            }
            else if (state.checkTokenType("command"))
            {
                CommandParser parser;
                if (!parser(state))
                    return false;
            }
            else
                break;
        }

        return true;
    }
}