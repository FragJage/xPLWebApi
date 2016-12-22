#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include "Plateforms/Plateforms.h"
#include "CacheManager.h"

using namespace std;


CacheManager::CacheManager()
{
}

CacheManager::~CacheManager()
{
}

struct tm CacheManager::GetTmTime()
{
    time_t     now = time(0);
    struct tm  tmstruct;

	#ifdef _MSC_VER
    localtime_s(&tmstruct, &now);
	#else
        #ifdef WIN32
        tmstruct = *localtime(&now);            //MINGW ne gère pas localtime_r
        #else
        localtime_r(&now, &tmstruct);
        #endif
	#endif

	return tmstruct;
}

void CacheManager::SetTime(xPL::SchemaObject& msg, const string& valueName)
{
    struct tm  tmstruct;
    char       buf[32];

    tmstruct = GetTmTime();
    strftime(buf, sizeof(buf), "%Y-%m-%d %X", &tmstruct);
    msg.AddValue(valueName, buf);
}

void CacheManager::SetTimeAlive(xPL::SchemaObject& msg)
{
    SetTime(msg, "alive");
}

void CacheManager::SetTimeDetect(xPL::SchemaObject& msg)
{
    SetTime(msg, "detect");
}

bool CacheManager::ModuleAdd(xPL::SchemaObject& msg, bool bNeedConfig)
{
    string moduleName;
    string detectTime;
    string status;
    string advanceConfig;
    bool bNew;


    moduleName = msg.GetSource();
    if(moduleName=="") return false;

    bNew = (m_ModuleCache.find(moduleName)==m_ModuleCache.end());
    if(!bNew)
    {
        detectTime = m_ModuleCache[moduleName].GetValue("detect");
        status = m_ModuleCache[moduleName].GetValue("status");
        advanceConfig = m_ModuleCache[moduleName].GetValue("AdvanceConfig");
        if((status == "Need config")&&(!bNeedConfig)) status= "Configured";
    }
    else
    {
        status = "Not Configurable";
        advanceConfig = "NO";
    }
    if(bNeedConfig) status = "Need config";

    m_ModuleCache[moduleName] = msg;
    m_ModuleCache[moduleName].AddValue("status", status);
    m_ModuleCache[moduleName].AddValue("AdvanceConfig", advanceConfig);
	SetTimeAlive(m_ModuleCache[moduleName]);

	if(bNew)
        SetTimeDetect(m_ModuleCache[moduleName]);
    else
        m_ModuleCache[moduleName].AddValue("detect", detectTime);

	return bNew;
}

void CacheManager::ConfigAdd(xPL::SchemaObject& msg)
{
    string moduleName;

    moduleName = msg.GetSource();
    if(moduleName=="") return;

    m_ConfigCache[moduleName] = msg;
}

void CacheManager::ParamAdd(xPL::SchemaObject& msg)
{
    string moduleName;

    moduleName = msg.GetSource();
    if(moduleName=="") return;

    m_ParamsCache[moduleName] = msg;
}

void CacheManager::ModuleDelete(const string& moduleName)
{
    m_ModuleCache.erase(moduleName);
    m_ParamsCache.erase(moduleName);
    m_ConfigCache.erase(moduleName);
    AdvanceConfigDelete(moduleName);
    DevicesDelete(moduleName);
}

void CacheManager::AdvanceConfigDelete(const string& moduleName)
{
    m_AdvanceConfigCache.erase(moduleName);
}

void CacheManager::DeviceAdd(xPL::SchemaObject& msg)
{
    string deviceName;
    bool bNew;

    deviceName = msg.GetValue("device");
    if(deviceName=="") return;

    bNew = (m_DeviceCache.find(deviceName)==m_DeviceCache.end());
    m_DeviceCache[deviceName] = msg;
	SetTimeAlive(m_DeviceCache[deviceName]);
	if(bNew) SetTimeDetect(m_DeviceCache[deviceName]);
}

void CacheManager::DeviceDelete(const string& deviceName)
{
    m_DeviceCache.erase(deviceName);
}

void CacheManager::DevicesDelete(const string& moduleName)
{
    map<string, xPL::SchemaObject>::iterator it;

    it=m_DeviceCache.begin();
    while(it != m_DeviceCache.end())
    {
        if (it->second.GetSource() == moduleName)
            it = m_DeviceCache.erase(it);
        else
            ++it;
    }
}

void CacheManager::AdvanceFormatAdd(xPL::SchemaObject& msg)
{
    string moduleName;


    moduleName = msg.GetSource();
    if(moduleName=="") return;

    m_AdvanceFormatCache[moduleName] = msg;
    m_ModuleCache[moduleName].SetValue("AdvanceConfig", "YES");
}

void CacheManager::AdvanceConfigAdd(xPL::SchemaObject& msg)
{
    string moduleName;
    string deviceName;


    moduleName = msg.GetSource();
    if(moduleName=="") return;

    deviceName = msg.GetValue("configname");
    if(deviceName=="") return;

    m_AdvanceConfigCache[moduleName][deviceName] = msg;
}

map<string, xPL::SchemaObject>* CacheManager::GetModules()
{
    struct tm  now;
    struct tm  alive;
    double seconds;
    int interval;
    map<string, xPL::SchemaObject>::iterator it;
    map<string, xPL::SchemaObject>::iterator itEnd;

    now = GetTmTime();
    it = m_ModuleCache.begin();
    itEnd = m_ModuleCache.end();

    while(it != itEnd)
    {
        sscanf(it->second.GetValue("alive").c_str(),"%4d-%2d-%2d %2d:%2d:%2d", &alive.tm_year,&alive.tm_mon,&alive.tm_mday, &alive.tm_hour,&alive.tm_min,&alive.tm_sec);
        seconds = difftime(mktime(&now),mktime(&alive));

        interval = atoi(it->second.GetValue("Interval").c_str());
        if(interval==0) interval = 5;

        if(seconds > 3600)
        {
            it = m_ModuleCache.erase(it);
            ModuleDelete(it->first);
            continue;
        }

        if(seconds > interval*60+1)
        {
            if(seconds > interval*60*2+1)
                it->second.AddValue("alivectrl", "critical");
            else
                it->second.AddValue("alivectrl", "warning");
        }
        else
            it->second.AddValue("alivectrl", "ok");

        ++it;
    }

    return &m_ModuleCache;
}

map<string, xPL::SchemaObject>* CacheManager::GetDevices()
{
    struct tm  now;
    struct tm  alive;
    double seconds;
    map<string, xPL::SchemaObject>::iterator it;
    map<string, xPL::SchemaObject>::iterator itEnd;

    now = GetTmTime();
    it = m_DeviceCache.begin();
    itEnd = m_DeviceCache.end();

    while(it != itEnd)
    {
        sscanf(it->second.GetValue("alive").c_str(),"%4d-%2d-%2d %2d:%2d:%2d", &alive.tm_year,&alive.tm_mon,&alive.tm_mday, &alive.tm_hour,&alive.tm_min,&alive.tm_sec);
        seconds = difftime(mktime(&now),mktime(&alive));

        if(seconds > 3600)
        {
            it = m_DeviceCache.erase(it);
            continue;
        }

        if(seconds > 900)
        {
            if(seconds > 1800)
                it->second.AddValue("alivectrl", "critical");
            else
                it->second.AddValue("alivectrl", "warning");
        }
        else
            it->second.AddValue("alivectrl", "ok");

        ++it;
    }

    return &m_DeviceCache;
}

map<string, xPL::SchemaObject>* CacheManager::GetConfigs()
{
    return &m_ConfigCache;
}

map<string, xPL::SchemaObject>* CacheManager::GetParams()
{
    return &m_ParamsCache;
}

map<string, xPL::SchemaObject>* CacheManager::GetAdvanceFormat()
{
    return &m_AdvanceFormatCache;
}

map<string, xPL::SchemaObject>* CacheManager::GetAdvanceConfig(const std::string& moduleName)
{
    map<string, map<string, xPL::SchemaObject>>::iterator it;

    it=m_AdvanceConfigCache.find(moduleName);
    if(it==m_AdvanceConfigCache.end()) return nullptr;
    return &(it->second);
}

void CacheManager::PushCachedValue(const string& key, const xPL::SchemaObject& msg)
{
    m_CachedValuesMutex.lock();
    m_CachedValues[key].push(msg);
    m_CachedValuesMutex.unlock();
}

bool CacheManager::WaitCachedValue(string key, unsigned int timeout)
{
    std::map<std::string, queue<xPL::SchemaObject>>::iterator it;
    std::map<std::string, queue<xPL::SchemaObject>>::iterator itEnd = m_CachedValues.end();
    unsigned int nbTime;

    nbTime = timeout/50;

    while(nbTime-->0)
    {
        m_CachedValuesMutex.lock();
        it=m_CachedValues.find(key);
        m_CachedValuesMutex.unlock();
        if((it!=itEnd)&&(!it->second.empty())) return true;
        Plateforms::delay(50);
    }

    return false;
}

xPL::SchemaObject CacheManager::PopCachedValue(string key)
{
    std::map<std::string, queue<xPL::SchemaObject>>::iterator it;
    xPL::SchemaObject msg;

    m_CachedValuesMutex.lock();
    it=m_CachedValues.find(key);
    if(it!=m_CachedValues.end())
    {
        msg = it->second.front();
        it->second.pop();
    }
    m_CachedValuesMutex.unlock();

    return msg;
}
