#include <Parser/TestContextParser.h>
#include <Parser/InstructionParser.h>

namespace mali
{
    bool TestContextParser::operator()(ParserState &state)
    {
        if (state.checkTokenType("testContext"))
        {
            if (state.setTestRootPath())
            {
                InstructionParser parser;
                return nextAndParse(state, parser);
            }
            std::cerr << "Line " << state.getLine() << ": Invalid directory context: \"" << state.getValue() << "\"" << std::endl;
        }
        else
            std::cerr << "Line " << state.getPrevLine() << ": Missing directory context." << std::endl;
        return false;
    }
}