#include <stdlib.h>
#include "Plateforms/Plateforms.h"
#include "ModulesController.h"
#include "xPLLib/Schemas/SchemaObject.h"
#include "xPLLib/Schemas/SchemaConfig.h"
#include "xPLLib/Schemas/SchemaSensor.h"
#include "xPLLib/Schemas/SchemaAdvanceConfig.h"

using namespace std;

ModulesController::ModulesController()
{
}

ModulesController::~ModulesController()
{
}

void ModulesController::SetCacheManager(ICacheRead* cacheRead)
{
    m_CacheRead = cacheRead;
}

void ModulesController::SetxPLSender(IxPLSend* xPLSender)
{
    m_xPLSender = xPLSender;
}

bool ModulesController::Controller(Request& request, Response* response)
{
    string action;
    string module;
    Request::HttpMethod method;


    method = request.GetMethod();
    module = request.GetParameter("id");
    action = request.GetParameter("action");

    if(method==Request::mthGET)
    {
        if(module!="")
        {
            if(action=="refresh") RefreshModule(module, module);
            return GetModuleDetails(module, response);
        }
        GetModules(response);
        return true;
    }

    if((method==Request::mthPUT)&&(module!=""))
    {
        string newModule;
        newModule = PutModuleConfig(module, request.GetBody(), response);
        RefreshModule(newModule, module);
        return true;
    }

    response->SetStatut(500);
    response->SetContent("500 : Internal Server Error - 0x0001 : Support only GET and PUT methods.");
    return true;
}

void ModulesController::RefreshModule(const string& newModule, const string& oldModule)
{
    int i;
    xPL::SchemaConfigListRequest configListRequest;
    xPL::SchemaConfigCurrentRequest configCurrentRequest;
    xPL::SchemaHbeatRequest hbeatRequest;
    xPL::SchemaSensorRequest sensorRequest;
    xPL::SchemaAdvanceConfigCurrentCmnd advanceConfigCurrentRequest;
    map<string, xPL::SchemaObject>* modulList;
    map<string, xPL::SchemaObject>::iterator itModule;
    string myAlive;


    modulList = m_CacheRead->GetModules();
    itModule = modulList->find(oldModule);
    if(itModule==modulList->end())
    {
        if(oldModule==newModule) return;
        myAlive = "";
    }
    else
    {
        m_CacheRead->DevicesDelete(oldModule);
        myAlive = itModule->second.GetValue("alive");
    }

    m_xPLSender->SendxPLMessage(&configListRequest, newModule);
    m_xPLSender->SendxPLMessage(&configCurrentRequest, newModule);
    m_xPLSender->SendxPLMessage(&hbeatRequest, newModule);
    m_xPLSender->SendxPLMessage(&sensorRequest, newModule);
    m_xPLSender->SendxPLMessage(&advanceConfigCurrentRequest, newModule);

    for(i=0;i<20;i++)
    {
        Plateforms::delay(450);
        modulList = m_CacheRead->GetModules();
        itModule = modulList->find(newModule);
        if(itModule==modulList->end()) continue;
        if(myAlive != itModule->second.GetValue("alive")) break;
    }
}

string ModulesController::PutModuleConfig(const string& module, const string& body, Response* response)
{
    xPL::SchemaObject msg(xPL::SchemaObject::cmnd, "config", "response");
    Json::Value jsBody;
    Json::Value jsValue;
    Json::Value::iterator itValue;
    Json::Value::Members jsMembers;
    Json::Value::Members::iterator itMember;
    Json::Reader jsReader;
    string newModule = "";
    size_t pos;


    jsReader.parse(body, jsBody);

    jsMembers = jsBody.getMemberNames();
    for(itMember=jsMembers.begin(); itMember!=jsMembers.end(); ++itMember)
    {
        jsValue = jsBody.get(*itMember,"").get("value", "");
        if(jsValue.isArray())
        {
            for(itValue=jsValue.begin(); itValue!=jsValue.end(); ++itValue)
                msg.AddValue(*itMember, itValue->asString());
        }
        else
        {
            msg.AddValue(*itMember, jsValue.asString());
        }
    }

    m_xPLSender->SendxPLMessage(&msg, module);

    pos = module.find(".");
    if(pos != string::npos)
    {
        jsValue = jsBody.get("newconf","").get("value", "");
        if(jsValue.asString()!="")
        {
            newModule = module.substr(0, pos+1)+jsValue.asString();
        }
    }

    if(newModule=="") newModule = module;

    response->SetStatut(200);
    response->SetContent(newModule);

    return newModule;
}

void ModulesController::GetModules(Response* response)
{
    map<string, xPL::SchemaObject>* modulList;
    map<string, xPL::SchemaObject>::iterator it;
    map<string, xPL::SchemaObject>::iterator itFirst;
    Json::Value JSenveloppe(Json::objectValue);
    Json::Value JSmodulArray(Json::arrayValue);
    Json::Value JSmodule(Json::objectValue);
    Json::FastWriter writer;


    modulList = m_CacheRead->GetModules();

    itFirst = modulList->begin();
    for(it=itFirst; it!=modulList->end(); it++)
    {
        JSmodule["name"] = it->first;
        JSmodule["status"] = it->second.GetValue("status");
        JSmodule["alive"] = it->second.GetValue("alive");
        JSmodulArray.append(JSmodule);
    }

    JSenveloppe["modules"] = JSmodulArray;
    response->SetStatut(200);
    response->SetContent(writer.write(JSenveloppe));
}

bool ModulesController::GetModuleDetails(const string& module, Response* response)
{
    map<string, xPL::SchemaObject>* modulList;
    map<string, xPL::SchemaObject>::iterator itModule;
    vector<xPL::SchemaObject::SchemaItem>::iterator itItem;
    Json::Value JSmodule(Json::objectValue);
    Json::FastWriter writer;


    modulList = m_CacheRead->GetModules();
    itModule = modulList->find(module);
    if(itModule==modulList->end())
    {
        response->SetStatut(404);
        response->SetContent("404 : Not Found - 0x0002 : Module not found.");
        return true;
    }

    JSmodule["name"] = itModule->first.c_str();

    for(itItem=itModule->second.begin(); itItem!=itModule->second.end(); ++itItem)
        JSmodule[itItem->GetKey()] = itItem->GetValue();

    JSmodule["config"] = GetModuleDetailsConfig(module);

    response->SetStatut(200);
    response->SetContent(writer.write(JSmodule));
    return true;
}

Json::Value ModulesController::GetModuleDetailsConfig(const string& module)
{
    std::map<std::string, xPL::SchemaObject>* paramList;
    std::map<std::string, xPL::SchemaObject>* configList;
    std::map<std::string, xPL::SchemaObject>::iterator itModule;
    xPL::SchemaObject* paramSchema;
    xPL::SchemaObject* configSchema;
    vector<xPL::SchemaObject::SchemaItem>::iterator itItem;
    xPL::SchemaObject::SchemaItem* configItem;
    xPL::SchemaObject::SchemaItem::Iterator itConfig;
    xPL::SchemaObject::SchemaItem::Iterator itValue;
    xPL::SchemaObject::SchemaItem::Iterator itValueEnd;
    string itemName;
    string itemType;
    int itemMax;
	size_t posDeb;
    Json::Value JSconfigList(Json::objectValue);
    Json::Value JSconfig(Json::objectValue);
    Json::Value JSvalueArray(Json::arrayValue);


    configList = m_CacheRead->GetConfigs();
    itModule = configList->find(module);
    configSchema = nullptr;
    if(itModule!=configList->end()) configSchema = &itModule->second;

    paramList = m_CacheRead->GetParams();
    itModule = paramList->find(module);
    if(itModule==paramList->end()) return "";
    paramSchema = &itModule->second;

    for(itItem = paramSchema->begin(); itItem != paramSchema->end(); ++itItem)
    {
        for(itConfig = itItem->begin(); itConfig != itItem->end(); ++itConfig)
        {
            itemMax = 1;
            itemType = itItem->GetKey();
            itemName = *itConfig;
            posDeb = itemName.find("[");
            if(posDeb!=string::npos)
            {
                itemMax = atoi(itemName.substr(posDeb+1).c_str());
                itemName = itemName.substr(0, posDeb);
            }

            JSconfig["type"] = itemType;
            JSconfig["max"] = itemMax;

            if(configSchema!=nullptr) configItem = configSchema->GetItem(itemName);
            if((configSchema==nullptr)||(configItem==nullptr)) itemMax = 0;

            if(itemMax==0) JSconfig["value"] = "";
            if(itemMax==1) JSconfig["value"] = configItem->GetValue();
            if(itemMax>1)
            {
                itValueEnd = configItem->end();
                for(itValue = configItem->begin(); itValue != itValueEnd; ++itValue)
                    JSvalueArray.append(*itValue);
                JSconfig["value"] = JSvalueArray;
            }

            JSconfigList[itemName] = JSconfig;
            JSvalueArray.clear();
        }
    }
    return JSconfigList;
}
