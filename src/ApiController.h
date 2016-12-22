#ifndef ApiController_H
#define ApiController_H

#include "../../../WebServer/src/IWebController.h"

class ApiController : public IWebController
{
    public:
        ApiController();
        ~ApiController();

		bool Controller(Request& request, Response* response);
};

#endif // ApiController_H
