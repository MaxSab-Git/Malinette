#pragma once

#include "Parser.h"

namespace mali
{
    class TestContextParser : public Parser
    {
    public:
        TestContextParser() = default;

        bool operator()(ParserState& state) override;
    private:
        
    };
}