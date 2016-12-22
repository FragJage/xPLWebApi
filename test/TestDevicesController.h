#include <cassert>
#include "UnitTest/UnitTest.h"
#include "../src/DevicesController.h"
#include "CacheManagerMock.h"
#include "xPLSendMock.h"

using namespace std;

class TestDevicesController : public TestClass<TestDevicesController>
{
public:
    TestDevicesController();
    ~TestDevicesController();

    bool GetList();
    bool GetDevice();
    bool DeviceNotFound();
    bool WrongPOST();

private:
    DevicesController m_DevicesController;
    CacheManagerMock m_CacheManagerMock;
    xPLsendMock m_xPLsendMock;
};
