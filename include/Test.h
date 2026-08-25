#pragma once

#include <vector>
#include <string>
#include <chrono>

namespace mali
{
    using Task = std::vector<std::string>;
    using Command =  int (*)(const Task &args, const char *processPath, std::ostream &out, std::ostream &err, std::chrono::milliseconds timeout);

    enum class TaskType
    {
        Preparation,
        Compare,
    };

    class Test
    {
    private:
        struct TaskData
        {
            Task task;
            TaskType type;
            Command command;
            bool printOut;
            std::chrono::milliseconds timeout;
        };

    public:
        Test(const std::string& name = "");

        void setRootPath(const std::string& name);
        int run() const;
        void addTask(Task&& task, TaskType type, Command internalCommand = nullptr, bool printOut = false, std::chrono::milliseconds timeout = std::chrono::milliseconds(3000));

        const std::string& getRootPath() const;

    private:
        std::string m_name;
        std::string m_rootPath;
        std::vector<TaskData> m_tasks;

        void printCommand(const Task& task) const;
        void printLine(char symbol) const;
        void printStream(std::istream& stream) const;
        static int spawnProcess(const Task &args, const char *processPath, std::ostream &out, std::ostream &err, std::chrono::milliseconds timeout);
    };
}