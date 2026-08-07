#include <Parser/CommandParser.h>
#include <Parser/GetterParser.h>

namespace mali
{
    bool CommandParser::operator()(ParserState &state)
    {
        while (state.ok())
        {
            if (state.checkTokenType("getter"))
            {
                GetterParser parser;
                if (!parser(state))
                    return false;
            }
            else if (state.checkTokenType("commandArgument"))
            {
                state.addArg();
                state.next();
            }
            else
                break;
        }
        state.pushCommand();
        return true;
    }
}