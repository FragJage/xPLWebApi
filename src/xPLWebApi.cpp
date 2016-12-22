#ifdef _MSC_VER
#include<WinSock2.h>
#include<Windows.h>
#endif
#include "xPLWebApi.h"
#include "Plateforms/Plateforms.h"

using namespace std;

xPLWebApi::xPLWebApi()
{
    m_bServicePause = false;
    m_bServiceStop = false;
    m_Log = m_xPLMgr.GetLogHandle();
}

xPLWebApi::~xPLWebApi()
{
}

int xPLWebApi::ServiceStart(int argc, char* argv[])
{
    xPL::SchemaHbeatRequest HbeatRequest;
    string portTCP;
    string documentRoot;

    m_bServiceStop = false;
    if(argc > 1) m_xPLMgr.SetConfigFileName(argv[1]);
    m_xPLMgr.LoadConfig();

    //Read config
    portTCP = m_xPLMgr.GetWebPort();
    documentRoot = m_xPLMgr.GetWebDocumentRoot();

    //Add WebController
    m_PagesController.SetRootDir(documentRoot);
    m_PagesController.AddDefaultPage("index.html");
    m_WebMgr.AddRoute("/web/*", &m_PagesController);

    m_ModulesController.SetCacheManager(&m_CacheMgr);
    m_ModulesController.SetxPLSender(&m_xPLMgr);
    m_WebMgr.AddRoute("/api/v1/modules/[id]", &m_ModulesController);

    m_AdvanceConfigController.SetCacheManager(&m_CacheMgr);
    m_AdvanceConfigController.SetxPLSender(&m_xPLMgr);
    m_WebMgr.AddRoute("/api/v1/advanceconfig/[id]", &m_AdvanceConfigController);

    m_DevicesController.SetCacheManager(&m_CacheMgr);
    m_DevicesController.SetxPLSender(&m_xPLMgr);
    m_WebMgr.AddRoute("/api/v1/devices/[id]", &m_DevicesController);

    m_DataloggersController.SetCacheManager(&m_CacheMgr);
    m_DataloggersController.SetxPLSender(&m_xPLMgr);
    m_WebMgr.AddRoute("/api/v1/datalogger/{search}/[device]", &m_DataloggersController);

    m_MessagesController.SetxPLSender(&m_xPLMgr);
    m_WebMgr.AddRoute("/api/v1/messages/", &m_MessagesController);

    if(!m_WebMgr.Open(portTCP))
    {
        LOG_FATAL(m_Log) << "The Web server did not start";
        return 1;
    }
    LOG_INFO(m_Log) << "The Web server starting on port " << portTCP;

    //Open xPL
    m_xPLMgr.SetCacheManager(&m_CacheMgr);
    m_xPLMgr.Open();

    //Get modules and devices
    m_xPLMgr.SendxPLMessage(&HbeatRequest, "*");


    //Poll all
    while(!m_bServiceStop)
    {
        if(m_bServicePause)
            Plateforms::delay(500);
        else
        {
            m_xPLMgr.WaitRecv(100);
            m_WebMgr.PollServer(100);
        }
    }

    //Close all
    m_xPLMgr.Close();
    m_WebMgr.Close();

    return 0;
}

void xPLWebApi::ServicePause(bool bPause)
{
    m_bServicePause = bPause;
}

void xPLWebApi::ServiceStop()
{
    m_bServiceStop = true;
}
