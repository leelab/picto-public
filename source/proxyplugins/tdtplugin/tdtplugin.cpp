/*! \file plexonplugin.cpp
 * \ingroup proxypluginplexon
 * \brief A plugin allowing the proxy server to interface with a Plexon MAP
 */

#include <windows.h>
#include <tchar.h>

#include <QString>
#include <QLineEdit>
#include <QMessageBox>
#include <QXmlStreamWriter>

#include "tdtplugin.h"
#include "dialog.h"


QString TdtPlugin::device() const
{
	return "TDT";
}

bool TdtPlugin::startCOM()
{
	COMMutex.lock();
	//set up the connection
	//Initialize ActiveX object
	HRESULT hr;
	hr = CoInitialize(NULL);
	if (FAILED(hr)) {
		return false;
	}
	hr = tdtTank.CreateInstance("TTank.X"); //appId = "{670490CE-57D2-4176-8E74-80C4C6A47D88}"; //"TTankX.ocx"

	if (FAILED(hr)) 
	{
		bDeviceRunning = false;
		COMMutex.unlock();
		return false;
	}

	try
	{
		if(!(tdtTank->ConnectServer(szServerName,"PictoProxyServer")))
		{
			bDeviceRunning = false;
			COMMutex.unlock();
			return false;
		}
	}
	catch(_com_error e)
	{
		bDeviceRunning = false;
		COMMutex.unlock();
		return false;
	}

	if(!(tdtTank->OpenTank(szTankName,"R")))
	{
		tdtTank->ReleaseServer();
		bDeviceRunning = false;
		COMMutex.unlock();
		return false;
	}

	if(!(tdtTank->SelectBlock(szBlockName)))
	{
		tdtTank->CloseTank();
		tdtTank->ReleaseServer();
		bDeviceRunning = false;
		COMMutex.unlock();
		return false;
	}

	return true;
}

void TdtPlugin::stopCOM()
{
	tdtTank->CloseTank();
	tdtTank->ReleaseServer();
	CoUninitialize();

	COMMutex.unlock();
}


NeuralDataAcqInterface::deviceStatus TdtPlugin::startDevice()
{
	//get the server/tank/block name
	Dialog *getServerInfo = new Dialog();
	getServerInfo->exec();

	QString serverName = getServerInfo->serverLine->text();
	QString tankName = getServerInfo->tankLine->text();
	QString blockName = getServerInfo->blockLine->text();

	//convert everything from QString to wchar_t*
	//This is ugly....
	szServerName = new wchar_t[serverName.size()+1];
	serverName.toWCharArray(szServerName);
	szServerName[serverName.size()] = '\0';

	szTankName = new wchar_t[tankName.size()+1];
	tankName.toWCharArray(szTankName);
	szTankName[tankName.size()] = '\0';

	szBlockName = new wchar_t[blockName.size()+1];
	blockName.toWCharArray(szBlockName);
	szBlockName[blockName.size()] = '\0';

	
	//Test the tank Interface to make sure that the passed in values are working.
	QMessageBox errorMsgBox;

	HRESULT hr;
	hr = CoInitialize(NULL);
	if (FAILED(hr)) {
		errorMsgBox.setText(tr("COM failed to initialize"));
		errorMsgBox.exec();
		return NeuralDataAcqInterface::failedToStart;
	}
	hr = tdtTank.CreateInstance("TTank.X"); //appId = "{670490CE-57D2-4176-8E74-80C4C6A47D88}"; //"TTankX.ocx"

	if (FAILED(hr)) {
		errorMsgBox.setText(tr("Couldn't start the TDT interface. Are the TDT ActiveX controls installed?"));
		errorMsgBox.exec();
		return NeuralDataAcqInterface::failedToStart;
	}

	try
	{
		if(!(tdtTank->ConnectServer(szServerName,"PictoProxyServer")))
		{
			errorMsgBox.setText(tr("Couldn't connect to TDT server: %1").arg(serverName));
			errorMsgBox.exec();
			return NeuralDataAcqInterface::failedToStart;
		}
	}
	catch(_com_error e)
	{
		// Crack _com_error
		_bstr_t bstrSource(e.Source());
		QString qstSource((char*) bstrSource);

		_bstr_t bstrDescription(e.Description());
		QString qstDescription((char*) bstrDescription);

		QString qstErrorMessage(e.Error());

		errorMsgBox.setText(tr("Exception thrown for classes generated by #import"));
		errorMsgBox.setInformativeText(tr("Code = %1\nCodeMeaning = %2\n"
									"Source = %3\nDescription =%4\n").
									arg(e.Error()).
									arg(qstErrorMessage).
									arg(qstSource).
									arg(qstDescription));
		errorMsgBox.exec();
		return NeuralDataAcqInterface::failedToStart;
	}

	if(!(tdtTank->OpenTank(szTankName,"R")))
	{
		errorMsgBox.setText(tr("Couldn't open the tank: %1").arg(tankName));
		errorMsgBox.exec();
		tdtTank->ReleaseServer();
		return NeuralDataAcqInterface::failedToStart;
	}

	if(!(tdtTank->SelectBlock(szBlockName)))
	{
		errorMsgBox.setText(tr("Couldn't select block: %1").arg(blockName));
		errorMsgBox.exec();
		tdtTank->CloseTank();
		tdtTank->ReleaseServer();
		return NeuralDataAcqInterface::failedToStart;
	}
	

	sampleRate=0;
	lastTimestamp=0;

	//Close everything back up...
	tdtTank->CloseTank();
	tdtTank->ReleaseServer();
	CoUninitialize();

	bDeviceRunning = true;
	sampleRate = 0.0f;

	return NeuralDataAcqInterface::started;
}

NeuralDataAcqInterface::deviceStatus TdtPlugin::stopDevice()
{
	bDeviceRunning = false;
	sampleRate = 0.0f;

	return NeuralDataAcqInterface::stopped;
}
NeuralDataAcqInterface::deviceStatus TdtPlugin::getDeviceStatus()
{
	if(	bDeviceRunning )
	{
		return NeuralDataAcqInterface::running;
	}
	else
	{
		return NeuralDataAcqInterface::stopped;
	}
}

float TdtPlugin::samplingRate()
{
	//The sample rate is zeroed any time the device is stopped or started.
	//The rate is also updated every time dump data is called.
	//Since the sampling rate shouldn't be changing during a run, then 
	//if it isn't zero, we can jsut use the value from the last call to dump data.
	if(sampleRate != 0.0f)
		return sampleRate;

	if(!startCOM())
		return 0;
	
	long tankStatus = tdtTank->CheckTank(szTankName);
	if(tankStatus != 79 && tankStatus != 82)
	{
		stopCOM();
		return 0;
	}

	int numSpikeSamples;

	//Read spike samples
	numSpikeSamples = tdtTank->ReadEventsV(1000000,"Snip",0,0,0.0,0.0,"All");
	
	_variant_t spikeSampleFrequencyArray;

	spikeSampleFrequencyArray = tdtTank->ParseEvInfoV(0,numSpikeSamples,9);
	
	sampleRate = (float)((double *) spikeSampleFrequencyArray.parray->pvData)[0];

	stopCOM();
	return sampleRate;
}

QString TdtPlugin::dumpData()
{
	if(!startCOM())
	{
		return QString::null;
	}

	QString xmlData;
	QXmlStreamWriter writer(&xmlData);

	writer.setAutoFormatting(true);

	long tankStatus = tdtTank->CheckTank(szTankName);
	if(tankStatus != 79 && tankStatus != 82)
	{
		writer.writeStartElement("Error");
		writer.writeCharacters("TDT tank not open");
		writer.writeEndElement();
		stopCOM();
		return xmlData;
	}

	int numSpikeSamples;
	double lastSpikeTimestamp = 0.0;
	QVector<SpikeDetails> spikeList;
	QVector<EventDetails> eventList;

	//Read spike samples
	numSpikeSamples = tdtTank->ReadEventsV(1000000,"Snip",0,0,lastTimestamp,0.0,"ALL");
	//numSpikeSamples = tdtTank->ReadEventsV(1000000,"Snip",0,0,0.0,0.0,"All");

	//This is really only used in simulations where we are pulling data from a tank that
	//is already full.  We do this to avoid taking forever to respond.
	if(numSpikeSamples >1000)
		numSpikeSamples = 1000;

	//load spike samples
	_variant_t spikeSampleArray, spikeTimestampArray, spikeChannelArray, spikeUnitArray, spikeSampleFrequencyArray;

	if(numSpikeSamples > 0)
	{
		spikeSampleArray = tdtTank->ParseEvV(0,numSpikeSamples);
		spikeTimestampArray = tdtTank->ParseEvInfoV(0,numSpikeSamples,6);
		spikeChannelArray = tdtTank->ParseEvInfoV(0,numSpikeSamples,4);
		spikeUnitArray = tdtTank->ParseEvInfoV(0,numSpikeSamples,5);
		spikeSampleFrequencyArray = tdtTank->ParseEvInfoV(0,numSpikeSamples,9);

		sampleRate = (float)((double *) spikeSampleFrequencyArray.parray->pvData)[0];
	}

	for(int i=0;i<numSpikeSamples;i++)
	{
		SpikeDetails spikeDetails;

		spikeDetails.chanNum = (int) ((double *) spikeChannelArray.parray->pvData)[i];
		spikeDetails.unitNum = (int) ((double *) spikeUnitArray.parray->pvData)[i] + 1;
		
		spikeDetails.timeStamp = ((double *) spikeTimestampArray.parray->pvData)[i];
		for(unsigned int j=0;j<spikeSampleArray.parray->rgsabound[1].cElements;j++)
		{
			double spikeVoltage = ((float *) spikeSampleArray.parray->pvData)[i*spikeSampleArray.parray->rgsabound[1].cElements+j];

			spikeDetails.sampleWaveform.append(spikeVoltage);
		}
		spikeList.append(spikeDetails);
	}
	if(numSpikeSamples == 1000)
		lastSpikeTimestamp = spikeList.last().timeStamp;
	else
		lastSpikeTimestamp = 0.0;



	//Read event codes
	int numEvents;
	numEvents = tdtTank->ReadEventsV(1000000,"Evnt",0,0,lastTimestamp,lastSpikeTimestamp,"All");
	//numEvents = tdtTank->ReadEventsV(1000000,"Evnt",0,0,0.0,lastSpikeTimestamp,"All");

	_variant_t eventCodeArray, eventTimestampArray;

	if(numEvents > 0)
	{
		eventCodeArray = tdtTank->ParseEvV(0,numEvents);
		eventTimestampArray = tdtTank->ParseEvInfoV(0,numEvents,6);
	}



	//load event codes
	for(int i=0; i<numEvents; i++)
	{
		EventDetails eventDetails;
		eventDetails.timeStamp = ((double *) eventTimestampArray.parray->pvData)[i];
		eventDetails.code = (int) ((double *) eventCodeArray.parray->pvData)[i];

		eventList.append(eventDetails);
	}


	//if we don't care about order, we could have output everyhting as we went.  
	//However, everthing will have to get sorted sooner or later.  We'll do a stable
	//sort on the event list and spike list, then we'll interleave the whole mess
	//generating XML as we go

	qStableSort(spikeList.begin(),spikeList.end(),spikeTimestampLessThan);
	qStableSort(eventList.begin(),eventList.end(),eventTimestampLessThan);

	//record the last timestamp
	//(This is annoying because calling last on an empty QList crashes things)
	if(eventList.isEmpty() && spikeList.isEmpty())
		lastTimestamp = lastTimestamp;
	else if (eventList.isEmpty())
		lastTimestamp = spikeList.last().timeStamp;
	else if (spikeList.isEmpty())
		lastTimestamp = eventList.last().timeStamp;
	else if(spikeList.last().timeStamp >= eventList.last().timeStamp)
		lastTimestamp = spikeList.last().timeStamp;
	else
		lastTimestamp = eventList.last().timeStamp;

	//number of events
	writer.writeStartElement("numEvents");
	writer.writeCharacters(QString("%1").arg(numSpikeSamples+numEvents));
	writer.writeEndElement();

	while(spikeList.size() != 0 && eventList.size() !=0)
	{
		//output a spike waveform
		if(spikeList.begin()->timeStamp <= eventList.begin()->timeStamp)
		{
			writer.writeStartElement("event");

			writer.writeStartElement("timestamp");
			writer.writeCharacters(QString("%1").arg(spikeList.begin()->timeStamp));
			writer.writeEndElement();

			writer.writeStartElement("eventType");
			writer.writeCharacters("spike");
			writer.writeEndElement();

			writer.writeStartElement("channel");
			writer.writeCharacters(QString("%1").arg(spikeList.begin()->chanNum));
			writer.writeEndElement();

			writer.writeStartElement("unit");
			writer.writeCharacters(QString("%1").arg(spikeList.begin()->unitNum));
			writer.writeEndElement();

			//waveform data
			writer.writeStartElement("wave");
			for(int i=0; i<spikeList.begin()->sampleWaveform.size(); i++)
			{
				writer.writeCharacters(QString("%1 ").arg(spikeList.begin()->sampleWaveform[i]));
			}
			writer.writeEndElement();
			
			writer.writeEndElement(); //end event

			//remove the just processed spike
			spikeList.erase(spikeList.begin());
		}
		//output an eventcode
		else if(eventList.begin()->timeStamp < spikeList.begin()->timeStamp)
		{
			writer.writeStartElement("event");

			writer.writeStartElement("timestamp");
			writer.writeCharacters(QString("%1").arg(eventList.begin()->timeStamp));
			writer.writeEndElement();

			writer.writeStartElement("eventType");
			writer.writeCharacters("external event");
			writer.writeEndElement();

			writer.writeStartElement("eventCode");
			writer.writeCharacters(QString("%1").arg(eventList.begin()->code));
			writer.writeEndElement();

			writer.writeEndElement(); //event

			//remove the just processed event
			eventList.erase(eventList.begin());


		}
	}
	stopCOM();
	return xmlData;
}

//comparison functions for easy sorting
bool TdtPlugin::spikeTimestampLessThan(const SpikeDetails &sd1, const SpikeDetails &sd2)
{
	return sd1.timeStamp < sd2.timeStamp;
}

bool TdtPlugin::eventTimestampLessThan(const EventDetails &ed1, const EventDetails &ed2)
{
	return ed1.timeStamp < ed2.timeStamp;
}

Q_EXPORT_PLUGIN2(ProxyPluginTDT, TdtPlugin)