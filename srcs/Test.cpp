#include <iostream>
#include <sstream>
#include <iomanip>
#include <filesystem>
#include <SystemProcess.h>

#include <vector>

#include <Test.h>

namespace mali
{
    Test::Test(const std::string &name) : m_name(name), m_rootPath("./")
    {
    }

    int Test::run() const
    {
        std::stringstream out("");
        std::stringstream err("");
        std::stringstream prevout("");
        std::stringstream preverr("");
        int lastRet = 0;

        std::cout << "Test name : " << m_name << "\n"
                  << "Test path : " << m_rootPath << '\n'
                  << std::endl;

        printLine('=');
        for (const TaskData &task : m_tasks)
        {
            lastRet = task.command(task.task, m_rootPath.c_str(), out, err);
            if (lastRet == 127)
                return -2;
            if (task.type == TaskType::Compare)
            {
                std::cout << "\nCompare stdout >";
                printCommand(task.task);
                std::cout << "\n";
                if (out.str() != prevout.str())
                {
                    printLine('-');

                    std::cout << "your output :\n";
                    printStream(out);
                    printLine('-');

                    std::cout << "mali output :\n";
                    printStream(err);
                    printLine('-');
                    
                    std::cout << "Result : KO\n";
                }
                else
                {
                    if (task.printOut)
                    {
                        std::cout << "stdout > ";
                        printStream(out);
                    }
                    std::cout << "Result : OK\n";
                }
                printLine('-');
            }
            else if (task.type == TaskType::Preparation)
            {
                std::cout << "Preparation >";
                printCommand(task.task);
                if (lastRet != 0)
                {
                    std::cout << err.rdbuf() << '\n';
                    std::cout << "Result : KO\n";
                    printLine('=');
                    return lastRet;
                }
                std::cout << '\n';
                if (task.printOut)
                {
                    std::cout << "stdout > ";
                    printStream(out);
                }
            }
            std::cout << std::flush;
            std::swap(prevout, out);
            std::swap(preverr, err);
            out.str("");
            err.str("");
        }
        printLine('=');
        std::cout << std::flush;
        return 0;
    }

    void Test::setRootPath(const std::string &rootPath)
    {
        m_rootPath = rootPath;
    }

    void Test::addTask(Task &&task, TaskType type, Command internalCommand, bool printOut)
    {
        if (internalCommand)
            m_tasks.emplace_back(TaskData{std::move(task), type, internalCommand, printOut});
        else
            m_tasks.emplace_back(TaskData{std::move(task), type, spawnProcess, printOut});
    }

    const std::string &Test::getRootPath() const
    {
        return m_rootPath;
    }

    void Test::printCommand(const Task &task) const
    {
        for (std::size_t i = 0; i < task.size(); i++)
        {
            if (i == 0)
                std::cout << " " << task[i];
            else
                std::cout << " \"" << task[i] << "\"";
        }
    }

    void Test::printLine(char symbol) const
    {
        for (int i = 0; i < 127; i++)
            std::cout << symbol;
        std::cout << '\n';
    }

    void Test::printStream(std::ostream &stream) const
    {
        std::cout << stream.rdbuf() << '\n';
    }

    int Test::spawnProcess(const Task &args, const char *processPath, std::ostream &out, std::ostream &err)
    {
        SystemProcess process(args, processPath);
        if (!process.good())
        {
            std::cout << "Problem with process launch." << std::endl;
            return 127;
        }

        process.readOut(out);
        out << std::flush;

        process.readErr(err);
        err << std::flush;

        return process.wait();
    }
}