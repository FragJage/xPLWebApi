#include "xPLSendMock.h"

using namespace std;

xPLsendMock::xPLsendMock()
{
}

xPLsendMock::~xPLsendMock()
{

}

void xPLsendMock::SetCacheManager(CacheManagerMock* cacheMgr)
{
    m_CacheMgr = cacheMgr;
}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
void xPLsendMock::SendxPLMessage(xPL::ISchema *schema, const std::string& target)
{
    string msg = schema->ToMessage("fragxpl-src.def", "fragxpl-dst.def");
    if(msg.find("hbeat.request")!=string::npos)
        m_CacheMgr->ModifyAlive();
}
#pragma GCC diagnostic pop
