#include <cassert>
#include "UnitTest/UnitTest.h"
#include "../src/ModulesController.h"
#include "CacheManagerMock.h"
#include "xPLSendMock.h"

using namespace std;

class TestModulesController : public TestClass<TestModulesController>
{
public:
    TestModulesController();
    ~TestModulesController();

    bool GetList();
    bool GetConfig();
    bool SetConfig();
    bool WrongPOST();

private:
    ModulesController m_ModulesController;
    CacheManagerMock m_CacheManagerMock;
    xPLsendMock m_xPLsendMock;
};
