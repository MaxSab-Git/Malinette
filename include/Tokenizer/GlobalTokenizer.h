#pragma once

#include "Tokenizer.h"

namespace mali
{
    class GlobalTokenizer : public Tokenizer
    {
    public:
        GlobalTokenizer();

        bool tokenize(std::istream &stream, std::vector<Token> &tokens) override;

        static bool isTestName(char c);
    };
}