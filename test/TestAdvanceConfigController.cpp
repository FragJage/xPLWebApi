#include "jsoncpp/include/json/json.h"
#include "TestAdvanceConfigController.h"
#include "WebServer/Response.h"
#include "WebServer/Request.h"

using namespace std;

TestAdvanceConfigController::TestAdvanceConfigController() : TestClass("AdvanceConfigController", this)
{
    m_xPLsendMock.SetCacheManager(&m_CacheManagerMock);
    m_AdvanceConfigController.SetCacheManager(&m_CacheManagerMock);
    m_AdvanceConfigController.SetxPLSender(&m_xPLsendMock);


	addTest("GetList", &TestAdvanceConfigController::GetList);
	addTest("GetAdvanceConfig", &TestAdvanceConfigController::GetAdvanceConfig);
	addTest("SetAdvanceConfig", &TestAdvanceConfigController::SetAdvanceConfig);
    addTest("ConfigNotFound", &TestAdvanceConfigController::ConfigNotFound);
	addTest("WrongPOST", &TestAdvanceConfigController::WrongPOST);
}

TestAdvanceConfigController::~TestAdvanceConfigController()
{
}

bool TestAdvanceConfigController::GetList()
{
    Request request;
    Response response;
    Json::Reader jsReader;
    Json::Value jsBody;


    request.ParseRequest("GET /AdvanceConfig HTTP/1.1");
    m_AdvanceConfigController.Controller(request, &response);

    jsReader.parse(response.GetBody(), jsBody);
    assert(true==jsBody.isArray());
    assert(2==jsBody.size());
    assert("fragxpl-moduleone.default"==jsBody[0].asString());
    assert("fragxpl-moduletwo.default"==jsBody[1].asString());

    return true;
}

bool TestAdvanceConfigController::GetAdvanceConfig()
{
    Request request;
    Response response;

    request.ParseRequest("GET /AdvanceConfig?id=fragxpl-moduletwo.default&action=refresh HTTP/1.1");
    m_AdvanceConfigController.Controller(request, &response);

    string str = "{\"config\":[\
{\"configname\":\"device1\",\"type\":\"temp\",\"value\":\"28\"},\
{\"configname\":\"device2\",\"type\":\"output\",\"value\":\"high\"}],\
\"format\":\
{\"configname\":{\"type\":\"string\"},\
\"type\":{\"list\":\"controltype\",\"type\":\"string\"},\
\"value\":{\"type\":\"string\"}}}\n";

    assert(str==response.GetBody());
    return true;
}

bool TestAdvanceConfigController::SetAdvanceConfig()
{
    Request request;
    Response response;

    string str = "PUT /AdvanceConfig?id=fragxpl-moduletwo.default HTTP/1.1\n\n{[\
{\"configname\":\"device1\",\"type\":\"temp\",\"value\":\"28\"},\
{\"configname\":\"device2\",\"type\":\"output\",\"value\":\"high\"}]}";

    request.ParseRequest(str);
    m_AdvanceConfigController.Controller(request, &response);
    assert(200==response.GetStatut());
    return true;
}

bool TestAdvanceConfigController::ConfigNotFound()
{
    Request request;
    Response response;

    request.ParseRequest("GET /AdvanceConfig?id=fragxpl-modulezzz.default HTTP/1.1");
    m_AdvanceConfigController.Controller(request, &response);
    assert(404==response.GetStatut());
    return true;
}

bool TestAdvanceConfigController::WrongPOST()
{
    Request request;
    Response response;

    request.ParseRequest("POST /AdvanceConfig?id=moduleone HTTP/1.1");
    m_AdvanceConfigController.Controller(request, &response);
    assert(500==response.GetStatut());
    return true;
}
