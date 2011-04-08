#include "LFPDataStore.h"

using namespace Picto;
LFPDataStore::LFPDataStore()
{
	channel_ = -1;
	correlation_ = 0;
	times_ = "";
	fittedTimes_ = "";
	potentials_.clear();
	numSamples_ = 0;
}

/*! \brief Turns the FrameDataStore into an XML fragment
 *
 *	The XML will look like this:
 *	<FrameUnitDataStore time=1.234 state=somestate>1</FrameUnitDataStore>
 *	
 */
bool LFPDataStore::serializeAsXml(QSharedPointer<QXmlStreamWriter> xmlStreamWriter)
{
	xmlStreamWriter->writeStartElement("LFPDataStore");
	xmlStreamWriter->writeAttribute("correlation",QString("%1").arg(correlation_));
	xmlStreamWriter->writeAttribute("numsamples",QString("%1").arg(numSamples_));
	xmlStreamWriter->writeTextElement("times",times_);
	xmlStreamWriter->writeTextElement("fittedtimes",fittedTimes_);
	for(int i=0;i<potentials_.size();i++)
	{
		xmlStreamWriter->writeTextElement("ch"+QString::number(i).toAscii(),potentials_[i]);
	}
	DataStore::serializeAsXml(xmlStreamWriter);
	xmlStreamWriter->writeEndElement();
	return true;
}
//! Converts XML into a FrameDataStore object.  Note that this deletes any existing data.
bool LFPDataStore::deserializeFromXml(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	//Do some basic error checking
	if(!xmlStreamReader->isStartElement() || xmlStreamReader->name() != "LFPDataStore")
	{
		addError("LFPDataStore","Incorrect tag, expected <LFPDataStore>",xmlStreamReader);
		return false;
	}

	while(!(xmlStreamReader->isEndElement() && xmlStreamReader->name().toString() == "LFPDataStore") && !xmlStreamReader->atEnd())
	{
		if(!xmlStreamReader->isStartElement())
		{
			//Do nothing unless we're at a start element
			xmlStreamReader->readNext();
			continue;
		}

		QString name = xmlStreamReader->name().toString();
		if(name == "LFPDataStore")
		{
			if(xmlStreamReader->attributes().hasAttribute("correlation"))
			{
				correlation_ = xmlStreamReader->attributes().value("correlation").toString().toDouble();
			}
			else
			{
				addError("LFPDataStore","LFPDataStore missing correlation attribute",xmlStreamReader);
				return false;
			}
			if(xmlStreamReader->attributes().hasAttribute("numsamples"))
			{
				numSamples_ = xmlStreamReader->attributes().value("numsamples").toString().toInt();
			}
			else
			{
				addError("LFPDataStore","LFPDataStore missing numsamples attribute",xmlStreamReader);
				return false;
			}
		}
		else if(name == "times")
		{
			times_ = xmlStreamReader->readElementText();
		}
		else if(name == "fittedtimes")
		{
			fittedTimes_ = xmlStreamReader->readElementText();
		}
		else if(name.startsWith("ch"))
		{
			int chNum = name.mid(2).toInt();
			while(potentials_.size() <= chNum)
				potentials_.push_back("");
			potentials_[chNum] = xmlStreamReader->readElementText();
		}
		else
		{
			DataStore::deserializeFromXml(xmlStreamReader);
		}
		xmlStreamReader->readNext();
	}
	return true;
}

void LFPDataStore::addData(double timestamp, double* potentials, int numVals)
{
	while(potentials_.size() < numVals)
	{
		QString newChan;
		for(int i=0;i<numSamples_;i++)
			newChan.append(" 0");
		potentials_.push_back(newChan);
	}
	times_.append(" ").append(QString::number(timestamp));
	for(int i=0;i<numVals;i++)
	{		
		potentials_[i].append(" ").append(QString::number(potentials[i]));
	}
	numSamples_++;
	Q_ASSERT_X(numSamples_ <= 10000,"LFPDataStore::addData","No more than 10000 lfp samples should be stored in a lfpdatastore");

}
void LFPDataStore::addData(double timestamp, double* potentials, int numVals, double fittedtime)
{
	fittedTimes_.push_back(QString::number(fittedtime));
	addData(timestamp,potentials,numVals);
}

void LFPDataStore::addDataByBlock(lfpDataBlock* block)
{
	if(!block->data.size())
		return;
	QMap<int,QList<double>::Iterator> iters;
	int maxChan = -1;
	for(QMap<int,QList<double>>::iterator iter = block->data.begin();iter!=block->data.end();iter++)
	{
		iters[iter.key()] = iter.value().begin();
		if(iter.key() > maxChan)
			maxChan = iter.key();
	}
	double *data = new double[maxChan+1];
	bool done = false;
	double timestamp = block->timestamp_;
	while(!done)
	{
		for(int i=0;i<maxChan+1;i++)
		{
			if(iters.contains(i))
			{
				data[i] = (*(iters[i]));
				iters[i]++;
				if(iters[i] == block->data[i].end())
					done = true;
			}
			else
				data[i] = 0;
		}
		addData(timestamp,data,maxChan+1);
		timestamp += block->timePerSample_;
	}
	delete data;
	

}


