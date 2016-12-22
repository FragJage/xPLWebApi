#include <stdlib.h>
#include "jsoncpp/include/json/json.h"
#include "Plateforms/Plateforms.h"
#include "MessagesController.h"
#include "xPLLib/Schemas/SchemaObject.h"
#include "xPLLib/Schemas/SchemaConfig.h"

using namespace std;

MessagesController::MessagesController()
{
}

MessagesController::~MessagesController()
{
}

void MessagesController::SetxPLSender(IxPLSend* xPLSender)
{
    m_xPLSender = xPLSender;
}

bool MessagesController::Controller(Request& request, Response* response)
{
    xPL::SchemaObject msg;
    Request::HttpMethod method;
    string value;
    string target;
    size_t pos;
    Json::Value jsRoot;
    Json::Value jsBody;
    Json::Value jsValue;
    Json::Value::iterator itValue;
    Json::Value::Members jsMembers;
    Json::Value::Members::iterator itMember;
    Json::Reader jsReader;


    method = request.GetMethod();
    if((method!=Request::mthPUT)&&(method!=Request::mthPOST))
    {
        response->SetStatut(500);
        response->SetContent("500 : Internal Server Error - 0x0001 : Support only PUT or POST methods.");
        return true;
    }

    if(!jsReader.parse(request.GetBody(), jsRoot))
    {
        response->SetStatut(500);
        response->SetContent("500 : Internal Server Error - 0x0002 : Enable to parse JSON body.");
        return true;
    }

    value = jsRoot.get("structure","").asString();
    if(value=="")
    {
        response->SetStatut(500);
        response->SetContent("500 : Internal Server Error - 0x0003 : 'structure' member are mandatory in JSON request.");
        return true;
    }

    try
    {
        msg.SetMsgType(value);
    }
    catch(const xPL::SchemaObject::Exception &e)
    {
        response->SetStatut(500);
        response->SetContent("500 : Internal Server Error - 0x0004 : Unknown message type in 'structure' member (known types xpl-cmnd, xpl-stat, xpl-trig).");
        return true;
    }

    value = jsRoot.get("class", "").asString();
    if(value=="")
    {
        response->SetStatut(500);
        response->SetContent("500 : Internal Server Error - 0x0005 : 'class' member are mandatory in JSON request.");
        return true;
    }

    pos = value.find(".");
    if(pos==string::npos)
    {
        response->SetStatut(500);
        response->SetContent("500 : Internal Server Error - 0x0006 : No point in 'class' member, ex control.basic.");
        return true;
    }
    msg.SetClass(value.substr(0, pos));
    msg.SetType(value.substr(pos+1));

    target = jsRoot.get("target", "").asString();
    if(target=="")
    {
        response->SetStatut(500);
        response->SetContent("500 : Internal Server Error - 0x0007 : 'target' member are mandatory in JSON request.");
        return true;
    }
    msg.SetTarget(target);

    jsBody = jsRoot.get("body", "");
    jsMembers = jsBody.getMemberNames();
    for(itMember=jsMembers.begin(); itMember!=jsMembers.end(); ++itMember)
    {
        jsValue = jsBody.get(*itMember,"");
        if(jsValue.isArray())
        {
            for(itValue=jsValue.begin(); itValue!=jsValue.end(); ++itValue)
                msg.AddValue(*itMember, itValue->asString());
        }
        else
            msg.AddValue(*itMember, jsValue.asString());
    }

    m_xPLSender->SendxPLMessage(&msg, target);

    return true;
}
