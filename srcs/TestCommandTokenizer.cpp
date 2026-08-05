#include <TestCommandTokenizer.h>
#include <filesystem>

namespace mali
{
    TestCommandTokenizer::TestCommandTokenizer()
    {
    }

    bool TestCommandTokenizer::tokenize(std::istream &stream, std::vector<Token> &tokens)
    {
        std::string token;
        char c;

        c = skipChar(stream, '\n');
        if (c != '\t' || !stream.read(&c, 1))
        {
            stream.seekg(-1, std::ios::cur);
            return false;
        }
        if (c == ':')
        {
            if (!tokenizeControl(stream, tokens, "function"))
                return false;
            stream.read(&c, 1);
        }
        else
        {
            c = getChar(stream, token, c, isCommand);
            if (token.empty() || !stream)
                return false;
            tokens.emplace_back(token, "command");
            token.clear();

            while (c == ' ')
            {
                if (!stream.read(&c, 1))
                    return false;
                if (c == ':')
                {
                    if (!tokenizeControl(stream, tokens, "getter"))
                        return false;
                    stream.read(&c, 1);
                }
                else
                {
                    c = getChar(stream, token, c, isCommand);
                    if (token.empty() || !stream)
                        return false;
                    tokens.emplace_back(token, "commandArgument");
                    token.clear();
                }
            }
        }
        if (c != '\n')
            return false;
        return true;
    }

    bool TestCommandTokenizer::isCommand(char c)
    {
        return isPrintable(c) && c != ' ';
    }

    bool TestCommandTokenizer::tokenizeControl(std::istream &stream, std::vector<Token> &tokens, const char *type)
    {
        std::string token;
        char c;

        if (!stream.read(&c, 1))
            return false;
        c = getChar(stream, token, c, isAlpha);
        if (token.empty() || !stream)
            return false;
        tokens.emplace_back(token, type);
        token.clear();
        if (c == '(')
        {
            do
            {
                c = getChar(stream, token, skipChar(stream, ' '), isNumeric);
                if (c == ' ')
                    c = skipChar(stream, ' ');
                if (token.empty() || !stream)
                    return false;
                tokens.emplace_back(token, "parameter");
                token.clear();
            } while (c == ',');
            if (c != ')' || !stream.read(&c, 1))
                return false;
        }

        while (c == ' ')
        {
            stream.read(&c, 1);
            c = getChar(stream, token, c, isAlpha);
            if (token.empty() || !stream)
                return false;
            tokens.emplace_back(token, "controlArgument");
            token.clear();
        }
        if (c != ':')
            return false;
        return true;
    }
}