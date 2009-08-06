/*! \file globals.h
 * \ingroup picto
 * \brief Initialization calls and global variables exposed by the %Picto library
 */

#ifndef _GLOBALS_H_
#define _GLOBALS_H_

#include "common.h"
#include "namedefs.h"

namespace Picto {

/*! \addtogroup picto
 * @{
 */

/*! \brief Registers types needed for signals and slots */
inline void InitializeMetaTypes();

/*! \brief Initializes the %Picto library
 *
 * InitializeLib must be called by code using the %Picto library prior to usage.  This
 * function sets up the library's state for proper execution.
 */
PICTOLIB_API void InitializeLib(QCoreApplication * coreApp, QString localeLanguageCode);

/*! \brief Shuts down the %Picto library
 *
 * CloseLib must be called by code using the %Picto library prior to shutdown.  This
 * function performs any needed house-keeping prior to closure.
 */
PICTOLIB_API void CloseLib();

/*! This global variable contains all of the names utilized by components of the %Picto suite */
PICTOLIB_GLOBAL const TranslatedNames * Names;

/* @} */
}; //namespace Picto

#endif
