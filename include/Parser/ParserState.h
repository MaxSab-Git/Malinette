#pragma once

#include <map>
#include <stack>
#include <Tokenizer/Tokenizer.h>
#include <Test.h>

namespace mali
{
    using TokenIt = std::vector<mali::Token>::const_iterator;

    class ParserState
    {
    public:
        ParserState(TokenIt begin, TokenIt end);

        const Test& getTest();
        void reset();

        bool ok() const;
        bool next();
        bool checkTokenType(const char *type) const;

        int getLine() const;
        int getPrevLine() const;
        const std::string& getValue() const;
        const std::string& getPrevValue() const;
        const std::string& getTestRootPath() const;
        std::string *getVar(const std::string& name) const;

        void setTestName();
        bool setTestRootPath();
        void setType(TaskType type);
        void setPrintTestOut(bool printOut);
        void addArg();
        void addArg(const std::string& value);
        void pushCommand(Command internalCommand = nullptr);
        void pushLoop(int iterations);
        bool popLoop();
        void setVar(const std::string& value);

    private:
        mali::Test m_test;
        mali::Task m_task;
        mali::TaskType m_type;
        bool m_printTestOut;
        std::map<std::string, std::string> m_variables;
        std::map<std::string, std::string> m_specialVariables;
        std::stack<std::pair<TokenIt, int>> m_flowControl;
        TokenIt m_it;
        TokenIt m_end;
    };
}