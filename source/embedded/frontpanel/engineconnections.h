#ifndef ENGINECONNECTIONS_H
#define ENGINECONNECTIONS_H

#include <QTcpServer>
#include "FrontPanelInfo.h"

class EngineConnections : public QObject
{
	Q_OBJECT
public:
	EngineConnections(FrontPanelInfo *panelInfo, QObject* parent = 0);
	~EngineConnections();

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
