#pragma once

#include <vector>
#include <string>

namespace mali
{
    using Task = std::vector<std::string>;
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
        };

    public:
        Test(const std::string& name = "");

        void setRootPath(const std::string& name);
        int run() const;
        void addTask(Task&& task, TaskType type);

        const std::string& getRootPath() const;

    private:
        std::string m_name;
        std::string m_rootPath;
        std::vector<TaskData> m_tasks;

        void printCommand(const Task& task) const;
        void printLine(char symbol) const;
        int spawnProcess(const Task &args, const char *processPath, std::ostream &out, std::ostream &err) const;
    };
}