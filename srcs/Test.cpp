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
        std::stringstream out;
        std::stringstream err;
        std::stringstream prevout;
        std::stringstream preverr;

        std::cout << "Test name : " << m_name << "\n"
                  << "Test path : " << m_rootPath << std::endl;

        for (const TaskData &task : m_tasks)
        {
            int ret = spawnProcess(task.task, m_rootPath.c_str(), out, err);
            if (task.type == TaskType::Compare)
            {
                std::cout << "==============================================================================================================================\n"
                          << "Compare stdout >";
                for (std::size_t i = 0; i < task.task.size(); i++)
                {
                    if (i == 0)
                    {
                        std::cout << " " << task.task[i] << " ";
                    }
                    else
                        std::cout << "\"" << task.task[i] << "\"";
                }
                std::cout << "\n";
                if (out.str() != prevout.str())
                {
                    std::cout << "------------------------------------------------------------------------------------------------------------------------------\n"
                              << "your output :\n"
                              << out.rdbuf()
                              << "\n"
                              << "------------------------------------------------------------------------------------------------------------------------------\n"
                              << "mali output :\n"
                              << prevout.rdbuf()
                              << "\n"
                              << "------------------------------------------------------------------------------------------------------------------------------\n"
                              << "Result : KO\n";
                }
                else
                {
                    std::cout << "Result : OK\n";
                }
                std::cout << "==============================================================================================================================\n"
                          << std::endl;
            }
            if (ret != 0 && ret != 1)
                return ret;
            std::swap(prevout, out);
            std::swap(preverr, err);
            out.str("");
            err.str("");
        }
        return 0;
    }

    void Test::setRootPath(const std::string &rootPath)
    {
        m_rootPath = rootPath;
    }

    void Test::addTask(Task &&task, TaskType type)
    {
        m_tasks.emplace_back(TaskData{std::move(task), type});
    }

    const std::string &Test::getRootPath() const
    {
        return m_rootPath;
    }

    int Test::spawnProcess(const Task &args, const char *processPath, std::ostream &out, std::ostream &err) const
    {
        SystemProcess process(args, processPath);
        if (!process.good())
        {
            
        std::cout << "fail." << std::endl;
            return 127;
        }

        process.readOut(out);
        out << std::flush;

        process.readErr(err);
        err << std::flush;

        return process.wait();
    }
}