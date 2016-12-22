#include "xPLLib/Schemas/SchemaAdvanceConfig.h"
#include "xPLLib/Schemas/SchemaConfig.h"
#include "xPLLib/Schemas/SchemaSensor.h"
#include "TestCacheManager.h"

using namespace std;

TestCacheManager::TestCacheManager() : TestClass("CacheManager", this)
{
	addTest("ModuleCache", &TestCacheManager::ModuleCache);
	addTest("ConfigCache", &TestCacheManager::ConfigCache);
	addTest("ParamCache", &TestCacheManager::ParamCache);
	addTest("DeviceCache", &TestCacheManager::DeviceCache);
	addTest("AdvanceFormatCache", &TestCacheManager::AdvanceFormatCache);
	addTest("AdvanceConfigCache", &TestCacheManager::AdvanceConfigCache);
	addTest("QueueCache", &TestCacheManager::QueueCache);
}

TestCacheManager::~TestCacheManager()
{
}

bool TestCacheManager::ModuleCache()
{
    xPL::SchemaConfigApp schCA1(25, 3901, "127.0.0.1");
    xPL::SchemaConfigApp schCA2(30, 3902, "127.0.0.1");
    map<string, xPL::SchemaObject>* Modules;
    xPL::SchemaObject schObj;

    schCA1.SetSource("fragxpl-moduleone.default");
    m_CacheManager.ModuleAdd(schCA1, true);

    Modules = m_CacheManager.GetModules();
    assert(Modules->find("fragxpl-moduleone.default")!=Modules->end());
    schObj = (*Modules)["fragxpl-moduleone.default"];
    assert("25" == schObj.GetValue("interval"));
    assert("Need config" == schObj.GetValue("status"));
    assert("ok" == schObj.GetValue("alivectrl"));

    m_CacheManager.ModuleAdd(schCA1, false);
    schObj = (*Modules)["fragxpl-moduleone.default"];
    assert(1==Modules->size());

    schCA2.SetSource("fragxpl-moduletwo.default");
    m_CacheManager.ModuleAdd(schCA2, true);
    assert(Modules->find("fragxpl-moduletwo.default")!=Modules->end());
    schObj = (*Modules)["fragxpl-moduletwo.default"];
    assert("30" == schObj.GetValue("interval"));
    assert(2==Modules->size());

    m_CacheManager.ModuleDelete("fragxpl-moduletwo.default");
    assert(1==Modules->size());

    return true;
}

bool TestCacheManager::ConfigCache()
{
    map<string, xPL::SchemaObject>* Configs;
    xPL::SchemaObject schObj;

    xPL::SchemaConfigCurrent schCC1;
    schCC1.SetSource("fragxpl-moduleone.default");
    schCC1.AddValue("newconf", "default");
    schCC1.AddValue("interval", "25");
    schCC1.AddValue("group", "groupone");
    schCC1.AddValue("group", "grouptwo");
    schCC1.AddValue("filter", "filterone");
    schCC1.AddValue("filter", "filtertwo");
    schCC1.AddValue("port", "80");

    m_CacheManager.ConfigAdd(schCC1);
    Configs = m_CacheManager.GetConfigs();

    assert(Configs->find("fragxpl-moduleone.default")!=Configs->end());
    schObj = (*Configs)["fragxpl-moduleone.default"];
    assert("25" == schObj.GetValue("interval"));
    assert(1==Configs->size());

    m_CacheManager.ModuleDelete("fragxpl-moduleone.default");
    assert(0==Configs->size());

    return true;
}

bool TestCacheManager::ParamCache()
{
    map<string, xPL::SchemaObject>* Params;
    xPL::SchemaObject schObj;

    xPL::SchemaConfigList schCL1;
    schCL1.SetSource("fragxpl-moduleone.default");
    schCL1.AddValue("reconf", "newconf");
    schCL1.AddValue("option", "interval");
    schCL1.AddValue("option", "group[16]");
    schCL1.AddValue("option", "filter[16]");
    schCL1.AddValue("reconf", "port");

    m_CacheManager.ParamAdd(schCL1);
    Params = m_CacheManager.GetParams();

    assert(Params->find("fragxpl-moduleone.default")!=Params->end());
    schObj = (*Params)["fragxpl-moduleone.default"];
    assert("interval" == schObj.GetValue("option", 0));
    assert("group[16]" == schObj.GetValue("option", 1));
    assert("filter[16]" == schObj.GetValue("option", 2));
    assert(1==Params->size());

    m_CacheManager.ModuleDelete("fragxpl-moduleone.default");
    assert(0==Params->size());

    return true;
}

bool TestCacheManager::DeviceCache()
{
    map<string, xPL::SchemaObject>* Devices;
    xPL::SchemaObject schObj;

    xPL::SchemaSensorBasic schDev1("device1", xPL::SchemaSensorTypeUtility::SensorType::temp, "28", "°");
    schDev1.SetSource("fragxpl-moduletwo.default");
    schDev1.SetValue("max", "30");
    schDev1.SetValue("min", "26");

    m_CacheManager.DeviceAdd(schDev1);
    Devices = m_CacheManager.GetDevices();

    assert(Devices->find("device1")!=Devices->end());
    schObj = (*Devices)["device1"];
    assert("28" == schObj.GetValue("current"));
    assert("30" == schObj.GetValue("max"));
    assert("26" == schObj.GetValue("min"));
    assert(1==Devices->size());

    m_CacheManager.DeviceAdd(schDev1);
    assert(1==Devices->size());

    xPL::SchemaSensorBasic schDev3("device3", xPL::SchemaSensorTypeUtility::SensorType::output, "high");
    schDev3.SetSource("fragxpl-moduletwo.default");
    m_CacheManager.DeviceAdd(schDev3);
    assert(2==Devices->size());

    m_CacheManager.DeviceDelete("device3");
    assert(1==Devices->size());

    m_CacheManager.DevicesDelete("fragxpl-moduletwo.default");
    assert(0==Devices->size());

    return true;
}

bool TestCacheManager::AdvanceFormatCache()
{
    map<string, xPL::SchemaObject>* AdvFormat;
    xPL::SchemaObject schObj;

    xPL::SchemaAdvanceConfigList schACL1;
    schACL1.SetSource("fragxpl-moduletwo.default");
    schACL1.AddValue("configname", "string");
    schACL1.AddValue("type", "string[controltype]");
    schACL1.AddValue("value", "string");

    m_CacheManager.AdvanceFormatAdd(schACL1);
    AdvFormat = m_CacheManager.GetAdvanceFormat();

    assert(AdvFormat->find("fragxpl-moduletwo.default")!=AdvFormat->end());
    schObj = (*AdvFormat)["fragxpl-moduletwo.default"];
    assert("string" == schObj.GetValue("configname"));
    assert("string[controltype]" == schObj.GetValue("type"));
    assert("string" == schObj.GetValue("value"));
    assert(1==AdvFormat->size());

    return true;
}

bool TestCacheManager::AdvanceConfigCache()
{
    map<string, xPL::SchemaObject>* AdvConfig;
    xPL::SchemaObject schObj;

    xPL::SchemaAdvanceConfigCurrent schACC1;
    schACC1.SetSource("fragxpl-moduletwo.default");
    schACC1.AddValue("configname", "device1");
    schACC1.AddValue("type", "temp");
    schACC1.AddValue("value", "28");

    m_CacheManager.AdvanceConfigAdd(schACC1);
    AdvConfig = m_CacheManager.GetAdvanceConfig("fragxpl-moduletwo.default");

    assert(AdvConfig->find("device1")!=AdvConfig->end());
    schObj = (*AdvConfig)["device1"];
    assert("device1" == schObj.GetValue("configname"));
    assert("temp" == schObj.GetValue("type"));
    assert("28" == schObj.GetValue("value"));
    assert(1==AdvConfig->size());

    return true;
}

bool TestCacheManager::QueueCache()
{
    xPL::SchemaObject schObj;
    xPL::SchemaSensorBasic schDev1("device1", xPL::SchemaSensorTypeUtility::SensorType::temp, "28", "°");
    schDev1.SetSource("fragxpl-moduletwo.default");
    schDev1.SetValue("max", "30");
    schDev1.SetValue("min", "26");

    m_CacheManager.PushCachedValue("MyTest", schDev1);
    assert(true==m_CacheManager.WaitCachedValue("MyTest", 150));
    schObj = m_CacheManager.PopCachedValue("MyTest");
    assert("device1"==schObj.GetValue("device"));
    assert("28"==schObj.GetValue("current"));

    return true;
}
