#pragma once

#include <string>
#include <vector>
#include <iostream>

namespace mali
{
    struct Token
    {
        Token(const std::string& value, const char *type);

        std::string value;
        const char *type;
    };

    class Tokenizer
    {
    public:
        Tokenizer();
        virtual ~Tokenizer();

        virtual bool tokenize(std::istream& stream, std::vector<Token>& tokens) = 0;
        const char *getContext() const;

        static char skipChar(std::istream &stream, char skip);
        static char skipChar(std::istream &stream, bool (*pred)(char));
        static char getCharUntil(std::istream &stream, std::string &token, char start, char stop);
        static char getChar(std::istream &stream, std::string &token, char start, bool (*pred)(char));

        static bool isAlpha(char c);
        static bool isNumeric(char c);
        static bool isAlphaNumeric(char c);
        static bool isPrintable(char c);

        static std::string &trim(std::string &str);

    protected:
        const char *m_context;
    };
}