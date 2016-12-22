#include <cassert>
#include "UnitTest/UnitTest.h"
#include "../src/DataloggerController.h"
#include "CacheManagerMock.h"
#include "xPLSendMock.h"

using namespace std;

class TestDataloggerController : public TestClass<TestDataloggerController>
{
public:
    TestDataloggerController();
    ~TestDataloggerController();

    bool GetDevices();
    bool GetValues();
    bool WrongSearch();
    bool WrongPOST();

private:
    DataloggerController m_DataloggerController;
    CacheManagerMock m_CacheManagerMock;
    xPLsendMock m_xPLsendMock;
};
