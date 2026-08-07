#include <Parser/FunctionParser.h>
#include <Parser/ParameterParser.h>

namespace mali
{
    const char FunctionParser::s_alphanum[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";

    std::default_random_engine FunctionParser::s_engine(static_cast<unsigned int>(std::chrono::high_resolution_clock::now().time_since_epoch().count()));
    std::uniform_int_distribution<char> FunctionParser::s_randprint(32, 126);
    std::uniform_int_distribution<int> FunctionParser::s_randalphanum(0, sizeof(s_alphanum) / sizeof(*s_alphanum) - 1);

    const std::pair<const char *, bool (*)(ParserState &state)> FunctionParser::s_supportedFunctions[] =
        {
            std::make_pair("compare", FunctionParser::doCompare),
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

    bool mali::FunctionParser::doCompare(ParserState &state)
    {
        state.setType(TaskType::Compare);
        state.next();
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
                newString.push_back(s_randprint(s_engine));
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