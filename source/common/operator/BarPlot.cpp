#include "BarPlot.h"
#include "BarPlotPlotHandler.h"
#include "BarAxisHandler.h"

#include "../memleakdetect.h"

namespace Picto {

const QString BarPlot::type = "Bar Plot";

BarPlot::BarPlot()
	: m_pAxisHandler(nullptr)
{
	AddDefinableProperty(QVariant::Double, "BinSpacing", 0.2);
}

/*!	\brief Constructs and returns a shared pointer to a new BarPlot
 */
QSharedPointer<Asset> BarPlot::Create()
{
	return QSharedPointer<Asset>(new BarPlot());
}

void BarPlot::handleXLabels(long lLowerBound, long lUpperBound)
{
	emit handleXLabelsSig(lLowerBound, lUpperBound);
}

//! Gets the name of the indicated bin.
const QString BarPlot::getLabel(long bin) const
{
	return m_pAxisHandler->getLabel(bin);
}

//! Sets the name of the indicated bin.
void BarPlot::setLabel(long bin, QString name)
{
	m_qhBinsModified[currentSetName_] = true;
	m_pAxisHandler->submitLabel(bin, name);
}


void BarPlot::postDeserialize()
{
	BarBase::postDeserialize();

	m_pAxisHandler = new BarAxisHandler(0, 0);
}

void BarPlot::initView()
{
	BarBase::initView();

	emit initializeBarPlotSig(m_pAxisHandler);
}

QSharedPointer<OperatorPlotHandler> BarPlot::getNewHandler()
{
	return QSharedPointer<OperatorPlotHandler>(new BarPlotPlotHandler());
}

void BarPlot::connectDataSignals(QSharedPointer<OperatorPlotHandler> plotHandler)
{
	BarBase::connectDataSignals(plotHandler);

	QSharedPointer<BarPlotPlotHandler> barPlotHandler = plotHandler.objectCast<BarPlotPlotHandler>();

	connect(this, SIGNAL(initializeBarPlotSig(BarAxisHandler *)),
		barPlotHandler.data(), SLOT(initializeBarPlot(BarAxisHandler *)));
}

}; //namespace Picto
