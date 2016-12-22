#ifndef IxPLSend_H
#define IxPLSend_H

#include "xPLLib/Schemas/SchemaObject.h"

class IxPLSend
{
	public:
		virtual void SendxPLMessage(xPL::ISchema *schema, const std::string& target) = 0;
};

#endif // IxPLSend_H
