#include "PSTHPlot.h"

#include "../memleakdetect.h"

namespace Picto {

const QString PSTHPlot::type = "PSTH Plot";

PSTHPlot::PSTHPlot()
	: currentRecordingState_(RecordingState::NotRecording)
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

void PSTHPlot::beginRecordingWindow()
{
	if (currentRecordingState_ != NotRecording)
	{
		clearAccumulatedData();
	}

	currentRecordingState_ = RecordingState::RecordingBegan;

	if (getAlignType() == AlignType::ALIGN_BEGIN)
	{
		alignEvent();
	}
}

void PSTHPlot::endRecordingWindow()
{
	if (getAlignType() == AlignType::ALIGN_END)
	{
		alignEvent();
	}

	submitAccumulatedData();
	currentRecordingState_ = RecordingState::NotRecording;
	clearAccumulatedData();
}

void PSTHPlot::alignEvent()
{
	if (currentRecordingState_ == RecordingState::RecordingBegan)
	{
		currentRecordingState_ = RecordingState::AlignOccurred;
	}
}

void PSTHPlot::clearAccumulatedData()
{

}

void PSTHPlot::submitAccumulatedData()
{

}

AlignType::AlignType PSTHPlot::getAlignType() const
{
	return (AlignType::AlignType)(propertyContainer_->getPropertyValue("AlignData").toInt());
}

}; //namespace Picto
