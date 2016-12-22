#ifndef xPLSendMock_H
#define xPLSendMock_H

#include "../src/IxPLSend.h"
#include "CacheManagerMock.h"

using namespace std;

class xPLsendMock : public IxPLSend
{
public:
    xPLsendMock();
    ~xPLsendMock();

    void SetCacheManager(CacheManagerMock* cacheMgr);
    void SendxPLMessage(xPL::ISchema *schema, const std::string& target);

private:
    CacheManagerMock* m_CacheMgr;
};
#endif //xPLSendMock_H
