#ifndef FRAGMUTEX_H
#define FRAGMUTEX_H

#include <windows.h>

namespace std
{

class mutex
{
    public:
        mutex()
        {
            m_hMutex = CreateMutex(nullptr,false,nullptr);
        }
        ~mutex()
        {
            CloseHandle(m_hMutex);
        }

        bool lock()
        {
            if(WaitForSingleObject(m_hMutex,INFINITE)!=WAIT_OBJECT_0) return false;
            return true;
        }

        bool unlock()
        {
            if(ReleaseMutex(m_hMutex)==0) return false;
            return true;
        }

    private:
        HANDLE m_hMutex;
};

}
#endif  //FRAGMUTEX_H
