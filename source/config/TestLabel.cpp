#include "TestLabel.h"

#include "../common/network/ServerDiscoverer.h"
#include "../common/memleakdetect.h"

/*! \brief Constructs a label with very simple server information.
*/
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
