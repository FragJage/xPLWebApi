#ifndef CacheManagerMock_H
#define CacheManagerMock_H

#include "../src/CacheManager.h"
using namespace std;

class CacheManagerMock : public ICacheRead
{
public:
    CacheManagerMock();
    ~CacheManagerMock();

    xPL::SchemaObject PopCachedValue(std::string key);
    bool WaitCachedValue(std::string key, unsigned int timeout=3000);
    std::map<std::string, xPL::SchemaObject>* GetModules();
    std::map<std::string, xPL::SchemaObject>* GetDevices();
    std::map<std::string, xPL::SchemaObject>* GetConfigs();
    std::map<std::string, xPL::SchemaObject>* GetParams();
    std::map<std::string, xPL::SchemaObject>* GetAdvanceFormat();
    std::map<std::string, xPL::SchemaObject>* GetAdvanceConfig(const std::string& moduleName);
    void DevicesDelete(const std::string& moduleName);
    void AdvanceConfigDelete(const std::string& moduleName);

    void ModifyAlive();

private:
    std::map<std::string, xPL::SchemaObject> m_MapModules;
    std::map<std::string, xPL::SchemaObject> m_MapDevices;
    std::map<std::string, xPL::SchemaObject> m_MapConfigs;
    std::map<std::string, xPL::SchemaObject> m_MapParams;
    std::map<std::string, xPL::SchemaObject> m_MapAdvFormats;
    std::map<std::string, std::map<std::string, xPL::SchemaObject>> m_MapAdvConfigs;
    std::map<std::string, std::queue<xPL::SchemaObject>> m_CachedValues;
};
#endif //CacheManagerMock_H
