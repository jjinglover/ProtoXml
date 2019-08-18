
#include <stdio.h>
#include "xmlProto.h"

int main()
{
	xmlProto *ptr = new xmlProto;
	ptr->loadXml();
	ptr->printProtoXml();
	delete ptr;
	ptr = nullptr;
	getchar();
	return 0;
}

