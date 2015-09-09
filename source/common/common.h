/*! \file common.h
 * \brief Definitions common to the %Picto library
 *	\author Trevor Stavropoulos, Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2015
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
//Joey - Haven't used this... not sure that the assumptions made for this flag are still valid.
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
 *	\note The method mentioned here was used before we added "System numbers" to Picto.  Now the System Number takes care
 *	of making sure that Picto in development doesn't affect live experiments.
 */
//#define DEVELOPMENTBUILD
//#define SYSTEM_0	//Currently we are using different servers for each picto system.  System 0 is Chris's/Bart's/Hansem's system.  System 1 is Hyojung's. System 2 is Min's.
////#define SYSTEM_1
////#define SYSTEM_2
//#ifdef DEVELOPMENTBUILD
//	#define PORTPREFIX 50000
//#else
//	#ifdef SYSTEM_0
//		#define PORTPREFIX 40000
//	#else
//		#ifdef SYSTEM_1
//			#define PORTPREFIX 60000
//		#else
//			#ifdef SYSTEM_2
//				#define PORTPREFIX 50000
//			#else
//				#error Either DEVELOPMENTBUILD, SYSTEM_0, SYSTEM_1, or SYSTEM_2 must be defined!
//			#endif
//		#endif
//	#endif
//#endif
//#define APPUPDATEPORT PORTPREFIX+2423
//#define MINDISCOVERSERVERPORT PORTPREFIX+2425
//#define MAXDISCOVERSERVERPORT PORTPREFIX+2500
//#define SERVERPORT PORTPREFIX+2424
//#define LCDCOMMANDPORT PORTPREFIX+2422
//#define LCDEVENTPORT PORTPREFIX+2421

/*! \brief This is the current Version of the Picto Software according to MAJOR.MINOR.BUILD.
 *	\details We are currently on Major number 2.  We moved to this number when we reached 
 *	a "feature complete" version of Picto containing a full Experimental and Analysis framework.
 *	Obviously there is a whole ton more that needs to be done, but in this version it is possible
 *	for researchers to build and run their own experiments fully within the Picto system without
 *	help from a Picto software developer.
 */
#define PICTOVERSION "2.2.3"
/*! \brief This is the current version of the Picto design syntax.  It is saved along with every Picto design
 *	so that it will be clear on deserialization whether some syntax needs to be upgraded and how.
 */
#define DESIGNSYNTAXVERSION "0.0.2"


#ifdef _DEBUG 
	//Uncomment this to enable memory leak detection
	//#define DETECTMEMLEAKS
#endif


#endif
