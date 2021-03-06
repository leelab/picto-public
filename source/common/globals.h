/*! \file globals.h
 * \brief Initialization calls and global variables exposed by the %Picto library
 *	\author Trevor Stavropoulos, Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2015
 */

#ifndef _GLOBALS_H_
#define _GLOBALS_H_

#include "common.h"
#include "namedefs.h"
#include "portdefs.h"

namespace Picto {

PICTOLIB_API inline void InitializeNames();

inline void InitializeMetaTypes();

PICTOLIB_API void InitializeLib(QCoreApplication * coreApp, QString localeLanguageCode);

PICTOLIB_API void InitializePorts(QString appName,bool usedInSystemService = false);

PICTOLIB_API void CloseLib();

/*! \brief This global variable contains all of the names utilized by components of the %Picto suite */
PICTOLIB_GLOBAL const TranslatedNames * Names;

/*! \brief This global variable contains all of the port numbers utilized by components of the %Picto suite */
PICTOLIB_GLOBAL PortNums * portNums;

}; //namespace Picto

#endif
