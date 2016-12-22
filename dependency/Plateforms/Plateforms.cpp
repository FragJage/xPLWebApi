#include "Plateforms.h"
#include <cstdarg>

using namespace std;

#ifdef _WIN32
void Plateforms::delay(unsigned long ms)
{
    Sleep( ms );
}
#else
#include<unistd.h>
void Plateforms::delay(unsigned long ms)
{
    usleep(ms*1000);
}
#endif

#ifdef PROCESS_MOCK
    map<processId, string> Process::m_processList;
    processId Process::m_pidMax=0;

    processId Process::launch(string command)
    {
        m_pidMax++;
        m_processList[m_pidMax] = command;

        return m_pidMax;
    }

    bool Process::terminate(processId pid)
    {
        map<processId, string>::iterator it;

        it = m_processList.find(pid);
        if(it==m_processList.end()) return false;

        m_processList.erase(it);
        return true;
    }

    bool Process::exist(processId pid)
    {
        map<processId, string>::iterator it;

        it = m_processList.find(pid);

        return(it!=m_processList.end());
    }

    bool Process::find(string command)
    {
        map<processId, string>::iterator it;

        for(it = m_processList.begin(); it != m_processList.end(); ++it)
        {
            if(it->second==command) return true;
        }

        return false;
    }

#else
    #ifdef _WIN32
    processId Process::launch(string command)
    {
        PROCESS_INFORMATION process;
        STARTUPINFO startup;

        memset(&startup, 0, sizeof(startup));
        startup.cb = sizeof(startup);
        CreateProcessA(0, const_cast<LPSTR>(command.c_str()), 0, 0, TRUE, 0, 0, 0, &startup, &process);
        return process;
    }

    bool Process::terminate(processId pid)
    {
        BOOL result;

        result = TerminateProcess(pid.hProcess, 0);
        CloseHandle(pid.hProcess);
        CloseHandle(pid.hThread);

        return result;
    }

    bool Process::exist(processId pid)
    {
        DWORD result;

        result = WaitForSingleObject(pid.hProcess, 0);
        return result == WAIT_TIMEOUT;
    }

    #else

    #include<errno.h>
    #include<sys/wait.h>
    #include<cstdlib>
    #include<csignal>

    processId Process::launch(string command)
    {
        processId pid;

        pid = fork();
        if(pid==0)
        {
            execl(command.c_str(), command.c_str(), nullptr);
            exit(1);
        }
        return pid;
    }

    bool Process::terminate(processId pid) //, bool forceKill)
    {
        int signal;


        //if(forceKill==true)
        //    signal = SIGKILL;
        //else
            signal = SIGTERM;

        if(kill(pid, signal) == 0) return true;
        if (errno == ESRCH) return true;
        return false;
    }

    bool Process::exist(processId pid)
    {
        while(waitpid(-1, 0, WNOHANG) > 0);     // Wait for defunct....
        if(kill(pid, 0) == 0) return true;        // Process exists
        return false;
    }
#endif

#endif
