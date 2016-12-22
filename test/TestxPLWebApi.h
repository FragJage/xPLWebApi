#include <cassert>
#include "UnitTest/UnitTest.h"
#ifdef WIN32
    #include "Thread/mingw.thread.h"
#else
    #include <thread>
#endif
#include "../src/xPLWebApi.h"

using namespace std;

class TestxPLWebApi : public TestClass<TestxPLWebApi>
{
public:
    TestxPLWebApi();
    ~TestxPLWebApi();

    static void ThreadStart(xPLWebApi* pxPLDev);
    bool Start();
    bool StdConfig();
    bool Stop();
    bool ReStart();
    bool ReStop();

    xPLWebApi xPLDev;
};
