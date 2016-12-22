#ifndef MessagesController_H
#define MessagesController_H

#include "IxPLSend.h"
#include "WebServer/IWebController.h"
#include "WebServer/Response.h"
#include "WebServer/Request.h"

class MessagesController : public IWebController
{
    public:
        MessagesController();
        ~MessagesController();

        void SetxPLSender(IxPLSend* xPLSender);
        bool Controller(Request& request, Response* response);

    private:
        IxPLSend* m_xPLSender;
};

#endif // MessagesController_H
