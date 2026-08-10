#include <Parser/GetterParser.h>
#include <filesystem>

namespace mali
{
    const std::pair<const char *, bool (*)(ParserState &state)> GetterParser::s_supportedFunctions[] =
        {
            std::make_pair("get", GetterParser::doGet),
            std::make_pair("malifile", GetterParser::doMalifile),
            std::make_pair("concat", GetterParser::doConcat),
    };

    bool GetterParser::operator()(ParserState &state)
    {
        for (std::size_t i = 0; i < sizeof(s_supportedFunctions) / sizeof(*s_supportedFunctions); i++)
        {
            if (state.getValue() == s_supportedFunctions[i].first)
                return s_supportedFunctions[i].second(state);
        }
        std::cerr << "Line " << state.getLine() << ": Invalid getter: \"" << state.getValue() << "\"" << std::endl;
        return false;
    }

    bool GetterParser::doGet(ParserState &state)
    {
        if (state.next() && state.checkTokenType("variableName"))
        {
            const std::string *variable = state.getVar(state.getValue());
            if (variable)
            {
                state.addArg(*variable);
                state.next();
                return true;
            }
            else
                std::cerr << "Line " << state.getLine() << ": Variable \"" << state.getValue() << "\" is not initialized." << std::endl;
        }
        else
            std::cerr << "Line " << state.getPrevLine() << ": Missing argument in getter: \"" << state.getPrevValue() << "\"" << std::endl;
        return false;
    }

    bool GetterParser::doMalifile(ParserState &state)
    {
        if (state.next() && state.checkTokenType("controlArgument"))
        {
            if (std::filesystem::is_regular_file(state.getValue()))
            {
                std::filesystem::path path = std::filesystem::relative(state.getValue(), state.getTestRootPath());
                state.addArg(path.string());
                state.next();
                return true;
            }
            else
                std::cerr << "Line " << state.getLine() << ": " << state.getValue() << " is not a file." << std::endl;
        }
        return true;
    }

    bool GetterParser::doConcat(ParserState &state)
    {
        std::string result;
        while (state.next())
        {
            if (state.checkTokenType("variableName"))
            {
                const std::string *variable = state.getVar(state.getValue());
                if (variable)
                    result.append(*variable);
                else
                {
                    std::cerr << "Line " << state.getLine() << ": Variable \"" << state.getValue() << "\" is not initialized." << std::endl;
                    return false;
                }
            }
            else if (state.checkTokenType("controlArgument"))
                result.append(state.getValue());
            else
                break;
        }
        state.addArg(result);
        return true;
    }
}
