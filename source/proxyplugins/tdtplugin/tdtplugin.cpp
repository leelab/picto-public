/*! \file plexonplugin.cpp
 * \ingroup proxypluginplexon
 * \brief A plugin allowing the proxy server to interface with a Plexon MAP
 */

#include "tdtplugin.h"

QString TdtPlugin::device() const
{
	return "TDT";
}

NeuralDataAcqInterface::deviceStatus TdtPlugin::startDevice()
{
	return NeuralDataAcqInterface::stopped;
}

NeuralDataAcqInterface::deviceStatus TdtPlugin::stopDevice()
{
	return NeuralDataAcqInterface::stopped;

}
NeuralDataAcqInterface::deviceStatus TdtPlugin::getDeviceStatus()
{
	return NeuralDataAcqInterface::stopped;
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