#include <cassert>
#include "UnitTest/UnitTest.h"
#include "../src/MessagesController.h"
#include "xPLSendMock.h"

using namespace std;

class TestMessagesController : public TestClass<TestMessagesController>
{
public:
    TestMessagesController();
    ~TestMessagesController();

    bool WrongMethod();
    bool InvalideJson();
    bool MandatoryStructure();
    bool WrongMessageType();
    bool MandatoryClass();
    bool WrongClass();
    bool MandatoryTarget();
    bool SendxPLMessage();

private:
    MessagesController m_MessagesController;
    xPLsendMock m_xPLsendMock;
};
