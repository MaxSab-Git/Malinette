#pragma once

#include "Parser.h"

namespace mali
{
    class CommandParser : public Parser
    {
    public:
        CommandParser() = default;

        bool operator()(ParserState& state) override;
    private:
        
    };
}