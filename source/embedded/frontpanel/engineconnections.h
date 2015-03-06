#ifndef ENGINECONNECTIONS_H
#define ENGINECONNECTIONS_H

#include <QTcpServer>
#include "FrontPanelInfo.h"


/*!	\brief Sets up the connections between the engine and the front panel.
 * \details This used to be a more important class than it is now.  The only
 * thing that it does at this point is set up the socket used to send commands
 * from the Front Panel application to the Director application, and set that
 * socket into the FrontPanelInfo object.
 * \author Trevor Stavropoulos, Joey Schnurr, Mark Hammond, Matt Gay
 * \date 2009-2015
 */
class EngineConnections : public QObject
{
	Q_OBJECT
public:
	EngineConnections(FrontPanelInfo *panelInfo, QObject* parent = 0);
	virtual ~EngineConnections();

private slots:
	void setupCommandConnection();
signals:
	/*! \brief No Longer Used*/
	void newEventRead();

private:
	FrontPanelInfo *panelInfo;
	QTcpServer *commandChannel;
};


#endif
