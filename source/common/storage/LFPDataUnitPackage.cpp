#include "LFPDataUnitPackage.h"
#include "../memleakdetect.h"

using namespace Picto;
LFPDataUnitPackage::LFPDataUnitPackage()
{
	channel_ = -1;
	correlation_ = 0;
	timestamp_ = QString::number(0,'e',14);;
	fittedTimestamp_ = QString::number(0,'e',14);;
	potentials_.clear();
	numSamples_ = 0;
	potArray_ = NULL;
	fittedResolution_ = 0;
	resolution_ = 0;
}

/*! \brief Turns the LFPDataUnitPackage into an XML fragment
 *	
 */
bool LFPDataUnitPackage::serializeAsXml(QSharedPointer<QXmlStreamWriter> xmlStreamWriter)
{
	xmlStreamWriter->writeStartElement("LFPDataUnitPackage");
	xmlStreamWriter->writeAttribute("ch",QString("%1").arg(channel_));
	xmlStreamWriter->writeAttribute("timestamp",QString("%1").arg(timestamp_));
	xmlStreamWriter->writeAttribute("ftimestamp",QString("%1").arg(fittedTimestamp_));
	xmlStreamWriter->writeAttribute("correlation",QString("%1").arg(correlation_));
	xmlStreamWriter->writeAttribute("resolution",QString("%1").arg(resolution_,0,'e',14));
	xmlStreamWriter->writeAttribute("fresolution",QString("%1").arg(fittedResolution_,0,'e',14));
	xmlStreamWriter->writeAttribute("pots",getPotentials());
	DataUnit::serializeDataID(xmlStreamWriter);
	xmlStreamWriter->writeEndElement();
	return true;
}
/*! Converts XML into a LFPDataUnitPackage object.  Note that this deletes any existing data.*/
bool LFPDataUnitPackage::deserializeFromXml(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	//Do some basic error checking
	if(!xmlStreamReader->isStartElement() || xmlStreamReader->name() != "LFPDataUnitPackage")
	{
		addError("Incorrect tag, expected <LFPDataUnitPackage>");
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
				addError("LFPDataUnitPackage missing ch (channel) attribute");
				return false;
			}

			if(xmlStreamReader->attributes().hasAttribute("timestamp"))
			{
				timestamp_ = xmlStreamReader->attributes().value("timestamp").toString();
			}
			else
			{
				addError("LFPDataUnitPackage missing correlation attribute");
				return false;
			}

			if(xmlStreamReader->attributes().hasAttribute("ftimestamp"))
			{
				fittedTimestamp_ = xmlStreamReader->attributes().value("ftimestamp").toString();
			}
			else
			{
				addError("LFPDataUnitPackage missing ftimestamp (fitted timestamp) attribute");
				return false;
			}

			if(xmlStreamReader->attributes().hasAttribute("correlation"))
			{
				correlation_ = xmlStreamReader->attributes().value("correlation").toString().toDouble();
			}
			else
			{
				addError("LFPDataUnitPackage missing correlation attribute");
				return false;
			}

			if(xmlStreamReader->attributes().hasAttribute("resolution"))
			{
				resolution_ = xmlStreamReader->attributes().value("resolution").toString().toDouble();
			}
			else
			{
				addError("LFPDataUnitPackage missing resolution attribute");
				return false;
			}

			if(xmlStreamReader->attributes().hasAttribute("fresolution"))
			{
				fittedResolution_ = xmlStreamReader->attributes().value("fresolution").toString().toDouble();
			}
			else
			{
				addError("LFPDataUnitPackage missing fresolution (fitted resolution) attribute");
				return false;
			}


			if(xmlStreamReader->attributes().hasAttribute("pots"))
			{
				potentials_ = xmlStreamReader->attributes().value("pots").toString().split(" ",QString::SkipEmptyParts);
				numSamples_ = potentials_.size();
			}
			else
			{
				addError("LFPDataUnitPackage missing pots(potentials) attribute");
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

/*! \brief Adds the input list of Local Field Potential readings to this package.
 *	@param adPotentialReadings An array of potential readings
 *	@param numVals the size of the adPotentialReadings array
 */
void LFPDataUnitPackage::appendData(const float* adPotentialReadings, int numVals)
{
	QString newVals;
	for(int i=0;i<numVals;i++)
	{
		potentials_.push_back(QString("%1").arg(adPotentialReadings[i]));
	}
	numSamples_ = potentials_.length();
	Q_ASSERT_X(numSamples_ <= 10000,"LFPDataUnitPackage::addData","No more than 10000 lfp samples should be stored in a lfpdatastore");
}

/*! \brief Appends the input value to the list of lfp readings.*/
void LFPDataUnitPackage::appendData(float adPotentialReading)
{
	appendData(&adPotentialReading,1);
}
/*! \brief Returns the list of LFP potential readings as a byte array.*/
QByteArray LFPDataUnitPackage::getPotentialsAsByteArray()
{
	float* pots = new float[potentials_.size()];
	for(int i=0;i<potentials_.size();i++)
	{
		pots[i] = potentials_[i].toFloat();
	}
	//Note: We must create the byte array with the constructor (not setRawData of fromRawData)
	//So that we can then delete the pots array without problems.
	QByteArray returnVal(reinterpret_cast<const char*>(pots),potentials_.size()*sizeof(float));
	delete[] pots;
	return returnVal;
}

/*! \brief Returns the list of LFP potential readings as a linked list of QPointF objects where the 
 *	'xPos' is the timestamp (aligned with the behavioral time stream) and 'yPos' is the potential value.
 *	This is useful for plotting lfp data.
 */
QLinkedList<QPointF> LFPDataUnitPackage::getAlignedDataAsLinkedList()
{
	QLinkedList<QPointF> returnVal;
	double dTimestamp = getFittedTimestamp().toDouble();
	double resolution = getFittedResolution();
	int i=0;
	foreach(QString potential,potentials_)
	{
		returnVal.push_back(QPointF(dTimestamp+(i*resolution),potential.toDouble()));
		i++;
	}
	return returnVal;
}

/*! \brief Sets the list of LFP potential readings from a byte array (as returned from getPotentialsAsByteArray()).*/
void LFPDataUnitPackage::setPotentialsFromByteArray(QByteArray potentials)
{
	const float* pots = reinterpret_cast<const float*>(potentials.constData());
	potentials_.clear();
	appendData(pots,potentials.size()/sizeof(float));
}


