#include "SamplingHistogramPlot.h"

#include "../memleakdetect.h"

namespace Picto {

const QString SamplingHistogramPlot::type = "Sampling Histogram Plot";

SamplingHistogramPlot::SamplingHistogramPlot()
{
	AddDefinableProperty(QVariant::Double, "BinSize", 1.0);
}

/*!	\brief Constructs and returns a shared pointer to a new SamplingHistogramPlot
 */
QSharedPointer<Asset> SamplingHistogramPlot::Create()
{
	return QSharedPointer<Asset>(new SamplingHistogramPlot());
}

//! Modifies the value in the indicated bin by the passed-in value.
void SamplingHistogramPlot::submitValue(double bin, double value)
{
	_submitValue(long(bin / getBinSize()), value);
}

//! Returns the Average value of the indicated bin.
double SamplingHistogramPlot::getValue(double bin) const
{
	const long lBin = long ( bin / getBinSize());

	if (_getSamples(lBin) > 0)
	{
		return _getValue(lBin) / _getSamples(lBin);
	}

	return 0.0;
}

//! Sets the value in indicated bin to the value.
void SamplingHistogramPlot::setCumulativeValue(double bin, double value)
{
	_setValue(long(bin / getBinSize()), value);
}

//! Returns the accumulated value in the indicated bin.
double SamplingHistogramPlot::getCumulativeValue(double bin) const
{
	return _getValue(long(bin / getBinSize()));
}

//! Sets the value in indicated bin to the value.
void SamplingHistogramPlot::setSamples(double bin, long value)
{
	_setSamples(long(bin / getBinSize()), value);
}

//! Returns the accumulated value in the indicated bin.
long SamplingHistogramPlot::getSamples(double bin) const
{
	return _getSamples(long(bin / getBinSize()));
}

//! Sets the value in indicated bin to the value.
void SamplingHistogramPlot::setCumulativeValueSquared(double bin, double value)
{
	_setValueSquared(long(bin / getBinSize()), value);
}

//! Returns the accumulated value in the indicated bin.
double SamplingHistogramPlot::getCumulativeValueSquared(double bin) const
{
	return _getValueSquared(long(bin / getBinSize()));
}

//! Eliminates the values in indicated bin.
void SamplingHistogramPlot::dropBin(double bin)
{
	_dropBin(long(bin / getBinSize()));
}

QSharedPointer<OperatorPlotHandler> SamplingHistogramPlot::getNewHandler()
{
	//Histogram does not need to add any features to the base class yet
	return SamplingBarBase::getNewHandler();
}

}; //namespace Picto
