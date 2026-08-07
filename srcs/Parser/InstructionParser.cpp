#include <Parser/InstructionParser.h>
#include <Parser/FunctionParser.h>
#include <Parser/CommandParser.h>
#include <Parser/GetterParser.h>

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
            else
            {
                if (state.checkTokenType("command"))
                {
                    state.addArg();
                    state.next();
                }
                else if (state.checkTokenType("commandGetter"))
                {
                    GetterParser parser;
                    if (!parser(state))
                        return false;
                }
                else
                    break;
                    
                CommandParser parser;
                if (!parser(state))
                    return false;
            }
        }

        return true;
    }
}