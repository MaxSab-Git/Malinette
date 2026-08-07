#include <iostream>
#include <sstream>
#include <iomanip>

#include <vector>

#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>

#include <Test.h>

namespace mali
{
    Test::Test(const std::string& name) : m_name(name), m_rootPath("./")
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
                std::cout << "========================================================================\n"
                          << "Compare stdout >";
                for (std::size_t i = 0; i < task.task.size(); i++)
                {
                    if (i == 0)
                        std::cout << " " << task.task[i] << " ";
                    else
                        std::cout << "\"" << task.task[i] << "\"";
                }
                std::cout << "\n";
                if (out.str() != prevout.str())
                {
                    std::cout << "------------------------------------------------------------------------\n"
                              << "your output :\n"
                              << out.str()
                              << "\n"
                              << "------------------------------------------------------------------------\n"
                              << "mali output :\n"
                              << prevout.str()
                              << "\n"
                              << "------------------------------------------------------------------------\n"
                              << "Result : KO\n";
                }
                else
                {
                    std::cout << "Result : OK\n";
                }
                std::cout << "========================================================================\n"
                          << std::endl;
            }
            if (ret != 0)
                return ret;
            std::swap(prevout, out);
            std::swap(preverr, err);
            out.str("");
            err.str("");
        }
        return 0;
    }

    void Test::setRootPath(const std::string& rootPath)
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
        int test_pipe[2];
        int err_pipe[2];
        if (pipe(test_pipe) < 0)
            return 127;
        if (pipe(err_pipe) < 0)
        {
            close(test_pipe[1]);
            close(test_pipe[0]);
            return 127;
        }

        pid_t pid = fork();
        if (pid == 0)
        {
            close(test_pipe[0]);
            close(err_pipe[0]);

            if (chdir(processPath) < 0)
                _exit(errno);

            std::vector<const char *> argv;
            argv.reserve(args.size() + 1);
            for (const std::string &arg : args)
                argv.push_back(arg.c_str());
            argv.push_back(nullptr);

            dup2(test_pipe[1], STDOUT_FILENO);
            dup2(err_pipe[1], STDERR_FILENO);
            close(test_pipe[1]);
            close(err_pipe[1]);

            if (argv.back() == nullptr)
                execv(argv[0], const_cast<char *const *>(argv.data()));
            _exit(127);
        }

        close(test_pipe[1]);
        close(err_pipe[1]);

        int status;
        if (pid >= 0)
        {
            char c[1024];
            int readed;

            while (true)
            {
                readed = read(test_pipe[0], c, 1024);
                if (readed <= 0)
                    break;
                out.write(c, readed);
            }
            out << std::flush;

            while (true)
            {
                readed = read(err_pipe[0], c, 1024);
                if (readed <= 0)
                    break;
                err.write(c, readed);
            }
            err << std::flush;
            waitpid(pid, &status, 0);
        }

        close(test_pipe[0]);
        close(err_pipe[0]);

        return (pid < 0) ? -1 : WEXITSTATUS(status);
    }
}