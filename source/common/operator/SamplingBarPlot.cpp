#include "SamplingBarPlot.h"
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

	m_pAxisHandler = new BarAxisHandler(0, 1);

	m_pPlot->setAxisScaleDiv(QwtPlot::xBottom, m_pAxisHandler->getScaleDiv());
	m_pPlot->setAxisScaleDraw(QwtPlot::xBottom, m_pAxisHandler);
}

void SamplingBarPlot::handleXLabels(long lLowerBound, long lUpperBound)
{
	m_pAxisHandler->rescale(lLowerBound, lUpperBound);
	m_pPlot->setAxisScaleDiv(QwtPlot::xBottom, m_pAxisHandler->getScaleDiv());
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

}; //namespace Picto
