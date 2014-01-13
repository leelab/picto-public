#ifndef _INTERACTIVESTDIOHANDLER_H_
#define _INTERACTIVESTDIOHANDLER_H_

#include <QThread>

/*! \brief THIS DOES NOT APPEAR TO BE USED IN PICTO AND SHOULD BE REMOVED.  ORIGINAL DOCUMENTATION APPEARS BELOW FOR REFERENCE.
 *
 *	\details Handles standard IO input when running in interactive mode 
 *	\author Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2013
 */
class InteractiveSTDIOHandler : public QThread
{
	Q_OBJECT

public:
	InteractiveSTDIOHandler();

    void run();
};

/*! @} */

#endif
