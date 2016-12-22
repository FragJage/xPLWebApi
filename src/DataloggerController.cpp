#include "DataloggerController.h"
#include "xPLLib/Schemas/SchemaObject.h"

using namespace std;

DataloggerController::DataloggerController()
{
}

DataloggerController::~DataloggerController()
{
}

void DataloggerController::SetxPLSender(IxPLSend* xPLSender)
{
    m_xPLSender = xPLSender;
}

void DataloggerController::SetCacheManager(ICacheRead* cacheRead)
{
    m_CacheRead = cacheRead;
}

bool DataloggerController::Controller(Request& request, Response* response)
{
    string searchType;
    string device;
    string startDate;
    string endDate;
    int number = 1;


    searchType  = request.GetParameter("search");
    device      = request.GetParameter("device");
    startDate   = request.GetParameter("start");
    endDate     = request.GetParameter("end");
    if(request.ExistsParameter("number"))
        number  = request.GetParameter<int>("number");

    if(request.GetMethod()!=Request::mthGET)
    {
        response->SetStatut(500);
        response->SetContent("500 : Internal Server Error - 0x0001 : Support only GET method.");
        return true;
    }

    if(searchType=="devices") return GetDevices(response, startDate, endDate);
    if(searchType=="values") return GetValues(response, device, startDate, endDate, number);

    response->SetStatut(404);
    response->SetContent("404 : Not Found - 0x0001 : Unknown search.");
    return true;
}

bool DataloggerController::GetDevices(Response* response, const string& startDate, const string& endDate)
{
    Json::Value JSenveloppe(Json::objectValue);
    Json::Value JSarray(Json::arrayValue);
    Json::FastWriter writer;
    string key = "datalogger-device-"+startDate+"-"+endDate;
    string value;
    size_t i;
	xPL::SchemaObject::SchemaItem* pItem;


    xPL::SchemaObject msg(xPL::SchemaObject::cmnd, "datalogger", "basic");
    msg.AddValue("command", "getdevices");
    msg.AddValue("starttime", startDate);
    msg.AddValue("endtime", endDate);

    m_xPLSender->SendxPLMessage(&msg, "*");

    if(!m_CacheRead->WaitCachedValue(key))
    {
        response->SetStatut(504);
        response->SetContent("504 : Gateway timeout - 0x0001 : Timeout from xPL network.");
        return true;
    }

    msg = m_CacheRead->PopCachedValue(key);
    pItem = msg.GetItem("device");
    for(i=0; i<pItem->Count(); i++)
    {
        value = msg.GetValue("device", i);
        JSarray.append(value);
    }

    JSenveloppe["devices"] = JSarray;
    response->SetStatut(200);
    response->SetContent(writer.write(JSenveloppe));

    return true;
}

bool DataloggerController::GetValues(Response* response, const string& device, const string& startDate, const string& endDate, int number)
{
    Json::Value JSenveloppe(Json::objectValue);
    Json::Value JSarray(Json::arrayValue);
    Json::Value JSvalue(Json::objectValue);
    Json::FastWriter writer;
    string key = "datalogger-values-"+device;

    xPL::SchemaObject msg(xPL::SchemaObject::cmnd, "datalogger", "basic");
    msg.AddValue("command", "getvalues");
    msg.AddValue("device", device);
    msg.AddValue("starttime", startDate);
    msg.AddValue("endtime", endDate);
    msg.AddValue("numbervalue", number);

    m_xPLSender->SendxPLMessage(&msg, "*");

    if(!m_CacheRead->WaitCachedValue(key))
    {
        response->SetStatut(504);
        response->SetContent("504 : Gateway timeout - 0x0001 : Timeout from xPL network.");
        return true;
    }

    do
    {
        msg = m_CacheRead->PopCachedValue(key);
        JSvalue["time"] = msg.GetValue("time");
        JSvalue["value"] = msg.GetValue("value");
        JSvalue["min"] = msg.GetValue("min");
        JSvalue["max"] = msg.GetValue("max");
        JSarray.append(JSvalue);
    } while(m_CacheRead->WaitCachedValue(key, 500));

    JSenveloppe["values"] = JSarray;
    response->SetStatut(200);
    response->SetContent(writer.write(JSenveloppe));

    return true;
}
