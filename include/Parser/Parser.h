#pragma once

#include "ParserState.h"

namespace mali
{
    class Parser
    {
    public:
        Parser() = default;
        virtual ~Parser() = default;

        virtual bool operator()(ParserState& state) = 0;
    protected:
        static bool nextAndParse(ParserState& state, Parser& parser);
    private:
        
    };
}