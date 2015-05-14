#include "SamplingBarPlot.h"
#include "SamplingBarPlotPlotHandler.h"
#include "BarAxisHandler.h"

#include "../memleakdetect.h"

namespace Picto {

const QString SamplingBarPlot::type = "Sampling Bar Plot";

SamplingBarPlot::SamplingBarPlot()
	: m_pAxisHandler(nullptr)
{
	AddDefinableProperty(QVariant::Double, "BinSpacing", 0.2);
}

/*!	\brief Constructs and returns a shared pointer to a new SamplingBarPlot
 */
QSharedPointer<Asset> SamplingBarPlot::Create()
{
	return QSharedPointer<Asset>(new SamplingBarPlot());
}

void SamplingBarPlot::postDeserialize()
{
	SamplingBarBase::postDeserialize();

	m_pAxisHandler = new BarAxisHandler(0, 0);
}

void SamplingBarPlot::initView()
{
	SamplingBarBase::initView();

	emit initializeSamplingBarPlotSig(m_pAxisHandler);
}

void SamplingBarPlot::handleXLabels(long lLowerBound, long lUpperBound)
{
	emit handleXLabelsSig(lLowerBound, lUpperBound);
}

//! Gets the name of the indicated bin.
const QString SamplingBarPlot::getLabel(long bin) const
{
	return m_pAxisHandler->getLabel(bin);
}

//! Sets the name of the indicated bin.
void SamplingBarPlot::setLabel(long bin, QString name)
{
	m_pAxisHandler->submitLabel(bin, name);
}

QSharedPointer<OperatorPlotHandler> SamplingBarPlot::getNewHandler()
{
	return QSharedPointer<OperatorPlotHandler>(new SamplingBarPlotPlotHandler());
}


void SamplingBarPlot::connectDataSignals(QSharedPointer<OperatorPlotHandler> plotHandler)
{
	SamplingBarBase::connectDataSignals(plotHandler);

	QSharedPointer<SamplingBarPlotPlotHandler> barPlotHandler = plotHandler.objectCast<SamplingBarPlotPlotHandler>();

	connect(this, SIGNAL(initializeSamplingBarPlotSig(BarAxisHandler *)),
		barPlotHandler.data(), SLOT(initializeSamplingBarPlot(BarAxisHandler *)));

}

}; //namespace Picto
