#include <stdlib.h>
#include "jsoncpp/include/json/json.h"
#include "Plateforms/Plateforms.h"
#include "DevicesController.h"
#include "xPLLib/Schemas/SchemaObject.h"
#include "xPLLib/Schemas/SchemaConfig.h"

using namespace std;

DevicesController::DevicesController()
{
}

DevicesController::~DevicesController()
{
}

void DevicesController::SetCacheManager(ICacheRead* cacheRead)
{
    m_CacheRead = cacheRead;
}

void DevicesController::SetxPLSender(IxPLSend* xPLSender)
{
    m_xPLSender = xPLSender;
}

bool DevicesController::Controller(Request& request, Response* response)
{
    string device;
    Request::HttpMethod method;


    method = request.GetMethod();
    device = request.GetParameter("id");

    if(method==Request::mthGET)
    {
        if(device!="")
        {
            return GetDeviceDetails(device, response);
        }
        GetDevices(response);
        return true;
    }

    response->SetStatut(500);
    response->SetContent("500 : Internal Server Error - 0x0001 : Support only GET method.");
    return true;
}

void DevicesController::GetDevices(Response* response)
{
    map<string, xPL::SchemaObject>* deviceList;
    map<string, xPL::SchemaObject>::iterator it;
    map<string, xPL::SchemaObject>::iterator itEnd;
    size_t posPoint;
    string module;
    string instance;
    Json::Value JSenveloppe(Json::objectValue);
    Json::Value JSdevicesArray(Json::arrayValue);
    Json::Value JSdevice(Json::objectValue);
    Json::FastWriter writer;


    deviceList = m_CacheRead->GetDevices();

    itEnd = deviceList->end();
    for(it=deviceList->begin(); it!=itEnd; it++)
    {
        module = it->second.GetSource();
        instance = "";
        posPoint = module.find('.');
        if(posPoint!=string::npos)
        {
            instance = module.substr(posPoint+1);
            module = module.substr(0, posPoint);
        }
        JSdevice["name"] = it->first;
        JSdevice["schema"] = it->second.GetClass() +"."+ it->second.GetType();
        JSdevice["module"] = module;
        JSdevice["instance"] = instance;
        JSdevice["type"] = it->second.GetValue("type");
        JSdevice["value"] = it->second.GetValue("current");
        JSdevice["alive"] = it->second.GetValue("alive");
        JSdevicesArray.append(JSdevice);
    }
    JSenveloppe["devices"] = JSdevicesArray;
    response->SetStatut(200);
    response->SetContent(writer.write(JSenveloppe));
}

bool DevicesController::GetDeviceDetails(const string& module, Response* response)
{
    map<string, xPL::SchemaObject>* modulList;
    map<string, xPL::SchemaObject>::iterator itModule;
    vector<xPL::SchemaObject::SchemaItem>::iterator itItem;
    Json::Value JSdevice(Json::objectValue);
    Json::FastWriter writer;


    modulList = m_CacheRead->GetDevices();
    itModule = modulList->find(module);
    if(itModule==modulList->end())
    {
        response->SetStatut(404);
        response->SetContent("404 : Not Found - 0x0002 : Device not found.");
        return true;
    }

    JSdevice["schema"] = itModule->second.GetClass() +"."+ itModule->second.GetType();
    JSdevice["source"] = itModule->second.GetSource();

    for(itItem=itModule->second.begin(); itItem!=itModule->second.end(); ++itItem)
        JSdevice[itItem->GetKey()] = itItem->GetValue();

    response->SetStatut(200);
    response->SetContent(writer.write(JSdevice));
    return true;
}
