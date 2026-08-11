#include <Parser/FunctionParser.h>
#include <Parser/ParameterParser.h>
#include <Parser/LaunchFunctionParser.h>
#include <filesystem>

namespace mali
{
    const char FunctionParser::s_alphanum[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";

    std::default_random_engine FunctionParser::s_engine(static_cast<unsigned int>(std::chrono::high_resolution_clock::now().time_since_epoch().count()));
    std::uniform_int_distribution<int> FunctionParser::s_randprint(32, 126);
    std::uniform_int_distribution<int> FunctionParser::s_randalphanum(0, sizeof(s_alphanum) / sizeof(*s_alphanum) - 2);

    const std::pair<const char *, bool (*)(ParserState &state)> FunctionParser::s_supportedFunctions[] =
        {
            std::make_pair("compare", FunctionParser::doCompare),
            std::make_pair("launch", FunctionParser::doLaunch),
            std::make_pair("append", FunctionParser::doAppend),
            std::make_pair("loop", FunctionParser::doLoop),
            std::make_pair("end", FunctionParser::doEnd),
            std::make_pair("randprint", FunctionParser::doRandprint),
            std::make_pair("randalphanum", FunctionParser::doRandalphanum),
    };

    bool FunctionParser::operator()(ParserState &state)
    {
        for (std::size_t i = 0; i < sizeof(s_supportedFunctions) / sizeof(*s_supportedFunctions); i++)
        {
            if (state.getValue() == s_supportedFunctions[i].first)
                return s_supportedFunctions[i].second(state);
        }
        std::cerr << "Line " << state.getLine() << ": Invalid function: \"" << state.getValue() << "\"" << std::endl;
        return false;
    }

    bool FunctionParser::doCompare(ParserState &state)
    {
        state.setType(TaskType::Compare);
        state.next();
        return true;
    }

    bool FunctionParser::doLaunch(ParserState &state)
    {
        if (state.next() && state.checkTokenType("parameter"))
        {
            LaunchFunctionParser parser;
            if (!parser(state))
                return false;
        }
        else
        {
            std::cerr << "Line " << state.getPrevLine() << ": Missing parameter in function: \"" << state.getPrevValue() << "\"" << std::endl;
            return false;
        }
        return true;
    }

    bool FunctionParser::doAppend(ParserState &state)
    {
        if (state.next() && state.checkTokenType("variableName"))
        {
            std::string *toAppend = state.getVar(state.getValue());
            if (toAppend)
            {
                while (state.next())
                {
                    if (state.checkTokenType("variableName"))
                    {
                        const std::string *variable = state.getVar(state.getValue());
                        if (variable)
                            *toAppend += *variable;
                        else
                        {
                            std::cerr << "Line " << state.getLine() << ": Variable \"" << state.getValue() << "\" is not initialized." << std::endl;
                            return false;
                        }
                    }
                    else if (state.checkTokenType("controlArgument"))
                        *toAppend += state.getValue();
                    else
                        break;
                }
            }
            else
            {
                std::cerr << "Line " << state.getLine() << ": Variable \"" << state.getValue() << "\" is not initialized." << std::endl;
                return false;
            }
        }
        return true;
    }

    bool FunctionParser::doLoop(ParserState &state)
    {
        ParameterParser parser(2);
        if (nextAndParse(state, parser))
            state.next();
        state.pushLoop(parser.getValue());
        return true;
    }

    bool FunctionParser::doEnd(ParserState &state)
    {
        if (!state.popLoop())
        {
            std::cerr << "Line " << state.getLine() << ": No 'loop' before end." << std::endl;
            return false;
        }
        return true;
    }

    bool FunctionParser::doRandprint(ParserState &state)
    {
        ParameterParser parser(1);
        if (state.next() && (!parser(state) || state.next()) && state.checkTokenType("variableName"))
        {
            std::string newString;
            for (int j = 0; j < parser.getValue(); j++)
                newString.push_back((char)s_randprint(s_engine));
            state.setVar(newString);
            state.next();
        }
        else
        {
            std::cerr << "Line " << state.getPrevLine() << ": Missing argument in function: \"" << state.getPrevValue() << "\"" << std::endl;
            return false;
        }
        return true;
    }

    bool FunctionParser::doRandalphanum(ParserState &state)
    {
        ParameterParser parser(1);
        if (state.next() && (!parser(state) || state.next()) && state.checkTokenType("variableName"))
        {
            std::string newString;
            for (int j = 0; j < parser.getValue(); j++)
                newString.push_back(s_alphanum[s_randalphanum(s_engine)]);
            state.setVar(newString);
            state.next();
        }
        else
        {
            std::cerr << "Line " << state.getPrevLine() << ": Missing argument in function: \"" << state.getPrevValue() << "\"" << std::endl;
            return false;
        }
        return true;
    }
}