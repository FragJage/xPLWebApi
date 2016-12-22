#include <cassert>
#include "UnitTest/UnitTest.h"
#include "../src/AdvanceConfigController.h"
#include "CacheManagerMock.h"
#include "xPLSendMock.h"

using namespace std;

class TestAdvanceConfigController : public TestClass<TestAdvanceConfigController>
{
public:
    TestAdvanceConfigController();
    ~TestAdvanceConfigController();

    bool GetList();
    bool GetAdvanceConfig();
    bool SetAdvanceConfig();
    bool ConfigNotFound();
    bool WrongPOST();

private:
    AdvanceConfigController m_AdvanceConfigController;
    CacheManagerMock m_CacheManagerMock;
    xPLsendMock m_xPLsendMock;
};
