#pragma once

#ifdef _WIN32
# include <Windows.h>
#else
# include <unistd.h>
# include <sys/wait.h>
#endif

#include <vector>
#include <string>

namespace mali
{
#ifdef _WIN32
    using ProcessHandle = PROCESS_INFORMATION;
    using PipeHandle = HANDLE;
    using ArgContainer = std::wstring;
#else
    using ProcessHandle = pid_t;
    using PipeHandle = int;
    using ArgContainer = std::vector<const char *>;
#endif

    class SystemProcess
    {
    public:
        SystemProcess(const std::vector<std::string>& args, const char* processPath);
        SystemProcess(const SystemProcess&) = delete;
        SystemProcess& operator=(const SystemProcess&) = delete;
        ~SystemProcess();

        void readOut(std::ostream& out);
        void readErr(std::ostream& err);
        bool good() const;
        int wait();

    private:
        ProcessHandle m_handle;
        PipeHandle m_testPipe[2];
        PipeHandle m_errPipe[2];
        bool m_good;

        bool initProcess(const std::vector<std::string>& args, const char* processPath);
    };
}