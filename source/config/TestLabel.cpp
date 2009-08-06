#include "TestLabel.h"

#include "../common/network/ServerDiscoverer.h"

TestLabel::TestLabel(QWidget * parent) :
	QLabel("", parent)
{
	setText(tr("Server Discover Broadcast Sent"));

	Picto::ServerDiscoverer serverDiscoverer;

	serverDiscoverer.discover();

	if(serverDiscoverer.waitForDiscovered())
	{
		setText(QString("Found Server at: %1:%2").arg(serverDiscoverer.getAddress().toString())
												 .arg(serverDiscoverer.getPort()));
		adjustSize();
	}
}

TestLabel::~TestLabel()
{
}
