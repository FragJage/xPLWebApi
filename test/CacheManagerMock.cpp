#include "CacheManagerMock.h"
#include "xPLLib/Schemas/SchemaConfig.h"
#include "xPLLib/Schemas/SchemaAdvanceConfig.h"
#include "xPLLib/Schemas/SchemaSensor.h"

using namespace std;

CacheManagerMock::CacheManagerMock()
{
    xPL::SchemaConfigApp schCA1(25, 3901, "127.0.0.1");
    xPL::SchemaConfigApp schCA2(25, 3902, "127.0.0.1");

    schCA1.SetSource("fragxpl-moduleone.default");
    m_MapModules["fragxpl-moduleone.default"] = schCA1;
    schCA2.SetSource("fragxpl-moduletwo.default");
    m_MapModules["fragxpl-moduletwo.default"] = schCA2;

    xPL::SchemaConfigList schCL1;
    schCL1.SetSource("fragxpl-moduleone.default");
    schCL1.AddValue("reconf", "newconf");
    schCL1.AddValue("option", "interval");
    schCL1.AddValue("option", "group[16]");
    schCL1.AddValue("option", "filter[16]");
    schCL1.AddValue("reconf", "port");
    m_MapParams["fragxpl-moduleone.default"] = schCL1;

    xPL::SchemaConfigCurrent schCC1;
    schCC1.SetSource("fragxpl-moduleone.default");
    schCC1.AddValue("newconf", "default");
    schCC1.AddValue("interval", "25");
    schCC1.AddValue("group", "groupone");
    schCC1.AddValue("group", "grouptwo");
    schCC1.AddValue("filter", "filterone");
    schCC1.AddValue("filter", "filtertwo");
    schCC1.AddValue("port", "80");
    m_MapConfigs["fragxpl-moduleone.default"] = schCC1;

    xPL::SchemaAdvanceConfigCurrent schACC1;
    schACC1.SetSource("fragxpl-moduletwo.default");
    schACC1.AddValue("configname", "device1");
    schACC1.AddValue("type", "temp");
    schACC1.AddValue("value", "28");
    m_MapAdvConfigs["fragxpl-moduletwo.default"]["device1"] = schACC1;
    xPL::SchemaAdvanceConfigCurrent schACC2;
    schACC2.SetSource("fragxpl-moduletwo.default");
    schACC2.AddValue("configname", "device2");
    schACC2.AddValue("type", "output");
    schACC2.AddValue("value", "high");
    m_MapAdvConfigs["fragxpl-moduletwo.default"]["device2"] = schACC2;

    xPL::SchemaAdvanceConfigList schACL1;
    schACL1.SetSource("fragxpl-moduletwo.default");
    schACL1.AddValue("configname", "string");
    schACL1.AddValue("type", "string[controltype]");
    schACL1.AddValue("value", "string");
    m_MapAdvFormats["fragxpl-moduletwo.default"] = schACL1;
    xPL::SchemaAdvanceConfigList schACL2;
    schACL2.SetSource("fragxpl-moduleone.default");
    schACL2.AddValue("configname", "string");
    schACL2.AddValue("count", "booleen");
    schACL2.AddValue("period", "integer");
    schACL2.AddValue("output", "device");
    m_MapAdvFormats["fragxpl-moduleone.default"] = schACL2;

    xPL::SchemaObject schLog1(xPL::ISchema::stat, "datalogger", "basic");
    schLog1.AddValue("response", "device");
    schLog1.AddValue("device", "fragxpl-moduletwo.default:device1");
    schLog1.AddValue("device", "fragxpl-moduletwo.default:device2");
    m_CachedValues["datalogger-device-20100101-20101231"].push(schLog1);

    xPL::SchemaObject schLog2(xPL::ISchema::stat, "datalogger", "basic");
    schLog2.SetValue("response", "value");
    schLog2.SetValue("time", "20100101");
    schLog2.SetValue("value", "15");
    schLog2.SetValue("min", "5");
    schLog2.SetValue("max", "18");
    m_CachedValues["datalogger-values-fragxpl-moduletwo.default:device1"].push(schLog2);
    schLog2.SetValue("time", "20100102");
    schLog2.SetValue("value", "16");
    schLog2.SetValue("min", "6");
    schLog2.SetValue("max", "17");
    m_CachedValues["datalogger-values-fragxpl-moduletwo.default:device1"].push(schLog2);

    xPL::SchemaSensorBasic schDev1("device1", xPL::SchemaSensorTypeUtility::SensorType::temp, "28", "°");
    schDev1.SetSource("fragxpl-moduletwo.default");
    schDev1.SetValue("max", "30");
    schDev1.SetValue("min", "26");
    m_MapDevices["fragxpl-moduletwo.default:device1"] = schDev1;
    xPL::SchemaSensorBasic schDev2("device2", xPL::SchemaSensorTypeUtility::SensorType::temp, "25", "°");
    schDev2.SetSource("fragxpl-moduletwo.default");
    schDev2.SetValue("max", "30");
    schDev2.SetValue("min", "20");
    m_MapDevices["fragxpl-moduletwo.default:device2"] = schDev2;
    xPL::SchemaSensorBasic schDev3("device3", xPL::SchemaSensorTypeUtility::SensorType::output, "high");
    schDev3.SetSource("fragxpl-moduletwo.default");
    m_MapDevices["fragxpl-moduletwo.default:device3"] = schDev3;
}

CacheManagerMock::~CacheManagerMock()
{
}

xPL::SchemaObject CacheManagerMock::PopCachedValue(string key)
{
    std::map<std::string, queue<xPL::SchemaObject>>::iterator it;
    xPL::SchemaObject msg;

    it=m_CachedValues.find(key);
    if(it!=m_CachedValues.end())
    {
        msg = it->second.front();
        it->second.pop();
    }

    return msg;
}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
bool CacheManagerMock::WaitCachedValue(string key, unsigned int timeout)
{
    std::map<std::string, queue<xPL::SchemaObject>>::iterator it;
    std::map<std::string, queue<xPL::SchemaObject>>::iterator itEnd = m_CachedValues.end();

    it=m_CachedValues.find(key);
    if((it!=itEnd)&&(!it->second.empty())) return true;

    return false;
}
#pragma GCC diagnostic pop

map<string, xPL::SchemaObject>* CacheManagerMock::GetModules()
{
    return &m_MapModules;
}

map<string, xPL::SchemaObject>* CacheManagerMock::GetDevices()
{
    return &m_MapDevices;
}

map<string, xPL::SchemaObject>* CacheManagerMock::GetConfigs()
{
    return &m_MapConfigs;
}

map<string, xPL::SchemaObject>* CacheManagerMock::GetParams()
{
    return &m_MapParams;
}

map<string, xPL::SchemaObject>* CacheManagerMock::GetAdvanceFormat()
{
    return &m_MapAdvFormats;
}

map<string, xPL::SchemaObject>* CacheManagerMock::GetAdvanceConfig(const string& moduleName)
{
    return &m_MapAdvConfigs[moduleName];
}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
void CacheManagerMock::DevicesDelete(const string& moduleName)
{
}

void CacheManagerMock::AdvanceConfigDelete(const string& moduleName)
{
}
#pragma GCC diagnostic pop

void CacheManagerMock::ModifyAlive()
{
    map<string, xPL::SchemaObject>::iterator it;
    string code;

    for(it=m_MapModules.begin(); it!=m_MapModules.end();++it)
    {
        code = it->second.GetValue("alive");
        if(code == "B") code = "C";
        if(code == "A") code = "B";
        if(code == "") code = "A";
        it->second.SetValue("alive",code);
    }
}
