#include <windows.h>
#include <tchar.h>

#include <QString>
#include <QLineEdit>
#include <QMessageBox>
#include <QXmlStreamWriter>

#include "tdtplugin.h"
#include "dialog.h"
#include "../../common/memleakdetect.h"

TdtPlugin::TdtPlugin()
{
	bDeviceRunning = false;
}

QString TdtPlugin::device() const
{
	return "TDT";
}

bool TdtPlugin::startCOM()
{
	if(!bDeviceRunning)
		if(startDevice() != NeuralDataAcqInterface::started)
			return false;
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

QList<QSharedPointer<Picto::DataUnit>> TdtPlugin::dumpData()
{
	QList<QSharedPointer<Picto::DataUnit>> returnList;
	QSharedPointer<Picto::NeuralDataUnit> neuralData;
	QSharedPointer<Picto::AlignmentDataUnit> alignData;
	QSharedPointer<Picto::LFPDataUnitPackage> lfpData;
	if(!startCOM())
	{
		return returnList;
	}

	long tankStatus = tdtTank->CheckTank(szTankName);
	if(tankStatus != 79 && tankStatus != 82)
	{
		//TDT Tank not open
		stopCOM();
		return returnList;
	}

	int numSpikeSamples;
	double readUntilTime = 0.0;
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
		readUntilTime = spikeList.last().timeStamp;
	else
		readUntilTime = 0.0;



	//Read event codes
	int numEvents;
	numEvents = tdtTank->ReadEventsV(1000000,"Evnt",0,0,lastTimestamp,readUntilTime,"All");
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









	//Read lfp codes
	int numLFP;
	numLFP = tdtTank->ReadEventsV(1000000,"LDec",0,0,lastTimestamp,readUntilTime,"All");
	//numEvents = tdtTank->ReadEventsV(1000000,"Evnt",0,0,0.0,lastSpikeTimestamp,"All");

	_variant_t lfpSampleArray, lfpChannelArray, lfpTimestampArray, lfpFreqArray;
	double lastLFPTimestamp = lastTimestamp;
	if(numLFP > 0)
	{
		lfpSampleArray = tdtTank->ParseEvV(0,numLFP);
		lfpChannelArray = tdtTank->ParseEvInfoV(0,numLFP,4);
		lfpTimestampArray = tdtTank->ParseEvInfoV(0,numLFP,6);
		lfpFreqArray = tdtTank->ParseEvInfoV(0,numLFP,9);



		//load lfp codes
		//They are returned in lfpSampleArray as an 2D array, where each column
		//corresponds to a timestamp/channel in the timestamp/channel arrays, and
		//each row corresponds to a new time = columntimestamp + 1/freqArray value.

		double sampPerSec(((double *) lfpFreqArray.parray->pvData)[0]);
		double secPerSamp(1.0/sampPerSec);
		int numChans;
		int maxChan;
		double* potentials;
		int* chans;
		double currTime;
		lfpData = QSharedPointer<Picto::LFPDataUnitPackage>(new Picto::LFPDataUnitPackage());

		for(int i=0; i<numLFP; i=i+numChans)
		{
			//The minimum num of channels with the same timestamp is one.
			numChans = 1;
			maxChan = 0;
			
			//Get the next timestamp, "samples per second" and "seconds per sample"
			currTime = ((double *) lfpTimestampArray.parray->pvData)[i];
			sampPerSec = ((double *) lfpFreqArray.parray->pvData)[i];
			secPerSamp = 1.0/sampPerSec;
			
			//Find how many channels start with this time and which is greatest
			while(currTime==((double *) lfpTimestampArray.parray->pvData)[i+numChans])
				numChans++;

			//Create index to potentials array map
			chans = new int[numChans];
			// fill the index to potentials array map
			for(int arrayInd = 0;arrayInd<numChans;arrayInd++)
			{
				chans[arrayInd] = (int) ((double *) lfpChannelArray.parray->pvData)[i+arrayInd];
				if(chans[arrayInd]>maxChan)
					maxChan = chans[arrayInd];
			}
			//Create potentials array.  Use maxChan+1 so that each channel number can be placed in its index.  ie. If maxchan is 6 it needs to go 
			//into index 6 which is the 7th index.
			potentials = new double[maxChan+1];
			
			//Loop through the list of "sec per sample" separated enties for all channels that start with the same timestamp.
			//We assume here that all entries have the same "sec per sample" value, which is currently a valid assumption for the
			//tdt system.
			for(unsigned int j=0;j<lfpSampleArray.parray->rgsabound[1].cElements;j++)
			{
				for(int arrayInd = 0;arrayInd<numChans;arrayInd++)
				{
					//Initialize potentials array to have zero values
					for(int n=0;n<maxChan;n++)
					{
						potentials[n] = 0;
					}
					//If the lfpDataStore object is getting too big, add it to the list and make a new one.
					if(lfpData->numSamples() >= 10000)
					{
						returnList.push_back(lfpData);
						lfpData = QSharedPointer<Picto::LFPDataUnitPackage>(new Picto::LFPDataUnitPackage());
					}
					potentials[chans[arrayInd]] = ((short *) lfpSampleArray.parray->pvData)[((i+arrayInd)*lfpSampleArray.parray->rgsabound[1].cElements)+j];
					lfpData->addData(currTime,potentials,numChans);
				}
				currTime += secPerSamp;
			}
			//Get rid of the dynamically constructed arrays
			delete potentials;
			delete chans;
			if(currTime > lastLFPTimestamp)
				lastLFPTimestamp = currTime;
		}
		//Add the last LFPDataUnitPackage to the return list
		if(lfpData->numSamples() > 0)
			returnList.push_back(lfpData);
	}












	//if we don't care about order, we could have output everyhting as we went.  
	//However, everthing will have to get sorted sooner or later.  We'll do a stable
	//sort on the event list and spike list, then we'll interleave the whole mess
	//generating XML as we go

	qStableSort(spikeList.begin(),spikeList.end(),spikeTimestampLessThan);
	qStableSort(eventList.begin(),eventList.end(),eventTimestampLessThan);

	//record the last timestamp
	//(This is annoying because calling last on an empty QList crashes things)
	double lastEventTimestamp = eventList.isEmpty()?lastTimestamp: eventList.last().timeStamp;
	double lastSpikeTimestamp = spikeList.isEmpty()?lastTimestamp: spikeList.last().timeStamp;
	if(lastEventTimestamp > lastTimestamp)
		lastTimestamp = lastEventTimestamp;
	if(lastSpikeTimestamp > lastTimestamp)
		lastTimestamp = lastSpikeTimestamp;
	if(lastLFPTimestamp > lastTimestamp)
		lastTimestamp = lastLFPTimestamp;

	while(spikeList.size() != 0 && eventList.size() !=0)
	{
		//output a spike waveform
		if(spikeList.begin()->timeStamp <= eventList.begin()->timeStamp)
		{
			neuralData = QSharedPointer<Picto::NeuralDataUnit>(new Picto::NeuralDataUnit());
			neuralData->setTimestamp(spikeList.begin()->timeStamp);
			neuralData->setChannel(spikeList.begin()->chanNum);
			neuralData->setUnit(spikeList.begin()->unitNum);

			//waveform data
			QSharedPointer<QList<int>> waveform(new QList<int>);
			for(int i=0; i<spikeList.begin()->sampleWaveform.size(); i++)
			{
				waveform->push_back(spikeList.begin()->sampleWaveform[i]);
			}
			neuralData->setWaveform(waveform);
			returnList.push_back(neuralData);

			//remove the just processed spike
			spikeList.erase(spikeList.begin());
		}
		//output an eventcode
		else if(eventList.begin()->timeStamp < spikeList.begin()->timeStamp)
		{
			alignData = QSharedPointer<Picto::AlignmentDataUnit>(new Picto::AlignmentDataUnit());
			alignData->setTimestamp(eventList.begin()->timeStamp);
			alignData->setAlignCode(eventList.begin()->code);

			returnList.push_back(alignData);
			//remove the just processed event
			eventList.erase(eventList.begin());
		}
	}
	stopCOM();
	return returnList;
}

void TdtPlugin::deviceSelected()
{
	bool keepTrying;
	do
	{
		keepTrying = false;
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
			keepTrying = true;
			continue;
		}
		hr = tdtTank.CreateInstance("TTank.X"); //appId = "{670490CE-57D2-4176-8E74-80C4C6A47D88}"; //"TTankX.ocx"

		if (FAILED(hr)) {
			errorMsgBox.setText(tr("Couldn't start the TDT interface. Are the TDT ActiveX controls installed?"));
			errorMsgBox.exec();
			keepTrying = true;
			continue;
		}

		try
		{
			if(!(tdtTank->ConnectServer(szServerName,"PictoProxyServer")))
			{
				errorMsgBox.setText(tr("Couldn't connect to TDT server: %1").arg(serverName));
				errorMsgBox.exec();
				keepTrying = true;
				continue;
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
			keepTrying = true;
			continue;
		}

		if(!(tdtTank->OpenTank(szTankName,"R")))
		{
			errorMsgBox.setText(tr("Couldn't open the tank: %1").arg(tankName));
			errorMsgBox.exec();
			tdtTank->ReleaseServer();
			keepTrying = true;
			continue;
		}

		if(!(tdtTank->SelectBlock(szBlockName)))
		{
			errorMsgBox.setText(tr("Couldn't select block: %1").arg(blockName));
			errorMsgBox.exec();
			tdtTank->CloseTank();
			tdtTank->ReleaseServer();
			keepTrying = true;
			continue;
		}
		

		sampleRate=0;
		lastTimestamp=0;

		//Close everything back up...
		tdtTank->CloseTank();
		tdtTank->ReleaseServer();
		CoUninitialize();
	}while(keepTrying);

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