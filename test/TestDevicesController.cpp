#include "jsoncpp/include/json/json.h"
#include "TestDevicesController.h"
#include "WebServer/Response.h"
#include "WebServer/Request.h"

using namespace std;

TestDevicesController::TestDevicesController() : TestClass("TestDevicesController", this)
{
    m_xPLsendMock.SetCacheManager(&m_CacheManagerMock);
    m_DevicesController.SetCacheManager(&m_CacheManagerMock);
    m_DevicesController.SetxPLSender(&m_xPLsendMock);

	addTest("GetList", &TestDevicesController::GetList);
	addTest("GetDevice", &TestDevicesController::GetDevice);
	addTest("DeviceNotFound", &TestDevicesController::DeviceNotFound);
	addTest("WrongPOST", &TestDevicesController::WrongPOST);
}

TestDevicesController::~TestDevicesController()
{
}

bool TestDevicesController::GetList()
{
    Request request;
    Response response;
    Json::Reader jsReader;
    Json::Value jsBody;
    Json::Value jsDevices;


    request.ParseRequest("GET /Devices HTTP/1.1");
    m_DevicesController.Controller(request, &response);

    jsReader.parse(response.GetBody(), jsBody);
    jsDevices = jsBody.get("devices","");

    assert(true==jsDevices.isArray());
    assert(3==jsDevices.size());
    assert("fragxpl-moduletwo.default:device1"==jsDevices[0].get("name","").asString());
    assert("fragxpl-moduletwo.default:device2"==jsDevices[1].get("name","").asString());
    assert("fragxpl-moduletwo.default:device3"==jsDevices[2].get("name","").asString());

    return true;
}

bool TestDevicesController::GetDevice()
{
    Request request;
    Response response;

    request.ParseRequest("GET /Devices?id=fragxpl-moduletwo.default:device1 HTTP/1.1");
    m_DevicesController.Controller(request, &response);

    string str = "{\"current\":\"28\",\"device\":\"device1\",\"max\":\"30\",\"min\":\"26\",\"schema\":\"sensor.basic\",\"source\":\"fragxpl-moduletwo.default\",\"type\":\"temp\",\"unit\":\"°\"}\n";
    assert(str==response.GetBody());
    return true;
}

bool TestDevicesController::DeviceNotFound()
{
    Request request;
    Response response;

    request.ParseRequest("GET /Devices?id=modulezzz HTTP/1.1");
    m_DevicesController.Controller(request, &response);
    assert(404==response.GetStatut());
    return true;
}

bool TestDevicesController::WrongPOST()
{
    Request request;
    Response response;

    request.ParseRequest("POST /Devices?id=moduleone HTTP/1.1");
    m_DevicesController.Controller(request, &response);
    assert(500==response.GetStatut());
    return true;
}
