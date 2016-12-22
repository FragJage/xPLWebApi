#ifndef ModulesController_H
#define ModulesController_H

#include "CacheManager.h"
#include "IxPLSend.h"
#include "jsoncpp/include/json/json.h"
#include "WebServer/IWebController.h"
#include "WebServer/Response.h"
#include "WebServer/Request.h"

class ModulesController : public IWebController
{
    public:
        ModulesController();
        ~ModulesController();

        void SetCacheManager(ICacheRead* cacheRead);
        void SetxPLSender(IxPLSend* xPLSender);
        bool Controller(Request& request, Response* response);

    private:
        void GetModules(Response* response);
        bool GetModuleDetails(const std::string& module, Response* response);
        Json::Value GetModuleDetailsConfig(const std::string& module);
        std::string PutModuleConfig(const std::string& module, const std::string& body, Response* response);
        void RefreshModule(const std::string& newModule, const std::string& oldModule);
        ICacheRead* m_CacheRead;
        IxPLSend* m_xPLSender;
};

#endif // ModulesController_H
