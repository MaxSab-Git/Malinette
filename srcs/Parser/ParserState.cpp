#include <Parser/ParserState.h>
#include <Test.h>
#include <cstring>
#include <filesystem>
#include <exception>
#include <stdexcept>

namespace mali
{
    ParserState::ParserState(TokenIt begin, TokenIt end) : m_type(TaskType::Preparation), m_printTestOut(false), m_it(begin), m_end(end)
    {
        m_specialVariables["malidir"] = "./";
    }

    const Test &ParserState::getTest()
    {
        return m_test;
    }

    void ParserState::reset()
    {
        m_test = mali::Test();
        m_task = mali::Task();
        m_type = TaskType::Preparation;
        m_variables.clear();
        m_flowControl = std::stack<std::pair<TokenIt, int>>();
    }

    bool ParserState::ok() const
    {
        return m_it != m_end;
    }

    bool ParserState::next()
    {
        return ++m_it != m_end;
    }

    bool ParserState::checkTokenType(const char *type) const
    {
        return std::strcmp(m_it->type, type) == 0;
    }

    int ParserState::getLine() const
    {
        return m_it->line;
    }

    int ParserState::getPrevLine() const
    {
        return (m_it - 1)->line;
    }

    const std::string &ParserState::getValue() const
    {
        return m_it->value;
    }

    const std::string &ParserState::getPrevValue() const
    {
        return (m_it - 1)->value;
    }

    const std::string &ParserState::getTestRootPath() const
    {
        return m_test.getRootPath();
    }

    void ParserState::setTestName()
    {
        m_test = Test(m_it->value);
    }

    bool ParserState::setTestRootPath()
    {
        if (std::filesystem::is_directory(m_it->value) && std::filesystem::exists(m_it->value))
        {
            m_test.setRootPath(m_it->value);
            std::filesystem::path path = std::filesystem::relative("./", m_test.getRootPath()) / "";
            m_specialVariables["malidir"] = path.string();
            return true;
        }
        return false;
    }

    void ParserState::setType(TaskType type)
    {
        m_type = type;
    }

    void ParserState::setPrintTestOut(bool printOut)
    {
        m_printTestOut = printOut;
    }

    void ParserState::addArg()
    {
        m_task.emplace_back(m_it->value);
    }

    void ParserState::addArg(const std::string &value)
    {
        m_task.emplace_back(value);
    }

    void ParserState::pushCommand(Command internalCommand)
    {
        m_test.addTask(std::move(m_task), m_type, internalCommand, m_printTestOut);
        m_type = TaskType::Preparation;
        m_task = mali::Task();
    }

    void ParserState::pushLoop(int iterations)
    {
        m_flowControl.emplace(m_it, iterations);
    }

    bool ParserState::popLoop()
    {
        if (m_flowControl.empty())
            return false;
        if (--m_flowControl.top().second > 0)
            m_it = m_flowControl.top().first;
        else
        {
            m_flowControl.pop();
            m_it++;
        }
        return true;
    }

    void ParserState::setVar(const std::string &value)
    {
        auto it = m_specialVariables.find(m_it->value);
        if (it != m_specialVariables.end())
        {
            throw std::invalid_argument("Line " + std::to_string(m_it->line) + ": Can't set value of special variable \"" + it->first + "\".");
        }
        m_variables[m_it->value] = value;
    }

    std::string *ParserState::getVar(const std::string &name) const
    {
        auto it = m_specialVariables.find(name);
        if (it != m_specialVariables.end())
            return const_cast<std::string*>(&it->second);
        it = m_variables.find(name);
        return (it != m_variables.end()) ? const_cast<std::string*>(&it->second) : nullptr;
    }
}
