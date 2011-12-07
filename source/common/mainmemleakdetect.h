//Any main.cpp for an application that wants memory leak detection enabled should 
//include this file
//Note that this file must be included after all predefined headers (ie. 
//#include <QSharedPointer>) but before common.h, all non-predefined headers 
//(ie. picto headers) must be included after all predefined headers in general
//or else this will effectively be before some predefined headers
// See http://www.qtcentre.org/wiki/index.php?title=Memory_Leak_Detection_in_VS
#if defined (DETECTMEMLEAKS) && defined(WIN32) && defined(_DEBUG)
	#define _CRTDBG_MAP_ALLOC
	#include <stdlib.h>
	#include <crtdbg.h>
	#define DEBUG_NEW new( _NORMAL_BLOCK, __FILE__, __LINE__ )
	#define new DEBUG_NEW
#endif