#pragma once

#include "TestContextParser.h"

namespace mali
{
    class TestNameParser : public Parser
    {
    public:
        TestNameParser() = default;

        bool operator()(ParserState &state) override;
    private:
    };
}