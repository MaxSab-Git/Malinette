#include <Parser/ParameterParser.h>

namespace mali
{
    ParameterParser::ParameterParser() : m_value(1)
    {
    }

    ParameterParser::ParameterParser(int value) : m_value(value)
    {
    }

    bool ParameterParser::operator()(ParserState &state)
    {
        if (state.checkTokenType("parameter"))
        {
            m_value = std::atoi(state.getValue().c_str());
            return true;
        }
        return false;
    }

    int ParameterParser::getValue() const
    {
        return m_value;
    }
}
