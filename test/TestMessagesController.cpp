#include "jsoncpp/include/json/json.h"
#include "TestMessagesController.h"
#include "WebServer/Response.h"
#include "WebServer/Request.h"

using namespace std;

TestMessagesController::TestMessagesController() : TestClass("TestMessagesController", this)
{
    m_MessagesController.SetxPLSender(&m_xPLsendMock);

	addTest("WrongMethod", &TestMessagesController::WrongMethod);
	addTest("InvalideJson", &TestMessagesController::InvalideJson);
	addTest("MandatoryStructure", &TestMessagesController::MandatoryStructure);
	addTest("WrongMessageType", &TestMessagesController::WrongMessageType);
	addTest("MandatoryClass", &TestMessagesController::MandatoryClass);
	addTest("WrongClass", &TestMessagesController::WrongClass);
	addTest("MandatoryTarget", &TestMessagesController::MandatoryTarget);
	addTest("SendxPLMessage", &TestMessagesController::SendxPLMessage);
}

TestMessagesController::~TestMessagesController()
{
}

bool TestMessagesController::WrongMethod()
{
    Request request;
    Response response;

    request.ParseRequest("GET /Messages HTTP/1.1");
    m_MessagesController.Controller(request, &response);
    assert(500==response.GetStatut());
    assert(string::npos!=response.GetBody().find("- 0x0001 :"));

    return true;
}

bool TestMessagesController::InvalideJson()
{
    Request request;
    Response response;

    request.ParseRequest("POST /Messages HTTP/1.1\n\n{");
    m_MessagesController.Controller(request, &response);
    assert(500==response.GetStatut());
    assert(string::npos!=response.GetBody().find("- 0x0002 :"));

    return true;
}

bool TestMessagesController::MandatoryStructure()
{
    Request request;
    Response response;

    request.ParseRequest("POST /Messages HTTP/1.1\n\n{\"tructure\":\"xpl-cmnd\"}");
    m_MessagesController.Controller(request, &response);
    assert(500==response.GetStatut());
    assert(string::npos!=response.GetBody().find("- 0x0003 :"));

    return true;
}

bool TestMessagesController::WrongMessageType()
{
    Request request;
    Response response;

    request.ParseRequest("POST /Messages HTTP/1.1\n\n{\"structure\":\"xpl-cmzz\"}");
    m_MessagesController.Controller(request, &response);
    assert(500==response.GetStatut());
    assert(string::npos!=response.GetBody().find("- 0x0004 :"));

    return true;
}

bool TestMessagesController::MandatoryClass()
{
    Request request;
    Response response;

    request.ParseRequest("POST /Messages HTTP/1.1\n\n{\"structure\":\"xpl-cmnd\"}");
    m_MessagesController.Controller(request, &response);
    assert(500==response.GetStatut());
    assert(string::npos!=response.GetBody().find("- 0x0005 :"));

    return true;
}

bool TestMessagesController::WrongClass()
{
    Request request;
    Response response;

    request.ParseRequest("POST /Messages HTTP/1.1\n\n{\"structure\":\"xpl-cmnd\",\"class\":\"sensor\"}");
    m_MessagesController.Controller(request, &response);
    assert(500==response.GetStatut());
    assert(string::npos!=response.GetBody().find("- 0x0006 :"));

    return true;
}

bool TestMessagesController::MandatoryTarget()
{
    Request request;
    Response response;

    request.ParseRequest("POST /Messages HTTP/1.1\n\n{\"structure\":\"xpl-cmnd\",\"class\":\"sensor.basic\"}");
    m_MessagesController.Controller(request, &response);
    assert(500==response.GetStatut());
    assert(string::npos!=response.GetBody().find("- 0x0007 :"));

    return true;
}

bool TestMessagesController::SendxPLMessage()
{
    Request request;
    Response response;

    request.ParseRequest("POST /Messages HTTP/1.1\n\n{\"structure\":\"xpl-cmnd\",\"class\":\"sensor.request\",\"target\":\"fragxpl-test.default\",\"body\":{\"request\":\"current\"}}");
    m_MessagesController.Controller(request, &response);
    assert(200==response.GetStatut());

    return true;
}
