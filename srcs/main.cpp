#include <iostream>
#include <fstream>

#include <random>
#include <chrono>

#include <vector>
#include <map>
#include <stack>
#include <filesystem>
#include <string>
#include <cstring>

#include <Test.h>
#include <Tokenizer/GlobalTokenizer.h>
#include <Tokenizer/TestCommandTokenizer.h>
#include <Parser/TestNameParser.h>
#include <Parser/ParserState.h>

std::vector<mali::Test> createTaskFromToken(const std::vector<mali::Token> &tokens)
{
    std::vector<mali::Test> tests;
    mali::ParserState state(tokens.cbegin(), tokens.cend());
    mali::TestNameParser parser;
    while (state.ok())
    {
        if (!parser(state))
        {
            return tests;
        }
        tests.emplace_back(state.getTest());
        state.reset();
    }
    return tests;
}

int has_option(const char *option, int ac, char **av)
{
    for (int i = 1; i < ac; i++)
    {
        if (std::strcmp(option, av[i]) == 0)
        {
            return i;
        }
    }
    return 0;
}

int main(int ac, char **av)
{
    struct
    {
        bool debugTypes = false;
        bool debugValues = false;
    } options;

    options.debugTypes = has_option("-DTypes", ac, av);
    options.debugValues = has_option("-DValues", ac, av);

    mali::GlobalTokenizer gt;
    mali::TestCommandTokenizer tct;

    std::ifstream file;
    for (int i = 1; i < ac; i++)
    {
        if (av[i][0] != '-')
        {
            file.open(av[i]);
            if (!file)
            {
                std::cerr << "Invalid file : " << av[i] << std::endl;
                return -1;
            }
            std::filesystem::current_path(std::filesystem::path(av[i]).parent_path());
        }
    }
    if (!file.is_open())
    {
        std::cerr << "No file specified. " << std::endl;
        return -1;
    }

    std::vector<mali::Token> tokens;
    while (std::strcmp(tct.getContext(), "bad") != 0 && gt.tokenize(file, tokens))
    {
        std::size_t i = tokens.size();
        while (tct.tokenize(file, tokens))
        {
            if (std::strcmp(tokens[i].type, "command") == 0)
            {
                // tokens.emplace_back("commandEnd", "implicit");
            }
            i = tokens.size();
        }
    }

    if ((options.debugTypes || options.debugValues) && tokens.size() > 0)
    {
        for (std::size_t i = 0; i < tokens.size(); i++)
        {
            std::cout << "<" << tokens[i].line << " ";
            if (options.debugTypes)
                std::cout << tokens[i].type;
            if (options.debugValues && options.debugTypes)
                std::cout << "=";
            if (options.debugValues)
                std::cout << tokens[i].value;
            std::cout << ">";

            if (i < tokens.size() - 1)
            {
                if (tokens[i].line != tokens[i + 1].line)
                    std::cout << "\n";
            }
        }
        std::cout << "\n"
                  << std::endl;
    }
    if (file)
    {
        std::cerr << "Syntax error around line " << (tokens.empty() ? 0 : tokens.back().line) << "." << std::endl;
        return -1;
    }
    file.close();

    std::vector<mali::Test> tests = createTaskFromToken(tokens);
    for (const mali::Test &test : tests)
    {
        int ret = test.run();
        if (ret != 0)
        {
            std::cout << "Result: KO: error: " << ret << "\n"
                      << std::endl;
        }
        else
        {
            std::cout << "Result: OK\n"
                      << std::endl;
        }
    }

    return 0;
}