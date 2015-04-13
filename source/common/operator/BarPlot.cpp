#include "BarPlot.h"
#include "BarAxisHandler.h"

#include "../memleakdetect.h"

namespace Picto {

const QString BarPlot::type = "Bar Plot";

BarPlot::BarPlot()
	: m_pAxisHandler(nullptr)
{
	m_pAxisHandler = new BarAxisHandler(0, 1);

	m_pPlot->setAxisScaleDiv(QwtPlot::xBottom, m_pAxisHandler->getScaleDiv());
	m_pPlot->setAxisScaleDraw(QwtPlot::xBottom, m_pAxisHandler);

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
	m_pAxisHandler->rescale(lLowerBound, lUpperBound);
	m_pPlot->setAxisScaleDiv(QwtPlot::xBottom, m_pAxisHandler->getScaleDiv());
}

//! Gets the name of the indicated bin.
const QString BarPlot::getLabel(long bin) const
{
	return m_pAxisHandler->getLabel(bin);
}

//! Sets the name of the indicated bin.
void BarPlot::setLabel(long bin, QString name)
{
	m_pAxisHandler->submitLabel(bin, name);
}

}; //namespace Picto
