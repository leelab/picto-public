#ifndef ENGINECONNECTIONS_H
#define ENGINECONNECTIONS_H

#include <QTcpServer>
#include "FrontPanelInfo.h"


/*!	\brief Sets up the connections between the engine and the front panel.
 *
 *	We're using a client/server arrangement to communicate between the engine and
 *	the front panel.  This isn't really easy to set up in Picto, and should be
 *	plenty fast, since we won't actually be using the network.
 */
class EngineConnections : public QObject
{
	Q_OBJECT
public:
	EngineConnections(FrontPanelInfo *panelInfo, QObject* parent = 0);
	virtual ~EngineConnections();

private slots:
	void setupCommandConnection();
	void setupEventConnection();
signals:
	void newEventRead();

private:
	FrontPanelInfo *panelInfo;
	QTcpServer *commandChannel;
	QTcpServer *eventChannel;


};


#endif
