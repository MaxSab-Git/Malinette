#pragma once

#include <Tokenizer.h>

namespace mali
{
    class TestCommandTokenizer : public Tokenizer
    {
    public:
        TestCommandTokenizer();

        bool tokenize(std::istream &stream, std::vector<Token> &tokens) override;

        static bool isCommand(char c);
    private:
        bool tokenizeControl(std::istream &stream, std::vector<Token> &tokens, const char *type, int line);
    };
}