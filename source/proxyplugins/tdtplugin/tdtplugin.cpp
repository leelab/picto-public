/*! \file plexonplugin.cpp
 * \ingroup proxypluginplexon
 * \brief A plugin allowing the proxy server to interface with a Plexon MAP
 */

#include "tdtplugin.h"

QString TdtPlugin::device() const
{
	return "tdt";
}

QString TdtPlugin::deviceName()
{
	return "TDT";
}
QString TdtPlugin::deviceStatus()
{
	return "TDT:device status";
}


float TdtPlugin::samplingRate()
{
	return 0;
}

QString TdtPlugin::dumpData()
{
	QString xmlData = "TDT: dump data";
	return xmlData;
}

Q_EXPORT_PLUGIN2(ProxyPluginTDT, TdtPlugin)