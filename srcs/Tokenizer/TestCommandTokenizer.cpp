#include <Tokenizer/TestCommandTokenizer.h>
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
        int line = tokens.empty() ? 1 : tokens.back().line + 1;

        std::streampos tpos = stream.tellg();
        c = skipChar(stream, '\n');
        line += stream.tellg() - tpos - 1;
        if (c != '\t' || !stream.read(&c, 1))
        {
            m_context = "unchanged";
            stream.seekg(tpos, std::ios::beg);
            return false;
        }
        m_context = "bad";
        if (c == ':')
        {
            if (!tokenizeControl(stream, tokens, "function", line))
                return false;
            stream.read(&c, 1);
        }
        else
        {
            if (c == '\"')
            {
                stream.read(&c, 1);
                do
                {
                    c = getChar(stream, token, c, isString);
                    if (c != '\"')
                        return false;
                } while (stream.read(&c, 1) && c == '\"');
            }
            else
                c = getChar(stream, token, c, isCommand);
            if (token.empty() || !stream)
                return false;
            tokens.emplace_back(token, "command", line);
            token.clear();

            while (c == ' ')
            {
                if (!stream.read(&c, 1))
                    return false;
                if (c == ':')
                {
                    if (!tokenizeControl(stream, tokens, "getter", line))
                        return false;
                    stream.read(&c, 1);
                }
                else
                {
                    if (c == '\"')
                    {
                        stream.read(&c, 1);
                        do
                        {
                            c = getChar(stream, token, c, isString);
                            if (c != '\"')
                                return false;
                        } while (stream.read(&c, 1) && c == '\"');
                    }
                    else
                        c = getChar(stream, token, c, isCommand);
                    if (c == ':' || !stream)
                        return false;
                    tokens.emplace_back(token, "commandArgument", line);
                    token.clear();
                }
            }
        }
        if (c != '\n')
            return false;
        m_context = "good";
        return true;
    }

    bool TestCommandTokenizer::isCommand(char c)
    {
        return isPrintable(c) && c != ' ' && c != ':';
    }

    bool TestCommandTokenizer::isVariable(char c)
    {
        return isAlpha(c) || c == '_';
    }

    bool TestCommandTokenizer::isString(char c)
    {
        return isPrintable(c) && c != '\"';
    }

    bool TestCommandTokenizer::tokenizeControl(std::istream &stream, std::vector<Token> &tokens, const char *type, int line)
    {
        std::string token;
        char c;

        if (!stream.read(&c, 1))
            return false;
        c = getChar(stream, token, c, isAlpha);
        if (token.empty() || !stream)
            return false;
        tokens.emplace_back(token, type, line);
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
                tokens.emplace_back(token, "parameter", line);
                token.clear();
            } while (c == ',');
            if (c != ')' || !stream.read(&c, 1))
                return false;
        }

        while (c == ' ')
        {
            stream.read(&c, 1);
            if (c == '\"')
            {
                stream.read(&c, 1);
                do
                {
                    c = getChar(stream, token, c, isString);
                    if (c != '\"')
                        return false;
                } while (stream.read(&c, 1) && c == '\"');
                if (token.empty() || !stream)
                    return false;
                tokens.emplace_back(token, "controlArgument", line);
            }
            else
            {
                c = getChar(stream, token, c, isVariable);
                if (token.empty() || !stream)
                    return false;
                tokens.emplace_back(token, "variableName", line);
            }
            token.clear();
        }
        if (c != ':')
            return false;
        return true;
    }
}