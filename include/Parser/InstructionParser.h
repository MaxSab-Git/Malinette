#pragma once

#include "Parser.h"

namespace mali
{
    class InstructionParser : public Parser
    {
    public:
        InstructionParser() = default;

        bool operator()(ParserState& state) override;
    private:
        
    };
}