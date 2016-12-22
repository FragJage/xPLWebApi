#ifndef WEBMANAGER_H
#define WEBMANAGER_H

#include <string>
#include "CacheManager.h"
#include "WebServer/WebServer.h"

class WebManager
{
    public:
        WebManager();
        void AddRoute(const std::string& route, IWebController* controller);
        bool Open(std::string portTCP);
        void PollServer(int delay);
        void Close();

    private:
        WebServer m_Server;
};

#endif // WEBMANAGER_H
