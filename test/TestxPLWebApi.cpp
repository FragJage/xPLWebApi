#include "TestxPLWebApi.h"
#include "Plateforms/Plateforms.h"
#include "xPLLib/Schemas/SchemaObject.h"

using namespace std;

TestxPLWebApi::TestxPLWebApi() : TestClass("WebApi", this)
{
	addTest("Start", &TestxPLWebApi::Start);
	addTest("StdConfig", &TestxPLWebApi::StdConfig);
	addTest("Stop", &TestxPLWebApi::Stop);
	addTest("ReStart", &TestxPLWebApi::ReStart);
	addTest("ReStop", &TestxPLWebApi::ReStop);

    if(remove("config")==0)
        cout << termcolor::yellow << "Remove old config file" << endl << termcolor::grey;
}

TestxPLWebApi::~TestxPLWebApi()
{
    if(remove("config")!=0)
        cout << termcolor::red << "Unable to remove config file" << endl << termcolor::grey;
}

void TestxPLWebApi::ThreadStart(xPLWebApi* pxPLDev)
{
    char exeName[] = "test";
    char confName[] = "config";
    char* argv[2];

    argv[0]= exeName;
    argv[1]= confName;
    pxPLDev->ServiceStart(2, argv);
}

bool TestxPLWebApi::Start()
{
    string msg;
    xPL::SchemaObject sch;


    thread integrationTest(ThreadStart, &xPLDev);
    integrationTest.detach();

    msg = SimpleSockUDP::GetLastSend(10);
    sch.Parse(msg);
    assert("xPL WebApi"==sch.GetValue("appname"));

    msg = SimpleSockUDP::GetLastSend(10);
    sch.Parse(msg);
    assert("hbeat"==sch.GetClass());
    assert("request"==sch.GetType());

    return true;
}

bool TestxPLWebApi::StdConfig()
{
    string msg;
    xPL::SchemaObject sch;
    xPL::SchemaObject schCfg(xPL::SchemaObject::cmnd, "config", "response");
    xPL::SchemaObject schHb(xPL::SchemaObject::trig, "hbeat", "app");
    SimpleSockUDP socket;

    schCfg.SetValue("interval", "25");
    schCfg.SetValue("newconf", "test");
    schCfg.SetValue("webdoc", "../web");
    schCfg.SetValue("webport", "88");

    msg = schCfg.ToMessage("fragxpl-test.default", "fragxpl-webapi.default");
    SimpleSockUDP::SetNextRecv(msg);

    msg = SimpleSockUDP::GetLastSend(10);     //Pass Hbeat.end message
    msg = SimpleSockUDP::GetLastSend(10);
    sch.Parse(msg);
    assert("25"==sch.GetValue("interval"));
    assert("fragxpl-webapi.test"==sch.GetSource());

    return true;
}

bool TestxPLWebApi::Stop()
{
    string msg;
    xPL::SchemaObject sch;

    xPLDev.ServicePause(true);
    Plateforms::delay(500);
    xPLDev.ServicePause(false);
    xPLDev.ServiceStop();

    msg = SimpleSockUDP::GetLastSend(10);
    sch.Parse(msg);
    assert("hbeat"==sch.GetClass());
    assert("end"==sch.GetType());
    Plateforms::delay(300);

    return true;
}

bool TestxPLWebApi::ReStart()
{
    string msg;

    thread integrationTest(ThreadStart, &xPLDev);
    integrationTest.detach();

    msg = SimpleSockUDP::GetLastSend(10);   //Pass hbeat.request

    return true;
}

bool TestxPLWebApi::ReStop()
{
    string msg;
    xPL::SchemaObject sch;


    xPLDev.ServiceStop();

    msg = SimpleSockUDP::GetLastSend(10);
    sch.Parse(msg);
    assert("hbeat"==sch.GetClass());
    assert("end"==sch.GetType());
    Plateforms::delay(300);

    return true;
}
