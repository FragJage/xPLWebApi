#include "xPLManager.h"
#include "xPLLib/Schemas/SchemaAdvanceConfig.h"

using namespace std;

xPLManager::xPLManager()
{
    m_xPLDevice.Initialisation("fragxpl", "webapi", "default");
    m_xPLDevice.SetAppName("xPL WebApi", "1.1.0.0");
    m_xPLDevice.SetAnswerAllMsg(true);

    m_xPLDevice.AddExtension(this);

    m_xPLDevice.AddBasicConfig("webdoc", xPL::ConfigItem::Config, ".");
    m_xPLDevice.AddBasicConfig("webport", xPL::ConfigItem::Config, "80");
}

xPLManager::~xPLManager()
{
}

void xPLManager::SetConfigFileName(const char* fileName)
{
	m_xPLDevice.SetConfigFileName(fileName);
}

void xPLManager::LoadConfig()
{
    m_xPLDevice.LoadConfig();
}

SimpleLog* xPLManager::GetLogHandle()
{
    return m_xPLDevice.GetLogHandle();
}

bool xPLManager::MsgAnswer(xPL::SchemaObject& msg)
{
    string device;
    string sClass;
    string sType;
    string source;
    xPL::ISchema::MsgType msgType;


    msgType = msg.GetMsgType();

    if(msgType == xPL::SchemaObject::stat)
    {
        sClass = msg.GetClass();
        sType = msg.GetType();
        source = msg.GetSource();

        if(sClass=="config")
        {
            if((sType=="basic")||(sType=="app"))
            {
                if(m_CacheWrite->ModuleAdd(msg, true))
                {
                    xPL::SchemaConfigListRequest configListRequest;
                    xPL::SchemaConfigCurrentRequest configCurrentRequest;
                    xPL::SchemaAdvanceConfigListRequest advanceConfigListRequest;

                    m_xPLDevice.SendMessage(&configListRequest, source);
                    m_xPLDevice.SendMessage(&configCurrentRequest, source);
                    m_xPLDevice.SendMessage(&advanceConfigListRequest, source);
                }
            }
            if(sType=="end") m_CacheWrite->ModuleDelete(source);
            if(sType=="list") m_CacheWrite->ParamAdd(msg);
            if(sType=="current") m_CacheWrite->ConfigAdd(msg);
        }

        if(sClass=="advanceconfig")
        {
            if(sType=="list") m_CacheWrite->AdvanceFormatAdd(msg);
            if(sType=="current") m_CacheWrite->AdvanceConfigAdd(msg);
            return true;
        }

        if(sClass=="hbeat")
        {
            if((sType=="basic")||(sType=="app"))
            {
                if(m_CacheWrite->ModuleAdd(msg, false))
                {
                    xPL::SchemaConfigListRequest configListRequest;
                    xPL::SchemaConfigCurrentRequest configCurrentRequest;
                    xPL::SchemaSensorRequest sensorRequest;
                    xPL::SchemaAdvanceConfigListRequest advanceConfigListRequest;
                    xPL::SchemaAdvanceConfigCurrentCmnd advanceConfigCurrentRequest;

                    m_xPLDevice.SendMessage(&configListRequest, source);
                    m_xPLDevice.SendMessage(&configCurrentRequest, source);
                    m_xPLDevice.SendMessage(&sensorRequest, source);
                    m_xPLDevice.SendMessage(&advanceConfigListRequest, source);
                    m_xPLDevice.SendMessage(&advanceConfigCurrentRequest, source);
                }
            }
            if(sType=="end") m_CacheWrite->ModuleDelete(source);
        }

        if((sClass=="datalogger")&&(sType=="basic"))
        {
            string response = msg.GetValue("response");
            string key;

            if(response == "value")
                key = "datalogger-values-"+msg.GetValue("device");

            if(response == "device")
                key = "datalogger-device-"+msg.GetValue("starttime")+"-"+msg.GetValue("endtime");

            if(key=="") return true;

            m_CacheWrite->PushCachedValue(key, msg);
            return true;
        }
    }

    if((msgType == xPL::SchemaObject::trig)||(msgType == xPL::SchemaObject::stat))
    {
        device = msg.GetValue("device");
        if(device!="") m_CacheWrite->DeviceAdd(msg);
        return true;
    }

    return false;
}

string xPLManager::GetWebPort()
{
    xPL::ConfigItem *pConfigWebApi;

   	pConfigWebApi = m_xPLDevice.GetConfigItem("webport");
    if(pConfigWebApi==NULL) return "80";
    return pConfigWebApi->GetValue();
}

string xPLManager::GetWebDocumentRoot()
{
    xPL::ConfigItem *pConfigWebApi;

    pConfigWebApi = m_xPLDevice.GetConfigItem("webdoc");
    if(pConfigWebApi==NULL) return ".";
    return pConfigWebApi->GetValue();
}

void xPLManager::SetCacheManager(ICacheWrite* cacheWrite)
{
	m_CacheWrite = cacheWrite;
}

void xPLManager::Open()
{
    m_xPLDevice.Open();
}

void xPLManager::Close()
{
    m_xPLDevice.Close();
}

bool xPLManager::WaitRecv(int delay)
{
    return m_xPLDevice.WaitRecv(delay);
}

void xPLManager::SendxPLMessage(xPL::ISchema *schema, const string& target)
{
    m_xPLDevice.SendMessage(schema, target);
}
