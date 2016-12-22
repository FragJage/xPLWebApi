#include "UnitTest/UnitTest.h"
#include "TestModulesController.h"
#include "TestAdvanceConfigController.h"
#include "TestDataloggerController.h"
#include "TestDevicesController.h"
#include "TestMessagesController.h"
#include "TestCacheManager.h"
#include "TestxPLWebApi.h"

using namespace std;

int main()
{
  UnitTest unitTest;
  int ret = 0;


  try
  {
  	unitTest.addTestClass(new TestModulesController());
  	unitTest.addTestClass(new TestAdvanceConfigController());
  	unitTest.addTestClass(new TestDataloggerController());
  	unitTest.addTestClass(new TestDevicesController());
  	unitTest.addTestClass(new TestMessagesController());
  	unitTest.addTestClass(new TestCacheManager());
  	unitTest.addTestClass(new TestxPLWebApi());
  }
  catch(const exception &e)
  {
      unitTest.displayError(e.what());
      ret = -1;
  }

  if(ret!=-1)
    if(!unitTest.run()) ret = 1;

	return ret;
}
