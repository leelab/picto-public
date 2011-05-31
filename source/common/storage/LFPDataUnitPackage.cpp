#include "LFPDataUnitPackage.h"

using namespace Picto;
LFPDataUnitPackage::LFPDataUnitPackage()
{
	channel_ = -1;
	correlation_ = 0;
	times_ = "";
	fittedTimes_ = "";
	potentials_.clear();
	numSamples_ = 0;
}

/*! \brief Turns the FrameDataUnitPackage into an XML fragment
 *
 *	The XML will look like this:
 *	<FrameDataUnit time=1.234 state=somestate>1</FrameDataUnit>
 *	
 */
bool LFPDataUnitPackage::serializeAsXml(QSharedPointer<QXmlStreamWriter> xmlStreamWriter)
{
	xmlStreamWriter->writeStartElement("LFPDataUnitPackage");
	xmlStreamWriter->writeAttribute("correlation",QString("%1").arg(correlation_));
	xmlStreamWriter->writeAttribute("numsamples",QString("%1").arg(numSamples_));
	xmlStreamWriter->writeTextElement("times",times_);
	xmlStreamWriter->writeTextElement("fittedtimes",fittedTimes_);
	for(int i=0;i<potentials_.size();i++)
	{
		xmlStreamWriter->writeTextElement("ch"+QString::number(i).toAscii(),potentials_[i]);
	}
	DataUnit::serializeDataID(xmlStreamWriter);
	xmlStreamWriter->writeEndElement();
	return true;
}
//! Converts XML into a FrameDataUnitPackage object.  Note that this deletes any existing data.
bool LFPDataUnitPackage::deserializeFromXml(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	//Do some basic error checking
	if(!xmlStreamReader->isStartElement() || xmlStreamReader->name() != "LFPDataUnitPackage")
	{
		addError("LFPDataUnitPackage","Incorrect tag, expected <LFPDataUnitPackage>",xmlStreamReader);
		return false;
	}

	while(!(xmlStreamReader->isEndElement() && xmlStreamReader->name().toString() == "LFPDataUnitPackage") && !xmlStreamReader->atEnd())
	{
		if(!xmlStreamReader->isStartElement())
		{
			//Do nothing unless we're at a start element
			xmlStreamReader->readNext();
			continue;
		}

		QString name = xmlStreamReader->name().toString();
		if(name == "LFPDataUnitPackage")
		{
			if(xmlStreamReader->attributes().hasAttribute("correlation"))
			{
				correlation_ = xmlStreamReader->attributes().value("correlation").toString().toDouble();
			}
			else
			{
				addError("LFPDataUnitPackage","LFPDataUnitPackage missing correlation attribute",xmlStreamReader);
				return false;
			}
			if(xmlStreamReader->attributes().hasAttribute("numsamples"))
			{
				numSamples_ = xmlStreamReader->attributes().value("numsamples").toString().toInt();
			}
			else
			{
				addError("LFPDataUnitPackage","LFPDataUnitPackage missing numsamples attribute",xmlStreamReader);
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
			DataUnit::deserializeDataID(xmlStreamReader);
		}
		xmlStreamReader->readNext();
	}
	return true;
}

bool LFPDataUnitPackage::validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	return true;
}

void LFPDataUnitPackage::addData(double timestamp, double* potentials, int numVals)
{
	while(potentials_.size() < numVals)
	{
		QString newChan;
		for(int i=0;i<numSamples_;i++)
			newChan.append(" 0");
		potentials_.push_back(newChan);
	}
	times_.append(" ").append(QString::number(timestamp,'e',6));
	for(int i=0;i<numVals;i++)
	{		
		potentials_[i].append(" ").append(QString::number(potentials[i]));
	}
	numSamples_++;
	Q_ASSERT_X(numSamples_ <= 10000,"LFPDataUnitPackage::addData","No more than 10000 lfp samples should be stored in a lfpdatastore");

}
void LFPDataUnitPackage::addData(double timestamp, double* potentials, int numVals, double fittedtime)
{
	fittedTimes_.push_back(QString::number(fittedtime,'e',6));
	addData(timestamp,potentials,numVals);
}

void LFPDataUnitPackage::addDataByBlock(lfpDataBlock* block)
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


