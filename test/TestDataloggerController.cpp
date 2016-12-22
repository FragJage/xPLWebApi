//#include "jsoncpp/include/json/json.h"
#include "TestDataloggerController.h"
#include "WebServer/Response.h"
#include "WebServer/Request.h"

using namespace std;

TestDataloggerController::TestDataloggerController() : TestClass("TestDataloggerController", this)
{
    m_xPLsendMock.SetCacheManager(&m_CacheManagerMock);
    m_DataloggerController.SetCacheManager(&m_CacheManagerMock);
    m_DataloggerController.SetxPLSender(&m_xPLsendMock);

	addTest("GetDevices", &TestDataloggerController::GetDevices);
	addTest("GetValues", &TestDataloggerController::GetValues);
	addTest("WrongSearch", &TestDataloggerController::WrongSearch);
	addTest("WrongPOST", &TestDataloggerController::WrongPOST);
}

TestDataloggerController::~TestDataloggerController()
{
}

bool TestDataloggerController::GetDevices()
{
    Request request;
    Response response;
    Json::Reader jsReader;
    Json::Value jsBody;
    Json::Value jsDevices;

    request.ParseRequest("GET /Datalogger?search=devices&start=20100101&end=20101231 HTTP/1.1");
    m_DataloggerController.Controller(request, &response);

    jsReader.parse(response.GetBody(), jsBody);
    jsDevices = jsBody.get("devices","");

    assert(true==jsDevices.isArray());
    assert(2==jsDevices.size());
    assert("fragxpl-moduletwo.default:device1"==jsDevices[0].asString());
    assert("fragxpl-moduletwo.default:device2"==jsDevices[1].asString());

    return true;
}

bool TestDataloggerController::GetValues()
{
    Request request;
    Response response;

    request.ParseRequest("GET /Datalogger?search=values&device=fragxpl-moduletwo.default:device1&start=20100101&end=20101231 HTTP/1.1");
    m_DataloggerController.Controller(request, &response);

    string str = "{\"values\":[{\"max\":\"18\",\"min\":\"5\",\"time\":\"20100101\",\"value\":\"15\"},{\"max\":\"17\",\"min\":\"6\",\"time\":\"20100102\",\"value\":\"16\"}]}\n";
    assert(str == response.GetBody());

    return true;
}

bool TestDataloggerController::WrongSearch()
{
    Request request;
    Response response;

    request.ParseRequest("GET /Datalogger?search=zzzz HTTP/1.1");
    m_DataloggerController.Controller(request, &response);
    assert(404==response.GetStatut());
    return true;
}

bool TestDataloggerController::WrongPOST()
{
    Request request;
    Response response;

    request.ParseRequest("POST /Datalogger?id=moduleone HTTP/1.1");
    m_DataloggerController.Controller(request, &response);
    assert(500==response.GetStatut());
    return true;
}
