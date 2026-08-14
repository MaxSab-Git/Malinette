#include <Tokenizer/Tokenizer.h>

namespace mali
{
    Tokenizer::Tokenizer() : m_context("unknown")
    {

    }

    Tokenizer::~Tokenizer()
    {
    }

    const char *mali::Tokenizer::getContext() const
    {
        return m_context;
    }

    char Tokenizer::skipChar(std::istream &stream, char skip)
    {
        char c = -1;
        while (stream.read(&c, 1) && c == skip)
            ;
        return c;
    }

    char Tokenizer::skipChar(std::istream &stream, bool (*pred)(char))
    {
        char c = -1;
        while (stream.read(&c, 1) && pred(c))
            ;
        return c;
    }

    char Tokenizer::getCharUntil(std::istream &stream, std::string &token, char start, char stop)
    {
        if (stream && start != stop)
        {
            do
            {
                token.push_back(start);
            } while (stream.read(&start, 1) && start != stop);
        }
        return start;
    }

    char Tokenizer::getChar(std::istream &stream, std::string& token, char start, bool (*pred)(char))
    {
        if (stream && pred(start))
        {
            do
            {
                token.push_back(start);
            } while (stream.read(&start, 1) && pred(start));
        }
        return start;
    }

    bool Tokenizer::isAlpha(char c)
    {
        return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
    }

    bool Tokenizer::isNumeric(char c)
    {
        return (c >= '0' && c <= '9') || c == '-';
    }
    bool Tokenizer::isAlphaNumeric(char c)
    {
        return isAlpha(c) || isNumeric(c);
    }
    bool Tokenizer::isPrintable(char c)
    {
        return (c >= 32 && c <= 126);
    }

    std::string &Tokenizer::trim(std::string &str)
    {
        std::size_t pos = str.find_last_not_of(' ');
        if (pos == std::string::npos)
            str.clear();
        else
            str.erase(pos + 1);
        return str;
    }

    Token::Token(const std::string &value, const char *type, int line) : value(value), type(type), line(line)
    {
    }
}