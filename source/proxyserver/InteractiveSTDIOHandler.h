/*! \file InteractiveSTDIOHandler.h
 * \ingroup pictoserver
 * \brief
 */

#ifndef _INTERACTIVESTDIOHANDLER_H_
#define _INTERACTIVESTDIOHANDLER_H_

#include <QThread>

/*! \addtogroup pictoserver
 * @{
 */

/*! \brief Handles standard IO input when running in interactive mode */
class InteractiveSTDIOHandler : public QThread
{
	Q_OBJECT

public:
	InteractiveSTDIOHandler();

    void run();
};

/*! @} */

#endif
