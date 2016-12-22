#ifndef Plateforms_H
#define Plateforms_H

#include<string>

#ifdef _WIN32
    #include<windows.h>
#endif

#ifdef PROCESS_MOCK
    #include<map>
    #define processId int
#else
    #ifdef _WIN32
    #define processId PROCESS_INFORMATION
    #define _NO_OLDNAMES
    #else
    #define processId pid_t
    #endif // _WIN32
#endif

class Plateforms
{
    public:
        static void delay(unsigned long ms);
};

class Process
{
    public:
        static processId launch(std::string command);
        static bool terminate(processId pid);
        static bool exist(processId pid);
        #ifdef PROCESS_MOCK
        static bool find(std::string command);
        #endif
    private:
        #ifdef PROCESS_MOCK
        static int m_pidMax;
        static std::map<processId, std::string> m_processList;
        #endif
};
#endif
