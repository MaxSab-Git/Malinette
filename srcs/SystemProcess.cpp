#include <SystemProcess.h>
#include <filesystem>
#include <iostream>

namespace mali
{
    SystemProcess::SystemProcess(const std::vector<std::string> &args, const char* processPath)
    {
        m_good = initProcess(args, processPath);
    }

    bool SystemProcess::good() const
    {
        return m_good;
    }

#ifdef _WIN32
    SystemProcess::~SystemProcess()
    {
        if (m_good)
        {
            CloseHandle(m_testPipe[0]);
            CloseHandle(m_errPipe[0]);
            CloseHandle(m_handle.hProcess);
            CloseHandle(m_handle.hThread);
            m_good = false;
        }
    }

    void SystemProcess::readOut(std::ostream &out)
    {
        char c[1024];
        DWORD readed;

        while (ReadFile(m_testPipe[0], c, 1024, &readed, nullptr) == TRUE)
        {
            if (readed <= 0)
                break;
            out.write(c, readed);
        }
    }

    void SystemProcess::readErr(std::ostream &err)
    {
        char c[1024];
        DWORD readed;

        while (ReadFile(m_errPipe[0], c, 1024, &readed, nullptr) == TRUE)
        {
            if (readed <= 0)
                break;
            err.write(c, readed);
        }
    }

    int SystemProcess::wait()
    {
        if (!m_good)
            return 127;
        
        DWORD code;
        if (WaitForSingleObject(m_handle.hProcess, INFINITE) == WAIT_OBJECT_0)
        {
            GetExitCodeProcess(m_handle.hProcess, &code);
            return code;
        }
        return 127;
    }

    // Much more finicky than linux implementaion
    bool SystemProcess::initProcess(const std::vector<std::string> &args, const char* processPath)
    {
        SECURITY_ATTRIBUTES inherit;
        inherit.bInheritHandle = TRUE;
        inherit.nLength = sizeof(inherit);
        inherit.lpSecurityDescriptor = nullptr;

        if (CreatePipe(&m_testPipe[0], &m_testPipe[1], &inherit, 0) == FALSE)
            return false;
        if (SetHandleInformation(m_testPipe[0], HANDLE_FLAG_INHERIT, 0) == FALSE
            || CreatePipe(&m_errPipe[0], &m_errPipe[1], &inherit, 0) == FALSE)
        {
            CloseHandle(m_testPipe[0]);
            CloseHandle(m_testPipe[1]);
            return false;
        }
        if (SetHandleInformation(m_errPipe[0], HANDLE_FLAG_INHERIT, 0) == FALSE)
        {
            CloseHandle(m_testPipe[0]);
            CloseHandle(m_testPipe[1]);
            CloseHandle(m_errPipe[0]);
            CloseHandle(m_errPipe[1]);
            return false;
        }
        
        ArgContainer argv;
        argv.reserve(128);
        for (const std::string &arg : args)
        {
            argv.push_back('\"');
            for (char c : arg)
            {
                if (c == '\"')
                {
                    if (argv.back() == L'\\')
                        argv.push_back('\\');
                    argv.push_back('\\');
                }
                argv.push_back(c);
            }
            if (argv.back() == L'\\')
                argv.push_back('\\');
            argv.push_back('\"');
            argv.push_back(' ');
        }
        argv.pop_back();

        STARTUPINFOW si;
        ZeroMemory(&si, sizeof(si));
        si.cb = sizeof(si);
        si.dwFlags = STARTF_USESTDHANDLES | STARTF_UNTRUSTEDSOURCE;
        si.hStdOutput = m_testPipe[1];
        si.hStdError = m_errPipe[1];

        ZeroMemory(&m_handle, sizeof(m_handle));

        std::filesystem::path defaultPath = std::filesystem::current_path();
        std::filesystem::current_path(processPath);

        BOOL result = CreateProcessW(
            nullptr,
            const_cast<wchar_t*>(argv.c_str()),
            nullptr,
            nullptr,
            TRUE,
            0,
            nullptr,
            nullptr, // <-- For those who asks, no, just set this is not fine to replace double std::filesystem::current_path().
            &si,
            &m_handle
        );

        std::filesystem::current_path(defaultPath);

        if (result == FALSE)
        {
            CloseHandle(m_testPipe[0]);
            CloseHandle(m_testPipe[1]);
            CloseHandle(m_errPipe[0]);
            CloseHandle(m_errPipe[1]);
            return false;
        }

        CloseHandle(m_testPipe[1]);
        CloseHandle(m_errPipe[1]);

        return true;
    }
#else
    SystemProcess::~SystemProcess()
    {
        if (m_good)
        {
            close(m_testPipe[0]);
            close(m_errPipe[0]);
            m_good = false;
        }
    }

    void SystemProcess::readOut(std::ostream &out)
    {
        char c[1024];
        int readed;

        while (true)
        {
            readed = read(m_testPipe[0], c, 1024);
            if (readed <= 0)
                break;
            out.write(c, readed);
        }
    }

    void SystemProcess::readErr(std::ostream &err)
    {
        char c[1024];
        int readed;

        while (true)
        {
            readed = read(m_errPipe[0], c, 1024);
            if (readed <= 0)
                break;
            err.write(c, readed);
        }
    }

    int SystemProcess::wait()
    {
        if (!m_good)
            return 127;

        int status;
        waitpid(m_handle, &status, 0);
        if (WIFEXITED(status))
            return WEXITSTATUS(status);
        return 127;
    }

    bool SystemProcess::initProcess(const std::vector<std::string> &args, const char* processPath)
    {
        if (pipe(m_testPipe) < 0)
            return false;
        if (pipe(m_errPipe) < 0)
        {
            close(m_testPipe[1]);
            close(m_testPipe[0]);
            return false;
        }

        m_handle = fork();
        if (m_handle < 0)
            return false;
        if (m_handle == 0)
        {
            close(m_testPipe[0]);
            close(m_errPipe[0]);

            std::filesystem::current_path(processPath);

            ArgContainer argv;
            argv.reserve(args.size() + 1);
            for (const std::string &arg : args)
            {
                argv.push_back(arg.c_str());
            }
            argv.push_back(nullptr);

            dup2(m_testPipe[1], STDOUT_FILENO);
            dup2(m_errPipe[1], STDERR_FILENO);
            close(m_testPipe[1]);
            close(m_errPipe[1]);

            if (argv.back() == nullptr)
                execvp(argv[0], const_cast<char *const *>(argv.data()));
            std::cout << "fail." << std::endl;
            _exit(127);
        }
        close(m_testPipe[1]);
        close(m_errPipe[1]);
        return true;
    }
#endif
}
