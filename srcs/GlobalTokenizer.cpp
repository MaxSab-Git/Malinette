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
        int line = tokens.empty() ? 1 : tokens.back().line + 1;

        std::streampos tpos = stream.tellg();
        c = skipChar(stream, '\n');
        line += stream.tellg() - tpos - 1;
        c = getChar(stream, token, c, isTestName);
        if (trim(token).empty() || c != ':')
            return false;
        tokens.emplace_back(token, "testName", line);
        token.clear();

        c = getChar(stream, token, skipChar(stream, ' '), isTestName);
        if (trim(token).empty() || c != '\n')
            return false;
        tokens.emplace_back(token, "testContext", line);
        token.clear();

        return true;
    }

    bool GlobalTokenizer::isTestName(char c)
    {
        return c != ':' && isPrintable(c);
    }
}