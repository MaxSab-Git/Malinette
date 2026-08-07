#pragma once

#include "Parser.h"

namespace mali
{
    class ParameterParser : public Parser
    {
    public:
        ParameterParser();
        ParameterParser(int value);

        bool operator()(ParserState& state) override;
        
        int getValue() const;
    private:
        int m_value;
    };
}