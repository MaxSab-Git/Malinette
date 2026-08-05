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
        Test(const char *name);

        void setRootPath(const char *rootPath);
        int run() const;
        void addTask(Task&& task, TaskType type);

    private:
        std::string m_name;
        std::string m_rootPath;
        std::vector<TaskData> m_tasks;

        int spawnProcess(const Task &args, const char *processPath, std::ostream &out, std::ostream &err) const;
    };
}