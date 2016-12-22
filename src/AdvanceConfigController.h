#ifndef AdvanceConfigController_H
#define AdvanceConfigController_H

#include "CacheManager.h"
#include "IxPLSend.h"
#include "jsoncpp/include/json/json.h"
#include "WebServer/IWebController.h"
#include "WebServer/Response.h"
#include "WebServer/Request.h"

class AdvanceConfigController : public IWebController
{
    public:
        AdvanceConfigController();
        ~AdvanceConfigController();

        void SetCacheManager(ICacheRead* cacheRead);
        void SetxPLSender(IxPLSend* xPLSender);
        bool Controller(Request& request, Response* response);

    private:
        void GetModulesList(Response* response);
        bool GetAdvanceConfig(const std::string& module, Response* response);
        void PutAdvanceConfig(const std::string& module, const std::string& body);
        void RefreshModule(const std::string& module);
        Json::Value GetAdvanceConfigPartFormat(std::map<std::string, xPL::SchemaObject>::iterator itConfig);
        Json::Value GetAdvanceConfigPartConfig(std::map<std::string, xPL::SchemaObject>* advanceConfig, std::map<std::string, xPL::SchemaObject>::iterator itConfig);
        ICacheRead* m_CacheRead;
        IxPLSend* m_xPLSender;
};

#endif // ModulesController_H
