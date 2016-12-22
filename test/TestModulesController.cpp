#include "jsoncpp/include/json/json.h"
#include "TestModulesController.h"
#include "WebServer/Response.h"
#include "WebServer/Request.h"

using namespace std;

TestModulesController::TestModulesController() : TestClass("TestModulesController", this)
{
    m_xPLsendMock.SetCacheManager(&m_CacheManagerMock);
    m_ModulesController.SetCacheManager(&m_CacheManagerMock);
    m_ModulesController.SetxPLSender(&m_xPLsendMock);

	addTest("GetList", &TestModulesController::GetList);
	addTest("GetConfig", &TestModulesController::GetConfig);
	addTest("SetConfig", &TestModulesController::SetConfig);
	addTest("WrongPOST", &TestModulesController::WrongPOST);
}

TestModulesController::~TestModulesController()
{
}

bool TestModulesController::GetList()
{
    Request request;
    Response response;
    Json::Reader jsReader;
    Json::Value jsBody;
    Json::Value jsModules;


    request.ParseRequest("GET /Modules HTTP/1.1");
    m_ModulesController.Controller(request, &response);

    jsReader.parse(response.GetBody(), jsBody);
    jsModules = jsBody.get("modules","");

    assert(true==jsModules.isArray());
    assert(2==jsModules.size());
    assert("fragxpl-moduleone.default"==jsModules[0].get("name","").asString());
    assert("fragxpl-moduletwo.default"==jsModules[1].get("name","").asString());

    return true;
}

bool TestModulesController::GetConfig()
{
    Request request;
    Response response;

    request.ParseRequest("GET /Modules?id=fragxpl-moduleone.default&action=refresh HTTP/1.1");
    m_ModulesController.Controller(request, &response);

    string str = "{\"alive\":\"A\",\"config\":{\
\"filter\":{\"max\":16,\"type\":\"option\",\"value\":[\"filterone\",\"filtertwo\"]},\
\"group\":{\"max\":16,\"type\":\"option\",\"value\":[\"groupone\",\"grouptwo\"]},\
\"interval\":{\"max\":1,\"type\":\"option\",\"value\":\"25\"},\
\"newconf\":{\"max\":1,\"type\":\"reconf\",\"value\":\"default\"},\
\"port\":{\"max\":1,\"type\":\"reconf\",\"value\":\"80\"}},\
\"interval\":\"25\",\"name\":\"fragxpl-moduleone.default\",\
\"port\":\"3901\",\"remote-ip\":\"127.0.0.1\"}\n";

    assert(str==response.GetBody());
    return true;
}

bool TestModulesController::SetConfig()
{
    Request request;
    Response response;

    request.ParseRequest("PUT /Modules?id=fragxpl-modulethree.default HTTP/1.1\n\n{\"interval\":{\"value\":\"25\"},\"port\":{\"value\":\"80\"},\"newconf\":{\"value\":\"default\"}}");
    m_ModulesController.Controller(request, &response);
    assert(200==response.GetStatut());
    assert("fragxpl-modulethree.default"==response.GetBody());
    return true;
}

bool TestModulesController::WrongPOST()
{
    Request request;
    Response response;

    request.ParseRequest("POST /Modules?id=moduleone HTTP/1.1");
    m_ModulesController.Controller(request, &response);
    assert(500==response.GetStatut());
    return true;
}
