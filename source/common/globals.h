/*! \file globals.h
 * \ingroup picto
 * \brief Initialization calls and global variables exposed by the %Picto library
 */

#ifndef _GLOBALS_H_
#define _GLOBALS_H_

#include "common.h"
#include "namedefs.h"
#include "portdefs.h"

namespace Picto {

/*! \addtogroup picto
 * @{
 */

/*! \brief Registers application names*/
PICTOLIB_API inline void InitializeNames();

/*! \brief Registers types needed for signals and slots */
inline void InitializeMetaTypes();

/*! \brief Sets up the factories */
inline void initializeFactories();

/*! \brief Initializes the %Picto library
 *
 * InitializeLib must be called by code using the %Picto library prior to usage.  This
 * function sets up the library's state for proper execution.
 */
PICTOLIB_API void InitializeLib(QCoreApplication * coreApp, QString localeLanguageCode);

/*! \brief Initializes the ports used by this %Picto application according to the system number
 *
 * InitializePorts must be called by code using the %Picto library prior to usage and after calling
 * initialize lib.  This function sets up the port numbers used by Picto to communicate with other
 * %picto applications.  The input name should come from the Names object that is set up in
 * InializeLib.
 */
PICTOLIB_API void InitializePorts(QString appName,bool usedInSystemService = false);

/*! \brief Shuts down the %Picto library
 *
 * CloseLib must be called by code using the %Picto library prior to shutdown.  This
 * function performs any needed house-keeping prior to closure.
 */
PICTOLIB_API void CloseLib();

/*! This global variable contains all of the names utilized by components of the %Picto suite */
PICTOLIB_GLOBAL const TranslatedNames * Names;

/*! This global variable contains all of the port numbers utilized by components of the %Picto suite */
PICTOLIB_GLOBAL PortNums * portNums;

/* @} */
}; //namespace Picto

#endif
