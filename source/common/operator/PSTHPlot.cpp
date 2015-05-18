#include "PSTHPlot.h"

#include "../memleakdetect.h"

namespace Picto {

const QString PSTHPlot::type = "PSTH Plot";

PSTHPlot::PSTHPlot()
{

	QStringList alignList;
	alignList << "BeginWindow" << "AlignEvent" << "EndWindow";
	AddDefinableProperty(PropertyContainer::enumTypeId(), "AlignData", 0, "enumNames", alignList);
}

/*!	\brief Constructs and returns a shared pointer to a new SamplingHistogramPlot
 */
QSharedPointer<Asset> PSTHPlot::Create()
{
	return QSharedPointer<Asset>(new PSTHPlot());
}

QSharedPointer<OperatorPlotHandler> PSTHPlot::getNewHandler()
{
	//Histogram does not need to add any features to the base class yet
	return SamplingBarBase::getNewHandler();
}


void PSTHPlot::beginTrial()
{

}

void PSTHPlot::endTrial()
{

}

void PSTHPlot::beginRecordingWindow()
{

}

void PSTHPlot::endRecordingWindow()
{

}

void PSTHPlot::alignEvent()
{

}

}; //namespace Picto
