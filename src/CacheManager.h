#ifndef CACHEMANAGER_H
#define CACHEMANAGER_H

#include <map>
#include <queue>
#include <ctime>
#include "xPLLib/Schemas/SchemaObject.h"

#ifdef WIN32
    #include "Thread/mingw.mutex.h"
#else
    #include <mutex>
#endif

class ICacheRead
{
	public:
		virtual xPL::SchemaObject PopCachedValue(std::string key) = 0;
		virtual bool WaitCachedValue(std::string key, unsigned int timeout=3000) = 0;
		virtual std::map<std::string, xPL::SchemaObject>* GetModules() = 0;
		virtual std::map<std::string, xPL::SchemaObject>* GetDevices() = 0;
		virtual std::map<std::string, xPL::SchemaObject>* GetConfigs() = 0;
		virtual std::map<std::string, xPL::SchemaObject>* GetParams() = 0;
        virtual std::map<std::string, xPL::SchemaObject>* GetAdvanceFormat() = 0;
        virtual std::map<std::string, xPL::SchemaObject>* GetAdvanceConfig(const std::string& moduleName) = 0;
		virtual void DevicesDelete(const std::string& moduleName) = 0;
		virtual void AdvanceConfigDelete(const std::string& moduleName) = 0;
};

class ICacheWrite
{
	public:
		virtual void PushCachedValue(const std::string& key, const xPL::SchemaObject& msg) = 0;
		virtual bool ModuleAdd(xPL::SchemaObject& msg, bool bNeedConfig) = 0;
        virtual void ConfigAdd(xPL::SchemaObject& msg) = 0;
        virtual void ParamAdd(xPL::SchemaObject& msg) = 0;
		virtual void DeviceAdd(xPL::SchemaObject& msg) = 0;
		virtual void AdvanceFormatAdd(xPL::SchemaObject& msg) = 0;
		virtual void AdvanceConfigAdd(xPL::SchemaObject& msg) = 0;
		virtual void ModuleDelete(const std::string& moduleName) = 0;
};

class CacheManager : public ICacheRead, public ICacheWrite
{
    public:
        CacheManager();
        ~CacheManager();

        bool ModuleAdd(xPL::SchemaObject& msg, bool bNeedConfig);
        void ModuleDelete(const std::string& moduleName);
        void ConfigAdd(xPL::SchemaObject& msg);
        void ParamAdd(xPL::SchemaObject& msg);
        void DeviceAdd(xPL::SchemaObject& msg);
        void DeviceDelete(const std::string& deviceName);
        void DevicesDelete(const std::string& moduleName);
        void AdvanceFormatAdd(xPL::SchemaObject& msg);
        void AdvanceConfigAdd(xPL::SchemaObject& msg);
        void AdvanceConfigDelete(const std::string& moduleName);
		void PushCachedValue(const std::string& key, const xPL::SchemaObject& msg);

		std::map<std::string, xPL::SchemaObject>* GetModules();
		std::map<std::string, xPL::SchemaObject>* GetDevices();
		std::map<std::string, xPL::SchemaObject>* GetConfigs();
		std::map<std::string, xPL::SchemaObject>* GetParams();
        std::map<std::string, xPL::SchemaObject>* GetAdvanceFormat();
        std::map<std::string, xPL::SchemaObject>* GetAdvanceConfig(const std::string& moduleName);
        xPL::SchemaObject PopCachedValue(std::string key);
		bool WaitCachedValue(std::string key, unsigned int timeout=3000);

    private:
        struct tm GetTmTime();
        void SetTime(xPL::SchemaObject& msg, const std::string& valueName);
        void SetTimeAlive(xPL::SchemaObject& msg);
        void SetTimeDetect(xPL::SchemaObject& msg);
        std::map<std::string, xPL::SchemaObject> m_DeviceCache;
        std::map<std::string, xPL::SchemaObject> m_ModuleCache;
        std::map<std::string, xPL::SchemaObject> m_ConfigCache;
        std::map<std::string, xPL::SchemaObject> m_ParamsCache;
        std::map<std::string, xPL::SchemaObject> m_AdvanceFormatCache;
        std::map<std::string, std::map<std::string, xPL::SchemaObject>> m_AdvanceConfigCache;
        std::map<std::string, std::queue<xPL::SchemaObject>> m_CachedValues;
        std::mutex m_CachedValuesMutex;
};

#endif // CACHEMANAGER_H
