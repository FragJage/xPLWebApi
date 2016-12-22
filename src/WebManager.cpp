#include "WebManager.h"
#include <string.h>

using namespace std;


WebManager::WebManager()
{
}

void WebManager::AddRoute(const std::string& route, IWebController* controller)
{
    m_Server.AddRoute(route, controller);
}

bool WebManager::Open(string portTCP)
{
    m_Server.Settings(WebServer::PortTCP, portTCP);
    m_Server.Start();

    return true;
}

void WebManager::Close()
{
    m_Server.Stop();
}

void WebManager::PollServer(int delay)
{
    m_Server.PollServer(delay);
}
