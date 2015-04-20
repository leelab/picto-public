#include "HistogramPlot.h"

#include "../memleakdetect.h"

namespace Picto {

const QString HistogramPlot::type = "Histogram Plot";

HistogramPlot::HistogramPlot()
{
	AddDefinableProperty(QVariant::Double, "BinSize", 1.0);
}

/*!	\brief Constructs and returns a shared pointer to a new HistogramPlot
 */
QSharedPointer<Asset> HistogramPlot::Create()
{
	return QSharedPointer<Asset>(new HistogramPlot());
}

/*!	\brief Modifies the value in the indicated bin by the passed-in value.
 */
void HistogramPlot::adjustValue(double bin, double value)
{
	_adjustValue(long(bin / getBinSize()), value);
}

/*!	\brief Sets the value in the indicated bin to the passed-in value.
 */
void HistogramPlot::setValue(double bin, double value)
{
	_setValue(long(bin / getBinSize()), value);
}

/*!	\brief Eliminates the value in the indicated bin.
 */
void HistogramPlot::dropBin(double bin)
{
	_dropBin(long(bin / getBinSize()));
}

/*!	\brief Returns the value of the indicated bin.
 */
double HistogramPlot::getValue(double bin)
{
	return _getValue(long(bin / getBinSize()));
}

QSharedPointer<OperatorPlotHandler> HistogramPlot::getNewHandler()
{
	//Histogram does not need to add any features to the base class yet
	return BarBase::getNewHandler();
}

}; //namespace Picto
