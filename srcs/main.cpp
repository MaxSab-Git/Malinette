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
#include <GlobalTokenizer.h>
#include <TestCommandTokenizer.h>

static std::default_random_engine engine(static_cast<unsigned int>(std::chrono::high_resolution_clock::now().time_since_epoch().count()));
static std::uniform_int_distribution<char> randprint(32, 126);

using TokenIt = std::vector<mali::Token>::const_iterator;

struct TokenParser
{
    mali::Test test;
    mali::Task task = mali::Task();
    mali::TaskType type = mali::TaskType::Preparation;
    std::map<std::string, std::string> variables = std::map<std::string, std::string>();
    std::stack<std::pair<TokenIt, int>> flowControl = std::stack<std::pair<TokenIt, int>>();
};

bool parameterParser(TokenIt &it, const TokenIt &end)
{
    return it != end && std::strcmp(it->type, "parameter") == 0;
}

bool controlArgumentParser(TokenIt &it, const TokenIt &end)
{
    return it != end && std::strcmp(it->type, "controlArgument") == 0;
}

bool getterParser(TokenParser &parser, TokenIt &it, const TokenIt &end)
{
    if (it->value == "get")
    {
        if (controlArgumentParser(++it, end))
        {
            auto variable = parser.variables.find(it->value);
            if (variable != parser.variables.end())
            {
                parser.task.emplace_back(variable->second);
            }
            else
            {
                std::cerr << "Line " << it->line << ": Variable \"" << it->value << "\" is not initialized." << std::endl;
                return false;
            }
        }
        else
        {
            std::cerr << "Line " << (it - 1)->line << ": Missing argument in getter: \"" << (it - 1)->value << "\"" << std::endl;
            return false;
        }
    }
    else
    {
        std::cerr << "Line " << it->line << ": Invalid getter: \"" << it->value << "\"" << std::endl;
        return false;
    }
    return true;
}

bool commandParser(TokenParser &parser, TokenIt &it, const TokenIt &end)
{
    parser.task.emplace_back(it->value);
    while (++it != end)
    {
        if (std::strcmp(it->type, "getter") == 0)
        {
            if (!getterParser(parser, it, end))
                return false;
        }
        else if (std::strcmp(it->type, "commandArgument") == 0)
            parser.task.emplace_back(it->value);
        else
            break;
    }
    parser.test.addTask(std::move(parser.task), parser.type);
    parser.task = mali::Task();
    parser.type = mali::TaskType::Preparation;
    return true;
}

bool functionParser(TokenParser &parser, TokenIt &it, const TokenIt &end)
{
    if (it->value == "compare")
    {
        parser.type = mali::TaskType::Compare;
        it++;
    }
    else if (it->value == "end")
    {
        if (parser.flowControl.empty())
        {
            std::cerr << "Line " << it->line << ": No 'loop' before end." << std::endl;
            return false;
        }
        if (--parser.flowControl.top().second > 0)
        {
            it = parser.flowControl.top().first;
        }
        else
        {
            parser.flowControl.pop();
            it++;
        }
    }
    else if (it->value == "loop")
    {
        int loopCount = 2;
        if (parameterParser(++it, end))
        {
            loopCount = std::atoi(it->value.c_str());
            ++it;
        }
        parser.flowControl.emplace(it, loopCount);
    }
    else if (it->value == "randprint")
    {
        int stringSize = 1;
        if (parameterParser(++it, end))
        {
            stringSize = std::atoi(it->value.c_str());
            ++it;
        }
        if (controlArgumentParser(it, end))
        {
            std::string newString;
            for (int j = 0; j < stringSize; j++)
                newString.push_back(randprint(engine));
            parser.variables[it->value] = newString;
            ++it;
        }
        else
        {
            std::cerr << "Line " << (it - 1)->line << ": Missing argument in function: \"" << (it - 1)->value << "\"" << std::endl;
            return false;
        }
    }
    else
    {
        std::cerr << "Line " << it->line << ": Invalid function: \"" << it->value << "\"" << std::endl;
        return false;
    }
    return true;
}

bool instructionParser(TokenParser &parser, TokenIt &it, const TokenIt &end)
{
    while (it != end)
    {
        if (std::strcmp(it->type, "function") == 0)
        {
            if (!functionParser(parser, it, end))
                return false;
        }
        else if (std::strcmp(it->type, "command") == 0)
        {
            if (!commandParser(parser, it, end))
                return false;
        }
        else
            break;
    }
    return true;
}

bool testContextParser(TokenParser &parser, TokenIt &it, const TokenIt &end)
{
    if (it != end && std::strcmp(it->type, "testContext") == 0)
    {
        if (std::filesystem::is_directory(it->value) && std::filesystem::exists(it->value))
        {
            parser.test.setRootPath(it->value.c_str());
            return instructionParser(parser, ++it, end);
        }
        else
            std::cerr << "Line " << it->line << ": Invalid directory context: \"" << it->value << "\"" << std::endl;
    }
    else
        std::cerr << "Line " << (it - 1)->line << ": Missing directory context." << std::endl;

    return false;
}

std::vector<mali::Test> testNameParser(TokenIt &it, const TokenIt &end)
{
    std::vector<mali::Test> tests;
    while (it != end && std::strcmp(it->type, "testName") == 0)
    {
        TokenParser parser{.test = mali::Test(it->value.c_str())};
        if (!testContextParser(parser, ++it, end))
        {
            return std::vector<mali::Test>();
        }
        tests.emplace_back(parser.test);
    }
    return tests;
}

std::vector<mali::Test> createTaskFromToken(const std::vector<mali::Token> &tokens)
{
    TokenIt it = tokens.cbegin();
    return testNameParser(it, tokens.cend());
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

    std::ifstream file("");
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
        }
    }
    if (!file)
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
                if (!(std::strcmp(tokens[i + 1].type, "testContext") == 0 && std::strcmp(tokens[i].type, "testName") == 0) && !((std::strcmp(tokens[i + 1].type, "commandArgument") == 0 || std::strcmp(tokens[i + 1].type, "getter") == 0) && (std::strcmp(tokens[i].type, "command") == 0 || std::strcmp(tokens[i].type, "commandArgument") == 0)) && !((std::strcmp(tokens[i + 1].type, "parameter") == 0 || std::strcmp(tokens[i + 1].type, "controlArgument") == 0) && (std::strcmp(tokens[i].type, "function") == 0 || std::strcmp(tokens[i].type, "getter") == 0 || std::strcmp(tokens[i].type, "parameter") == 0)) && !(std::strcmp(tokens[i + 1].type, "implicit") == 0 && tokens[i + 1].value == "commandEnd"))
                {
                    std::cout << "\n";
                }
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