#include <stdlib.h>
#include "AdvanceConfigController.h"
#include "xPLLib/Schemas/SchemaHbeat.h"
#include "xPLLib/Schemas/SchemaAdvanceConfig.h"
#include "Plateforms/Plateforms.h"

using namespace std;

AdvanceConfigController::AdvanceConfigController()
{
}

AdvanceConfigController::~AdvanceConfigController()
{
}

void AdvanceConfigController::SetCacheManager(ICacheRead* cacheRead)
{
    m_CacheRead = cacheRead;
}

void AdvanceConfigController::SetxPLSender(IxPLSend* xPLSender)
{
    m_xPLSender = xPLSender;
}

bool AdvanceConfigController::Controller(Request& request, Response* response)
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
            if(action=="refresh") RefreshModule(module);
            return GetAdvanceConfig(module, response);
        }
        GetModulesList(response);
        return true;
    }

    if((method==Request::mthPUT)&&(module!=""))
    {
        PutAdvanceConfig(module, request.GetBody());
        RefreshModule(module);
        return true;
    }

    response->SetStatut(500);
    response->SetContent("500 : Internal Server Error - 0x0001 : Support only GET and PUT methods.");
    return true;
}

void AdvanceConfigController::RefreshModule(const string& module)
{
    int i;
    xPL::SchemaHbeatRequest hbeatRequest;
    xPL::SchemaAdvanceConfigCurrentCmnd advanceConfigCurrentRequest;
    map<string, xPL::SchemaObject>* modulList;
    map<string, xPL::SchemaObject>::iterator itModule;
    string myAlive;


    modulList = m_CacheRead->GetModules();
    itModule = modulList->find(module);
    if(itModule==modulList->end()) return;

    myAlive = itModule->second.GetValue("alive");

    m_CacheRead->AdvanceConfigDelete(module);
    m_xPLSender->SendxPLMessage(&advanceConfigCurrentRequest, module);
    m_xPLSender->SendxPLMessage(&hbeatRequest, module);

    for(i=0;i<10;i++)
    {
        Plateforms::delay(300);
        if(myAlive != itModule->second.GetValue("alive")) break;
    }
}

void AdvanceConfigController::PutAdvanceConfig(const string& module, const string& body)
{
    xPL::SchemaObject msg(xPL::SchemaObject::cmnd, "advanceconfig", "request");
    xPL::SchemaObject msgDelete(xPL::SchemaObject::cmnd, "advanceconfig", "current");
    Json::Value jsBody;
    Json::Value jsValue;
    Json::Value::iterator itValue;
    Json::Value::Members jsMembers;
    Json::Value::Members::iterator itMember;
    Json::Reader jsReader;
    string value;
    unsigned len;


    jsReader.parse(body, jsBody);

    if(!jsBody.isArray())
    {
        return;
    }

    msgDelete.SetValue("command", "delete");
    m_xPLSender->SendxPLMessage(&msgDelete, module);
    for(itValue=jsBody.begin(); itValue!=jsBody.end(); ++itValue)
    {
        jsMembers = itValue->getMemberNames();
        for(itMember=jsMembers.begin(); itMember!=jsMembers.end(); ++itMember)
        {
            jsValue = itValue->get(*itMember,"");
            value = jsValue.asString();
            len = value.length();
            while(len>0)
            {
                msg.AddValue(*itMember, value.substr(0, 128));
                if(len>128)
                {
                    value = value.substr(128);
                    len -= 128;
                }
                else
                    len=0;
            }
        }
        m_xPLSender->SendxPLMessage(&msg, module);
        msg.ClearValues();
    }
}

void AdvanceConfigController::GetModulesList(Response* response)
{
    map<string, xPL::SchemaObject>* modulList;
    map<string, xPL::SchemaObject>::iterator it;
    map<string, xPL::SchemaObject>::iterator itFirst;
    Json::Value JSmodulArray(Json::arrayValue);
    Json::FastWriter writer;


    modulList = m_CacheRead->GetAdvanceFormat();

    itFirst = modulList->begin();
    for(it=itFirst; it!=modulList->end(); it++)
        JSmodulArray.append(it->first);

    response->SetStatut(200);
    response->SetContent(writer.write(JSmodulArray));
}

Json::Value AdvanceConfigController::GetAdvanceConfigPartFormat(map<string, xPL::SchemaObject>::iterator itConfig)
{
    Json::Value JSformat(Json::objectValue);
    Json::Value JSformatobj(Json::objectValue);
    vector<xPL::SchemaObject::SchemaItem>::iterator itItem;
    string myvalue;
    string mylist;
    size_t pos;


    for(itItem=itConfig->second.begin(); itItem!=itConfig->second.end(); ++itItem)
    {
        myvalue = itItem->GetValue();
        mylist = "";
        JSformatobj.clear();

        pos = myvalue.find("[");
        if(pos!=std::string::npos)
        {
            mylist = myvalue.substr(pos+1, myvalue.length()-pos-2);
            myvalue = myvalue.substr(0, pos);
        }

        JSformatobj["type"] = myvalue;
        if(mylist!="") JSformatobj["list"] = mylist;
        JSformat[itItem->GetKey()] = JSformatobj;
    }
    return JSformat;
}

Json::Value AdvanceConfigController::GetAdvanceConfigPartConfig(map<string, xPL::SchemaObject>* advanceConfig, map<string, xPL::SchemaObject>::iterator itConfig)
{
    map<string, xPL::SchemaObject>::iterator itModule;
    vector<xPL::SchemaObject::SchemaItem>::iterator itItem;
    xPL::SchemaObject::SchemaItem* configItem;
    Json::Value JSconfig(Json::objectValue);
    Json::Value JSconfigArray(Json::arrayValue);
    string type;


    for(itModule=advanceConfig->begin(); itModule!=advanceConfig->end(); ++itModule)
    {
        for(itItem=itModule->second.begin(); itItem!=itModule->second.end(); ++itItem)
        {
            configItem = itConfig->second.GetItem(itItem->GetKey());
            type = configItem->GetValue();
            if(type == "integer")
            {
                JSconfig[itItem->GetKey()] = atoi(itItem->GetValue().c_str());
            }
            else if(type == "boolean")
            {
                if(itItem->GetValue()=="true")
                    JSconfig[itItem->GetKey()] = true;
                else
                    JSconfig[itItem->GetKey()] = false;
            }
            else
            {
                unsigned nb, i;
                string value;

                nb = itItem->Count();
                for(i=0; i<nb; i++)
                    value += itItem->GetValue(i);

                JSconfig[itItem->GetKey()] = value;
            }
        }

        JSconfigArray.append(JSconfig);
    }
    return JSconfigArray;
}

bool AdvanceConfigController::GetAdvanceConfig(const string& module, Response* response)
{
    map<string, xPL::SchemaObject>* advanceFormat;
    map<string, xPL::SchemaObject>* advanceConfig;
    map<string, xPL::SchemaObject>::iterator itConfig;
    Json::Value JSenvelop(Json::objectValue);
    Json::FastWriter writer;


    advanceFormat = m_CacheRead->GetAdvanceFormat();
    itConfig = advanceFormat->find(module);
    if(itConfig==advanceFormat->end())
    {
        response->SetStatut(404);
        response->SetContent("404 : Not Found - 0x0002 : Module not found.");
        return true;
    }

    JSenvelop["format"] = GetAdvanceConfigPartFormat(itConfig);

    advanceConfig = m_CacheRead->GetAdvanceConfig(module);
    if(advanceConfig!=nullptr) JSenvelop["config"] = GetAdvanceConfigPartConfig(advanceConfig, itConfig);

    response->SetStatut(200);
    response->SetContent(writer.write(JSenvelop));
    return true;
}
