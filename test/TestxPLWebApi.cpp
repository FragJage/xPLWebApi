#include "TestxPLWebApi.h"
#include "Plateforms/Plateforms.h"
#include "xPLLib/Schemas/SchemaObject.h"

using namespace std;

TestxPLWebApi::TestxPLWebApi() : TestClass("WebApi", this)
{
	addTest("Start", &TestxPLWebApi::Start);
	addTest("StdConfig", &TestxPLWebApi::StdConfig);
	addTest("AnswerCoverage", &TestxPLWebApi::AnswerCoverage);
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

bool TestxPLWebApi::AnswerCoverage()
{
    string msg;
    xPL::SchemaObject schCnf(xPL::SchemaObject::stat, "config", "basic");
    msg = schCnf.ToMessage("fragxpl-modulone.default", "fragxpl-test.default");
    SimpleSockUDP::SetNextRecv(msg);

    msg = SimpleSockUDP::GetLastSend(10);     //Pass SchemaConfigListRequest
    msg = SimpleSockUDP::GetLastSend(10);     //Pass SchemaConfigCurrentRequest
    msg = SimpleSockUDP::GetLastSend(10);     //Pass SchemaAdvanceConfigListRequest

    schCnf.SetType("list");
    msg = schCnf.ToMessage("fragxpl-modulone.default", "fragxpl-test.default");
    SimpleSockUDP::SetNextRecv(msg);

    schCnf.SetType("current");
    msg = schCnf.ToMessage("fragxpl-modulone.default", "fragxpl-test.default");
    SimpleSockUDP::SetNextRecv(msg);

    schCnf.SetType("end");
    msg = schCnf.ToMessage("fragxpl-modulone.default", "fragxpl-test.default");
    SimpleSockUDP::SetNextRecv(msg);

    xPL::SchemaObject schAC(xPL::SchemaObject::stat, "advanceconfig", "list");
    msg = schAC.ToMessage("fragxpl-modulone.default", "fragxpl-test.default");
    SimpleSockUDP::SetNextRecv(msg);

    schAC.SetType("current");
    msg = schAC.ToMessage("fragxpl-modulone.default", "fragxpl-test.default");
    SimpleSockUDP::SetNextRecv(msg);

    xPL::SchemaObject schHb(xPL::SchemaObject::stat, "hbeat", "basic");
    msg = schHb.ToMessage("fragxpl-modulone.default", "fragxpl-test.default");
    SimpleSockUDP::SetNextRecv(msg);

    msg = SimpleSockUDP::GetLastSend(10);     //Pass SchemaConfigListRequest
    msg = SimpleSockUDP::GetLastSend(10);     //Pass SchemaConfigCurrentRequest
    msg = SimpleSockUDP::GetLastSend(10);     //Pass SchemaSensorRequest
    msg = SimpleSockUDP::GetLastSend(10);     //Pass SchemaAdvanceConfigListRequest
    msg = SimpleSockUDP::GetLastSend(10);     //Pass SchemaAdvanceConfigCurrentCmnd

    schHb.SetType("end");
    msg = schHb.ToMessage("fragxpl-modulone.default", "fragxpl-test.default");
    SimpleSockUDP::SetNextRecv(msg);

    xPL::SchemaObject schDL(xPL::SchemaObject::stat, "datalogger", "basic");
    schDL.SetValue("response", "value");
    schDL.SetValue("device", "temp01");
    msg = schDL.ToMessage("fragxpl-modulone.default", "fragxpl-test.default");
    SimpleSockUDP::SetNextRecv(msg);

    schDL.SetValue("response", "device");
    schDL.SetValue("starttime", "20160101");
    schDL.SetValue("endtime", "20161231");
    msg = schDL.ToMessage("fragxpl-modulone.default", "fragxpl-test.default");
    SimpleSockUDP::SetNextRecv(msg);

    xPL::SchemaObject schSensor(xPL::SchemaObject::stat, "sensor", "basic");
    schDL.SetValue("current", "22");
    schDL.SetValue("device", "temp01");
    msg = schDL.ToMessage("fragxpl-modulone.default", "fragxpl-test.default");
    SimpleSockUDP::SetNextRecv(msg);

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
