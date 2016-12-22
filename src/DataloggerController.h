#ifndef DataloggerController_H
#define DataloggerController_H

#include "CacheManager.h"
#include "IxPLSend.h"
#include "jsoncpp/include/json/json.h"
#include "WebServer/IWebController.h"
#include "WebServer/Response.h"
#include "WebServer/Request.h"

class DataloggerController : public IWebController
{
    public:
        DataloggerController();
        ~DataloggerController();

        void SetCacheManager(ICacheRead* cacheRead);
        void SetxPLSender(IxPLSend* xPLSender);
        bool Controller(Request& request, Response* response);

    private:
        bool GetDevices(Response* response, const std::string& startDate, const std::string& endDate);
        bool GetValues(Response* response, const std::string& device, const std::string& startDate, const std::string& endDate, int number);
        IxPLSend* m_xPLSender;
        ICacheRead* m_CacheRead;
};

#endif // DataloggerController_H
