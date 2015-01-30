/*! \file memleakdetect.h
 *	\brief Any cpp file that includes this file will have memory leak detection enabled.
 *	\note This file must be included after all predefined headers (ie. \#include \<QSharedPointer\>).
 *	Therefore, including this in header files makes things incredibly complicated since
 *  some files will include others that may define this and then include other files with
 *  precompiled headers at the top.  For this reason, you should only include this in cpp
 *  files and memory leaks that occur in .h files won't be detected.  SO DON'T ALLOCATE MEMORY
 *	IN .h FILES UNLESS YOU'RE REALLY CAREFUL!
 *	
 *	\details See http://www.qtcentre.org/wiki/index.php?title=Memory_Leak_Detection_in_VS
 *	for more information.
 *	\sa mainmemleakdetect.h
 *	\author Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2013
 */
#if !defined(_CRTDBG_MAP_ALLOC)
	#define _CRTDBG_MAP_ALLOC
	#include <stdlib.h>
	#include <crtdbg.h>
#endif
#if defined (DETECTMEMLEAKS) && defined(WIN32) && defined(_DEBUG)
	#define DEBUG_NEW new( _NORMAL_BLOCK, __FILE__, __LINE__ )
	#define new DEBUG_NEW
#endif

