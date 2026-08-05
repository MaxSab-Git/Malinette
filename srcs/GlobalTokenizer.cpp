#include <GlobalTokenizer.h>

namespace mali
{
    GlobalTokenizer::GlobalTokenizer()
    {
    }

    bool GlobalTokenizer::tokenize(std::istream &stream, std::vector<Token> &tokens)
    {
        std::string token;
        char c;

        c = getChar(stream, token, skipChar(stream, '\n'), isTestName);
        if (trim(token).empty() || c != ':')
            return false;
        tokens.emplace_back(token, "testName");
        token.clear();

        c = getChar(stream, token, skipChar(stream, ' '), isTestName);
        if (trim(token).empty() || c != '\n')
            return false;
        tokens.emplace_back(token, "testContext");
        token.clear();

        return true;
    }

    bool GlobalTokenizer::isTestName(char c)
    {
        return c != ':' && isPrintable(c);
    }
}