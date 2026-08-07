#pragma once

#include "Parser.h"
#include <utility>
#include <random>
#include <chrono>

namespace mali
{
    class GetterParser : public Parser
    {
    public:
        GetterParser() = default;

        bool operator()(ParserState& state) override;
    private:
        static bool doGet(ParserState& state);
        static bool doMalifile(ParserState& state);

        static const std::pair<const char *, bool (*)(ParserState& state)> s_supportedFunctions[];
    };
}