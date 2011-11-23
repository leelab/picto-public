/*! \file common.h
 * \ingroup picto
 * \brief Definitions common to the %Picto library
 */

/*! \addtogroup picto
 * @{
 */

#ifndef _COMMON_H_
#define _COMMON_H_

/*! \def PICTOLIB_API
 *       Flags a function as an API of the %Picto library
 */

/*! \def PICTOLIB_CLASS
 *       Flags a decorated class as an accessible part of the %Picto library
 */

/*! \def PICTOLIB_GLOBAL
 *       Flags a global value as an accessible part of the %Picto library
 */

/*! \def PICTOLIB_EXPORTS
 *       A define set by the %Picto library project file allowing the library
 *       to export its functionality, and allowing consumers to utilize its
 *       functionality
 */

//We make a special case for the documentation generator as these defines are
//conditional and will have varying values on different platforms.  We don't want
//any platform's values to show up in the documentation.  Also, some defines are
//set at the project level, and won't be seen by the documentation processor,
//so we explicitly define them here if it is a documentation pass.
#ifdef DOCUMENTATION_PASS
	#define PICTOLIB_EXPORTS
	#define PICTOLIB_CLASS
	#define PICTOLIB_API
	#define PICTOLIB_GLOBAL
#elif defined WIN32 || defined WINCE
	#ifdef PICTOLIB_EXPORTS
		#define PICTOLIB_API __declspec(dllexport)
		#define PICTOLIB_GLOBAL extern __declspec(dllexport)
		#define PICTOLIB_CLASS __declspec(dllexport)
	#else
		#define PICTOLIB_API __declspec(dllimport)
		#define PICTOLIB_GLOBAL extern __declspec(dllimport)
		#define PICTOLIB_CLASS __declspec(dllimport)
	#endif
#else
	#define PICTOLIB_API extern
	#define PICTOLIB_GLOBAL extern
	#define PICTOLIB_CLASS
#endif

/*! References an unused parameter \a P so as to remove compiler warnings.
 *  This is added in addition to Qt's Q_UNUSED because Q_UNUSED is
 *  inaccessible from ANSI C code (it requires C++)
 */
#define IGNORED_PARAMETER(P) (P)

/*! The various ports used by components of the system are changed for release and development versions of the code.
 *	This allows us to develop and debug picto on a network running picto experiments.  Otherwise, we could not have
 *	two separate PictoServers open at the same time.
 */
#define DEVELOPMENTBUILD
#ifdef DEVELOPMENTBUILD
#define PORTPREFIX 70000
#else
#define PORTPREFIX 40000
#endif
#define DISCOVERSERVERPORT PORTPREFIX+2425
#define SERVERPORT PORTPREFIX+2424
#define LCDCOMMANDPORT PORTPREFIX+2422
#define LCDEVENTPORT PORTPREFIX+2421

#endif

/*! @} */
