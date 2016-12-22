#include <cassert>
#include "UnitTest/UnitTest.h"
#include "../src/CacheManager.h"

using namespace std;

class TestCacheManager : public TestClass<TestCacheManager>
{
public:
    TestCacheManager();
    ~TestCacheManager();

    bool ModuleCache();
    bool ConfigCache();
    bool ParamCache();
    bool DeviceCache();
    bool AdvanceFormatCache();
    bool AdvanceConfigCache();
    bool QueueCache();

    CacheManager m_CacheManager;
};
