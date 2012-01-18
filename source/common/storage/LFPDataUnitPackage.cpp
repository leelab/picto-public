#include "LFPDataUnitPackage.h"
#include "../memleakdetect.h"

#define LFPDECPLACES 6
using namespace Picto;
LFPDataUnitPackage::LFPDataUnitPackage()
{
	channel_ = -1;
	correlation_ = 0;
	timestamp_ = QString::number(0,'e',6);;
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
	xmlStreamWriter->writeAttribute("ch",QString("%1").arg(channel_));
	xmlStreamWriter->writeAttribute("timestamp",QString("%1").arg(timestamp_));
	xmlStreamWriter->writeAttribute("correlation",QString("%1").arg(correlation_));
	xmlStreamWriter->writeAttribute("resolution",QString("%1").arg(resolution_,13,'e',6));
	xmlStreamWriter->writeAttribute("pots",potentials_);
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
			if(xmlStreamReader->attributes().hasAttribute("ch"))
			{
				channel_ = xmlStreamReader->attributes().value("ch").toString().toInt();
			}
			else
			{
				addError("LFPDataUnitPackage","LFPDataUnitPackage missing ch (channel) attribute",xmlStreamReader);
				return false;
			}

			if(xmlStreamReader->attributes().hasAttribute("timestamp"))
			{
				timestamp_ = xmlStreamReader->attributes().value("timestamp").toString();
			}
			else
			{
				addError("LFPDataUnitPackage","LFPDataUnitPackage missing correlation attribute",xmlStreamReader);
				return false;
			}

			if(xmlStreamReader->attributes().hasAttribute("correlation"))
			{
				correlation_ = xmlStreamReader->attributes().value("correlation").toString().toDouble();
			}
			else
			{
				addError("LFPDataUnitPackage","LFPDataUnitPackage missing correlation attribute",xmlStreamReader);
				return false;
			}

			if(xmlStreamReader->attributes().hasAttribute("resolution"))
			{
				resolution_ = xmlStreamReader->attributes().value("resolution").toString().toDouble();
			}
			else
			{
				addError("LFPDataUnitPackage","LFPDataUnitPackage missing resolution attribute",xmlStreamReader);
				return false;
			}

			if(xmlStreamReader->attributes().hasAttribute("pots"))
			{
				potentials_ = xmlStreamReader->attributes().value("pots").toString();
			}
			else
			{
				addError("LFPDataUnitPackage","LFPDataUnitPackage missing pots(potentials) attribute",xmlStreamReader);
				return false;
			}
		}
		else
		{
			DataUnit::deserializeDataID(xmlStreamReader);
		}
		xmlStreamReader->readNext();
	}
	return true;
}

void LFPDataUnitPackage::postDeserialize()
{

}

bool LFPDataUnitPackage::validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	return true;
}

void LFPDataUnitPackage::addData(double* potentials, int numVals, int timeIndex)
{
	QString newVals;
	for(int i=0;i<numVals;i++)
	{
		newVals.append(QString("%1 ").arg(potentials[i],7+LFPDECPLACES,'e',LFPDECPLACES));
	}
	potentials_.replace(timeIndex*(8+LFPDECPLACES),newVals.length(),newVals);	//8+LFPDECPLACES=sign+first digit + decimal + LFPDECPLACES decimal places + "e+??" + space
	numSamples_ = potentials_.length()/(8+LFPDECPLACES);
	Q_ASSERT_X(numSamples_ <= 10000,"LFPDataUnitPackage::addData","No more than 10000 lfp samples should be stored in a lfpdatastore");
}
void LFPDataUnitPackage::addData(double potential,int timeIndex)
{
	addData(&potential,1,timeIndex);
}
void LFPDataUnitPackage::addDataAtNextIndex(double potential)
{
	addData(potential,numSamples());
}
//void LFPDataUnitPackage::addDataByBlock(lfpDataBlock* block)
//{
//	if(!block->data.size())
//		return;
//	QMap<int,QList<double>::Iterator> iters;
//	int maxChan = -1;
//	for(QMap<int,QList<double>>::iterator iter = block->data.begin();iter!=block->data.end();iter++)
//	{
//		iters[iter.key()] = iter.value().begin();
//		if(iter.key() > maxChan)
//			maxChan = iter.key();
//	}
//	double *data = new double[maxChan+1];
//	bool done = false;
//	double timestamp = block->timestamp_;
//	while(!done)
//	{
//		for(int i=0;i<maxChan+1;i++)
//		{
//			if(iters.contains(i))
//			{
//				data[i] = (*(iters[i]));
//				iters[i]++;
//				if(iters[i] == block->data[i].end())
//					done = true;
//			}
//			else
//				data[i] = 0;
//		}
//		addData(timestamp,data,maxChan+1);
//		timestamp += block->timePerSample_;
//	}
//	delete[] data;
//	
//
//}


