#include <iostream>
#include <fstream>

#include <random>
#include <chrono>

#include <vector>
#include <map>
#include <filesystem>
#include <string>
#include <cstring>

#include <Test.h>
#include <GlobalTokenizer.h>
#include <TestCommandTokenizer.h>

static std::default_random_engine engine(static_cast<unsigned int>(std::chrono::high_resolution_clock::now().time_since_epoch().count()));
static std::uniform_int_distribution<char> randprint(32, 126);

std::vector<mali::Test> createTaskFromToken(const std::vector<mali::Token>& tokens)
{
    std::vector<mali::Test> tests;
    mali::Task task;
    mali::TaskType type = mali::TaskType::Preparation;
    std::map<std::string, std::string> variables;
    for (std::size_t i = 0; i < tokens.size(); i++)
    {
        if (std::strcmp(tokens[i].type, "testName") == 0)
        {
            if (!task.empty())
                tests.back().addTask(std::move(task), type);
            task = mali::Task();
            type = mali::TaskType::Preparation;
            tests.emplace_back(tokens[i].value.c_str());
            variables.clear();
        }
        else if (std::strcmp(tokens[i].type, "testContext") == 0)
        {
            tests.back().setRootPath(tokens[i].value.c_str());
        }
        else if (std::strcmp(tokens[i].type, "function") == 0)
        {
            if (!task.empty())
            {
                tests.back().addTask(std::move(task), type);
                task = mali::Task();
                type = mali::TaskType::Preparation;
            }
            if (tokens[i].value == "compare")
            {
                type = mali::TaskType::Compare;
            }
            else if (tokens[i].value == "randprint")
            {
                int stringSize = std::strcmp(tokens[++i].type, "parameter") == 0 ? std::atoi(tokens[i++].value.c_str()) : 1;
                if (std::strcmp(tokens[i].type, "controlArgument") == 0)
                {
                    std::string newString;
                    for (int j = 0; j < stringSize; j++)
                        newString.push_back(randprint(engine));
                    variables[tokens[i].value] = newString;
                }
            }
        }
        else if (std::strcmp(tokens[i].type, "getter") == 0)
        {
            if (tokens[i].value == "get")
            {
                if (std::strcmp(tokens[++i].type, "controlArgument") == 0)
                {
                    auto it = variables.find(tokens[i].value);
                    if (it != variables.end())
                    {
                        task.emplace_back(it->second);
                    }
                }
            }
        }
        else if (std::strcmp(tokens[i].type, "command") == 0)
        {
            if (!task.empty())
            {
                tests.back().addTask(std::move(task), type);
                task = mali::Task();
                type = mali::TaskType::Preparation;
            }
            task.emplace_back(tokens[i].value);
        }
        else if (std::strcmp(tokens[i].type, "commandArgument") == 0)
        {
            task.emplace_back(tokens[i].value);
        }
    }
    return tests;
}

int main(int ac, char **av)
{
    if (ac != 2)
        return 0;

    mali::GlobalTokenizer gt;
    mali::TestCommandTokenizer tct;

    std::ifstream file(av[1]);
    std::vector<mali::Token> tokens;
    while (gt.tokenize(file, tokens))
    {
        while (tct.tokenize(file, tokens))
            ;
    }

    std::vector<mali::Test> tests = createTaskFromToken(tokens);

    if (tests.empty())
    {
        std::cout << "Oh Oh !" << std::endl;
        return 0;
    }

    for (const mali::Test& test : tests)
    {
        int ret = test.run();
        if (ret != 0)
            std::cout << "Result: KO: error: " << ret << "\n" << std::endl;
        else
            std::cout << "Result: OK\n" << std::endl;
    }

    return 0;
}