#ifndef XPLMANAGER_H
#define XPLMANAGER_H

#include "CacheManager.h"
#include "IxPLSend.h"
#include "xPLLib/xPLDevCfg.h"
#include "xPLLib/Schemas/SchemaSensor.h"

class xPLManager : public xPL::xPLDevice::IExtension, public IxPLSend
{
    public:
        xPLManager();
        ~xPLManager();

        void SetConfigFileName(const char* fileName);
        void LoadConfig();
        SimpleLog* GetLogHandle();
        bool MsgAnswer(xPL::SchemaObject& msg);
        void Open();
        void Close();
        bool WaitRecv(int delay);
        void SendxPLMessage(xPL::ISchema *Schema, const std::string& target);

        std::string GetWebPort();
        std::string GetWebDocumentRoot();

        void SetCacheManager(ICacheWrite* cacheWrite);

    private:
        xPL::xPLDevCfg m_xPLDevice;
        ICacheWrite* m_CacheWrite;
};

#endif // XPLMANAGER_H
