#pragma once

#include "Parser.h"
#include <utility>
#include <random>
#include <chrono>

namespace mali
{
    class FunctionParser : public Parser
    {
    public:
        FunctionParser() = default;

        bool operator()(ParserState& state) override;
    private:
        static bool doCompare(ParserState& state);
        static bool doLaunch(ParserState& state);
        static bool doAppend(ParserState& state);
        static bool doLoop(ParserState& state);
        static bool doEnd(ParserState& state);
        static bool doRandprint(ParserState& state);
        static bool doRandalphanum(ParserState& state);

        static const std::pair<const char *, bool (*)(ParserState& state)> s_supportedFunctions[];
        static const char s_alphanum[];
        
        static std::default_random_engine s_engine;
        static std::uniform_int_distribution<int> s_randprint;
        static std::uniform_int_distribution<int> s_randalphanum;  
    };
}