#include <Parser/LaunchFunctionParser.h>
#include <filesystem>
#include <iostream>
#include <string>

namespace mali
{
    const std::pair<const char *, Command> LaunchFunctionParser::s_supportedFunctions[] =
        {
            std::make_pair("echo", LaunchFunctionParser::launchEcho),
            std::make_pair("remove", LaunchFunctionParser::launchRemove),
    };

    bool LaunchFunctionParser::operator()(ParserState &state)
    {
        for (std::size_t i = 0; i < sizeof(s_supportedFunctions) / sizeof(*s_supportedFunctions); i++)
        {
            if (state.getValue() == s_supportedFunctions[i].first)
            {
                state.addArg("malinette-" + state.getValue());
                while (state.next())
                {
                    if (state.checkTokenType("variableName"))
                    {
                        const std::string *variable = state.getVar(state.getValue());
                        if (variable)
                            state.addArg(*variable);
                        else
                        {
                            std::cerr << "Line " << state.getLine() << ": Variable \"" << state.getValue() << "\" is not initialized." << std::endl;
                            return false;
                        }
                    }
                    else if (state.checkTokenType("controlArgument"))
                        state.addArg(state.getValue());
                    else
                        break;
                }
                state.pushCommand(s_supportedFunctions[i].second);
                return true;
            }
        }
        std::cerr << "Line " << state.getLine() << ": Invalid Malinette command: \"" << state.getValue() << "\"" << std::endl;
        return false;
    }

    int LaunchFunctionParser::launchEcho(const Task &args, const char *processPath, std::ostream &out, std::ostream &err)
    {
        (void)processPath;
        (void)err;

        for (std::size_t i = 1; i < args.size(); i++)
        {
            out << args[i];
        }
        out << "\r" << std::endl;
        return 0;
    }
}

int mali::LaunchFunctionParser::launchRemove(const Task &args, const char *processPath, std::ostream &out, std::ostream &err)
{
    (void)processPath;
    (void)out;

    for (std::size_t i = 1; i < args.size(); i++)
    {
        std::error_code code;
        std::filesystem::path path(args[i]);
        path = processPath / path;
        if (std::filesystem::is_regular_file(path) && !std::filesystem::remove(path, code))
        {
            err << code.message() << std::endl;
            return code.value();
        }
    }
    return 0;
}
