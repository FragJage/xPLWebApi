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

#ifndef WebServer_H
#define WebServer_H

#include <vector>
#include <string>
#include <exception>
#include "SimpleSock/SimpleSockTCP.h"
#ifdef WIN32
    #include "Thread/mingw.thread.h"
#else
    #include <thread>
#endif
#include "Request.h"
#include "Response.h"
#include "IWebController.h"

class WebServer
{
    public:
        enum ConfigSettings {PortTCP, MaxConn, TimeOut};
        class Exception;

        WebServer();
        ~WebServer();

        void PrendMutex();
        void LacheMutex();

        void Settings(ConfigSettings setting, std::string value);
        void AddRoute(const std::string& route, IWebController* controller);
        void Start();
        void Stop();
        void PollServer(int delay);
        static void ThreadClient(struct stThreadParam* threadParam);
        void Dispatcher(Request request, Response* response);
        void SendResponse(SimpleSockTCP* sockClient, Response& response);

    private:
        bool MatchUrl(Request* request, struct stController controller);
        bool m_isStarted;
        int m_portTCP;
        int m_maxConn;
        int m_timeOut;
        SimpleSockTCP m_sockServer;
        std::vector<struct stController> m_Controllers;
        std::map<SimpleSockTCP*, time_t> m_Threads;
};

class WebServer::Exception: public std::exception
{
    public:
        Exception(int number, std::string const& message) throw();
        ~Exception() throw();
        const char* what() const throw();
        int GetNumber() const throw();

    private:
        int m_number;
        std::string m_message;
};

enum ParamType { All, Required, Optional};

struct stThreadParam
{
    WebServer* webServer;
    SimpleSockTCP* sockClient;
    int timeOut;
};

struct stParam
{
    std::string name;
    ParamType paramType;
};

struct stController
{
    IWebController* controller;
    std::string fixedPartQuery;
    std::vector<struct stParam> paramPartQuery;
};
#endif // WebServer_H
