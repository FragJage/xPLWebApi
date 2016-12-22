/*** LICENCE ***************************************************************************************/
/*
  WebServer - Simple class to manage a HTTP web server

  This file is part of WebServer.

    WebServer is free software : you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    WebServer is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with WebServer.  If not, see <http://www.gnu.org/licenses/>.
*/
/***************************************************************************************************/

#include <iostream>

#include <ctime>
#include "WebServer.h"
#include "Request.h"

using namespace std;

void WebServer::ThreadClient(struct stThreadParam* threadParam)
{
    WebServer* webServer = threadParam->webServer;
    SimpleSockTCP* sockClient = threadParam->sockClient;
    int timeOut = threadParam->timeOut;
    Request request;
    Response response;
    string buffer;
    string requestStr;
    bool ok;
    bool complete;
    bool keepAlive = true;


    sockClient->Blocking(true);

	while(keepAlive)
	{
	    keepAlive = false;
	    requestStr = "";

	    if(!sockClient->WaitRecv(timeOut)) continue;

        do
        {
            try
            {
                ok = sockClient->Recv(buffer);
            }
            catch(const std::exception& e)
            {
                ok = false;
            }

            requestStr+=buffer;

            if(ok) ok = request.ParseRequest(requestStr);
            if(ok) complete = (request.GetBody().size()>=(unsigned)atoi(request.GetHeader("Content-Length").c_str()));

        } while((ok)&&(!complete));

        if(!ok) continue;

        response.Clear();
        buffer = request.GetHeader("Connection");
        if(buffer=="keep-alive") keepAlive = true;
        response.SetHeader("Connection", buffer);

        webServer->Dispatcher(request, &response);
        //cout << response.GetStatut() << " : " << request.GetUri() << endl;
        webServer->SendResponse(sockClient, response);
   	    response.Clear();
	}
    sockClient->Close();
    delete sockClient;
    delete threadParam;
}

WebServer::WebServer()
{
    m_portTCP = 80;
    m_maxConn = 5;
    m_timeOut = 10;

    m_isStarted = false;
}

WebServer::~WebServer()
{
}

void WebServer::Settings(ConfigSettings setting, string value)
{
    if(m_isStarted)
    {
        throw WebServer::Exception(0x0011, "WebServer::Settings: unable to config the server when is start");
    }

    switch(setting)
    {
        case WebServer::PortTCP :
            m_portTCP = atoi(value.c_str());
            break;
        case WebServer::MaxConn :
            m_maxConn = atoi(value.c_str());
            break;
        case WebServer::TimeOut :
            m_timeOut = atoi(value.c_str());
            break;
        default :
            throw WebServer::Exception(0x0012, "WebServer::Settings: unknown ConfigSettings");
    }
}

void WebServer::Start()
{
    m_sockServer.Listen(m_portTCP);
    m_sockServer.Blocking(false);
    m_isStarted = true;
}

void WebServer::Stop()
{
    m_sockServer.Close();
    m_isStarted = false;
}

void WebServer::PollServer(int delay)
{
    SimpleSockTCP* sockClient;
    struct stThreadParam* threadParam;


    if(!m_sockServer.WaitRecv(delay)) return;

    sockClient = new SimpleSockTCP;
    if(!m_sockServer.Accept(sockClient))
    {
        delete sockClient;
        return;
    }

    sockClient->Blocking(false);       //Indispensable sous linux
    threadParam = new stThreadParam;
    threadParam->webServer = this;
    threadParam->sockClient = sockClient;
    threadParam->timeOut = m_timeOut*1000000;

    thread threadClient(ThreadClient, threadParam);
    threadClient.detach();
}

void WebServer::AddRoute(const string& route, IWebController* controller)
{   // /web/*
    // /api/v1/{controller}/[id]
    // /api/v1/modules/[id]
    // /api/v1/devices/[id]/*

    size_t pos, posMax;
    struct stController myCtrl;
    struct stParam myParam;
    char mode, car;

    mode = 'F';
    posMax = route.length();
    for(pos=0; pos < posMax; pos++)
    {
        car = route.at(pos);

        if(car=='*')
        {
            if(pos+1!=posMax) throw WebServer::Exception(0x0021, "WebServer::AddRoute: * must be at the end");
            myParam.name = "ALL";
            myParam.paramType = ParamType::All;
            myCtrl.paramPartQuery.push_back(myParam);
            continue;
        }

        if((car=='{')||(car=='['))
        {
            mode = 'P';
            myParam.name = "";
            continue;
        }

        if((car=='}')||(car==']'))
        {
            mode = 'C';
            myParam.paramType = (car=='}') ? ParamType::Required : ParamType::Optional;
            myCtrl.paramPartQuery.push_back(myParam);
            continue;
        }

        switch(mode)
        {
            case 'F' :
                myCtrl.fixedPartQuery += car;
                break;
            case 'P' :
                myParam.name += car;
                break;
            case 'C' :
                if(car!='/') throw WebServer::Exception(0x0021, "WebServer::AddRoute: invalid route");
        }
    }

    myCtrl.controller = controller;

    m_Controllers.push_back(myCtrl);
}

bool WebServer::MatchUrl(Request* request, struct stController controller)
{   // /web/*
    // /api/v1/{controller}/[id]
    // /api/v1/modules/[id]
    // /api/v1/devices/[id]/*

    size_t posDeb, posFin, lenQuery;
    string query, route, querytest;
    vector<struct stParam>::iterator it;

    query = request->GetUri();
    route = controller.fixedPartQuery;
    posDeb = route.length();
    lenQuery = query.length();


    querytest = query+"/";
    if(route!=querytest.substr(0, posDeb)) return false;

    for(it=controller.paramPartQuery.begin(); it!=controller.paramPartQuery.end(); ++it)
    {
        if((posDeb<lenQuery)&&(query.at(posDeb)=='/')) posDeb++;
        if(posDeb>lenQuery)
        {
            if(it->paramType==ParamType::Required) return false;
            break;
        }

        posFin = query.find('/', posDeb);
        if(posFin==string::npos) posFin=lenQuery+1;

        if(it->name=="ALL")
            request->SetParameter(it->name, query.substr(posDeb));
        else
            request->SetParameter(it->name, query.substr(posDeb, posFin-posDeb));
        posDeb=posFin+1;
    }

    return true;
}

void WebServer::Dispatcher(Request request, Response* response)
{
    std::stringstream stream;
    vector<struct stController>::iterator it;
    bool match = false;


    response->SetHeader("Server", "webServer/1.0.0");
    stream << "timeout=" << m_timeOut << ", max=" << m_maxConn;
    response->SetHeader("Keep-Alive", stream.str());

    for(it=m_Controllers.begin(); it!=m_Controllers.end(); ++it)
    {
        if(MatchUrl(&request, *it))
        {
            if(it->controller->Controller(request, response))
            {
                match = true;
                break;
            }
        }
    }

    if(!match)
    {
        response->SetStatut(404);
        response->SetContent("<!DOCTYPE html><html><head><title>404 Not Found</title></head><body><h1>WebServer</h1><h2>404 Not Found</h2></body></html>");
    }
}

void WebServer::SendResponse(SimpleSockTCP* sockClient, Response& response)
{
    int bufSize;
    char* bufPtr;

    while((bufPtr=response.GetSendPointer(&bufSize))!=nullptr)
        sockClient->Send(bufPtr, bufSize);
}

/***************************************************************************************************/
/***                                                                                             ***/
/*** Class WebServer::Exception                                                                 ***/
/***                                                                                             ***/
/***************************************************************************************************/
WebServer::Exception::Exception(int number, const string& message) throw()
{
    m_number = number;
    m_message = message;
}

WebServer::Exception::~Exception() throw()
{
}

const char* WebServer::Exception::what() const throw()
{
    return m_message.c_str();
}

int WebServer::Exception::GetNumber() const throw()
{
    return m_number;
}
