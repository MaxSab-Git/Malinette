#include <Parser/TestContextParser.h>
#include <Parser/TestNameParser.h>

namespace mali
{
    bool TestNameParser::operator()(ParserState &state)
    {
        if (state.checkTokenType("testName"))
        {
            state.setTestName();
            TestContextParser parser;
            if (!nextAndParse(state, parser))
                return false;
        }
        else
            return false;
        return true;
    }
}