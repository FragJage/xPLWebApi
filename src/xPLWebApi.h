#ifndef XPLWEBAPI_H
#define XPLWEBAPI_H

#include "xPLManager.h"
#include "WebManager.h"
#include "ModulesController.h"
#include "AdvanceConfigController.h"
#include "DevicesController.h"
#include "MessagesController.h"
#include "DataloggerController.h"
#include "Service/Service.h"
#include "WebServer/PageController.h"

class xPLWebApi : public Service::IService
{
    public:
        xPLWebApi();
        ~xPLWebApi();

		int ServiceStart(int argc, char* argv[]);
		void ServicePause(bool bPause);
		void ServiceStop();

    private:
        bool m_bServicePause;
        bool m_bServiceStop;
        SimpleLog* m_Log;
        CacheManager m_CacheMgr;
        xPLManager m_xPLMgr;
        WebManager m_WebMgr;
        MessagesController m_MessagesController;
        ModulesController m_ModulesController;
        DevicesController m_DevicesController;
        AdvanceConfigController m_AdvanceConfigController;
        DataloggerController m_DataloggersController;
        PageController m_PagesController;
};

#endif // XPLWEBAPI_H
