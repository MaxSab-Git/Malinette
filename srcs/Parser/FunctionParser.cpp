#include <Parser/FunctionParser.h>
#include <Parser/ParameterParser.h>
#include <Parser/LaunchFunctionParser.h>
#include <filesystem>

namespace mali
{
    const char FunctionParser::s_alphanum[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";

    std::default_random_engine FunctionParser::s_engine(static_cast<unsigned int>(std::chrono::high_resolution_clock::now().time_since_epoch().count()));

    const std::pair<const char *, bool (*)(ParserState &state)> FunctionParser::s_supportedFunctions[] =
        {
            std::make_pair("compare", FunctionParser::doCompare),
            std::make_pair("launch", FunctionParser::doLaunch),
            std::make_pair("append", FunctionParser::doAppend),
            std::make_pair("timeout", FunctionParser::doTimeout),
            std::make_pair("loop", FunctionParser::doLoop),
            std::make_pair("end", FunctionParser::doEnd),
            std::make_pair("randint", FunctionParser::doRandint),
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

    bool FunctionParser::doTimeout(ParserState &state)
    {
        ParameterParser parser(3000);
        if (nextAndParse(state, parser))
            state.next();
        state.setTimeout(std::chrono::milliseconds(parser.getValue()));
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

    bool FunctionParser::doRandint(ParserState &state)
    {
        ParameterParser parserMin(std::numeric_limits<int>::min());
        ParameterParser parserMax(std::numeric_limits<int>::max());
        const std::string &funcName = state.getValue();
        int funcLine = state.getLine();
        if (state.next())
        {
            if (parserMin(state))
            {
                if (!(state.next() && parserMax(state) && state.next()) || parserMin.getValue() > parserMax.getValue())
                {
                    std::cerr << "Line " << funcLine << ": Bad parameters in function: \"" << funcName << "\"" << std::endl;
                    return false;
                }
            }

            if (state.checkTokenType("variableName"))
            {
                std::uniform_int_distribution<int> randint(parserMin.getValue(), parserMax.getValue());
                state.setVar(std::to_string(randint(s_engine)));
                state.next();
            }
            else
            {
                std::cerr << "Line " << state.getPrevLine() << ": Missing argument in function: \"" << state.getPrevValue() << "\"" << std::endl;
                return false;
            }
        }
        else
        {
            std::cerr << "Line " << state.getPrevLine() << ": Missing argument in function: \"" << state.getPrevValue() << "\"" << std::endl;
            return false;
        }
        return true;
    }

    bool FunctionParser::doRandprint(ParserState &state)
    {
        ParameterParser parser(1);
        if (state.next() && (!parser(state) || state.next()) && state.checkTokenType("variableName"))
        {
            std::uniform_int_distribution<int> randprint(32, 126);
            std::string newString;
            for (int j = 0; j < parser.getValue(); j++)
                newString.push_back((char)randprint(s_engine));
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
            std::uniform_int_distribution<int> randalphanum(0, sizeof(s_alphanum) / sizeof(*s_alphanum) - 2);
            std::string newString;
            for (int j = 0; j < parser.getValue(); j++)
                newString.push_back(s_alphanum[randalphanum(s_engine)]);
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