#pragma once

#include "Parser.h"
#include <utility>
#include <Test.h>

namespace mali
{
    class LaunchFunctionParser : public Parser
    {
    public:
        LaunchFunctionParser() = default;

        bool operator()(ParserState& state) override;
    private:
        static int launchEcho(const Task &args, const char *processPath, std::ostream &out, std::ostream &err, std::chrono::milliseconds timeout);
        static int launchRemove(const Task &args, const char *processPath, std::ostream &out, std::ostream &err, std::chrono::milliseconds timeout);

        static const std::pair<const char *, Command> s_supportedFunctions[];
    };
}