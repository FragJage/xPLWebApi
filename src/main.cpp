#include <iostream>
#include <exception>
#include "xPLWebApi.h"

int main(int argc, char* argv[])
{
  int res = 0;

  try
  {
    xPLWebApi xPLDev;

	Service* pService = Service::Create("xPLWebApi", "WEB API for xPL protocol", &xPLDev);
  	res = pService->Start(argc, argv);
  	Service::Destroy();
  }
  catch(const exception &e)
  {
      std::cout << e.what();
  }
	return res;
}
