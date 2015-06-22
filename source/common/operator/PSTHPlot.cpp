#include "PSTHPlot.h"

#include "../memleakdetect.h"

namespace Picto {

const QString PSTHPlot::type = "PSTH Plot";

PSTHPlot::PSTHPlot()
	: currentRecordingState_(RecordingState::NotRecording)
{

	AddDefinableProperty(QVariant::Double, "PreFlagWindow", 400);
	AddDefinableProperty(QVariant::Double, "PostFlagWindow", 400);
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


void PSTHPlot::alignEvent()
{

}

void PSTHPlot::clearAccumulatedData()
{

}

void PSTHPlot::submitAccumulatedData()
{

}


double PSTHPlot::getPreFlagWindow() const
{
	return propertyContainer_->getPropertyValue("PreFlagWindow").toDouble();
}

double PSTHPlot::getPostFlagWindow() const
{
	return propertyContainer_->getPropertyValue("PostFlagWindow").toDouble();
}

}; //namespace Picto
