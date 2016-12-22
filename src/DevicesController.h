#ifndef DevicesController_H
#define DevicesController_H

#include "CacheManager.h"
#include "IxPLSend.h"
#include "WebServer/IWebController.h"
#include "WebServer/Response.h"
#include "WebServer/Request.h"

class DevicesController : public IWebController
{
    public:
        DevicesController();
        ~DevicesController();

        void SetCacheManager(ICacheRead* cacheRead);
        void SetxPLSender(IxPLSend* xPLSender);
        bool Controller(Request& request, Response* response);

    private:
        void GetDevices(Response* response);
        bool GetDeviceDetails(const std::string& device, Response* response);
        ICacheRead* m_CacheRead;
        IxPLSend* m_xPLSender;
};

#endif // DevicesController_H
