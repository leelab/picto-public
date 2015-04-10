#include "BarBase.h"

#include "../memleakdetect.h"

#include <qwt_text.h>
#include <qwt_plot.h>
#include <qwt_plot_histogram.h>
#include <qwt_legend.h>
#include <qwt_legend_label.h>
#include <qwt_scale_draw.h>
#include <qwt_plot_intervalcurve.h>

namespace Picto {

/*!	\brief This class is used to scale Normalized plots.
 */
class NormalizedScaleDraw : public QwtScaleDraw
{
public:
	/*!	\brief Constructor to set our properties.
	 */
	NormalizedScaleDraw(double max)
		:max_(max)
	{
		setTickLength(QwtScaleDiv::MajorTick, 6);
		setTickLength(QwtScaleDiv::MediumTick, 3);
		setTickLength(QwtScaleDiv::MinorTick, 0);

		setSpacing(15);
	}

	/*!	\brief Overwriting labeling functionality
	 */
	virtual QwtText label(double value) const
	{
		return QString("%1").arg(value / max_);
	}
private:
	//! \brief NormalizedScaleDraw holds onto the current total (Normalization quantity).
	double max_;
};

const QString BarBase::type = "Bar Base";

BarBase::BarBase()
	: m_pHistoPlotItem(nullptr)
{

	m_pHistoPlotItem = new QwtPlotHistogram("Data");
	m_pHistoPlotItem->setStyle(QwtPlotHistogram::Columns);
	m_pHistoPlotItem->setBrush(QBrush(Qt::red));
	m_pHistoPlotItem->attach(m_pPlot);

	AddDefinableProperty(QVariant::Bool, "DisplayLegend", false);
	AddDefinableProperty(QVariant::Bool, "NormalizedDisplay", false);
}

/*!	\brief Reconstructs elements of underlying plot.
*/
void BarBase::replot()
{
	QVector<QwtIntervalSample> qvSamples;
	QVector<QwtIntervalSample> qvError;

	const bool bNormalized = getNormalized();
	const double dBinSize = getBinSize();

	_handleErrorInitial(qvError);

	if (m_qhdCumulValue.isEmpty())
	{
		m_pHistoPlotItem->setSamples(new QwtIntervalSeriesData(qvSamples));

		if (bNormalized)
		{
			createNormalizedScale(1.0, 1.0);
		}

		m_pPlot->replot();
		return;
	}

	QList<long> keyList = m_qhdCumulValue.keys();
	std::sort(keyList.begin(), keyList.end());


	double total = 0.0;
	double dRangeMax = 0.0;
	
	foreach(long key, keyList)
	{
		const double dSample = _getSampleValue(key);
		const double dMin = (key - 0.5) * dBinSize;
		const double dMax = (key + 0.5) * dBinSize;
		qvSamples.push_back(std::move(QwtIntervalSample(dSample, dMin, dMax)));

		if (bNormalized)
		{
			if (dSample > dRangeMax)
			{
				dRangeMax = dSample;
			}

			total += dSample;
		}

		_handleErrorValue(key, dRangeMax, qvError);
	}

	m_pHistoPlotItem->setSamples(new QwtIntervalSeriesData(qvSamples));

	_handleErrorFinal(qvError);

	if (bNormalized)
	{
		createNormalizedScale(dRangeMax, total);
	}

	m_pPlot->replot();
}

double BarBase::_getSampleValue(long bin) const
{
	return m_qhdCumulValue[bin];
}

/*! \brief Uses the passed-in values to generate the ideal normalized plot range.
 *	\note Normalization in this case is really just a trick of the scale.
 */
void BarBase::createNormalizedScale(double dMaxValue, double dTotalValue)
{
	double dRangeMax = dMaxValue / dTotalValue;

	double dAxisMax = 1.0;
	double dTestRange = dAxisMax / 2.0;

	if (dRangeMax > 0.0)
	{
		while (dRangeMax <= dTestRange)
		{
			dAxisMax = dTestRange;
			dTestRange /= 2.0;
		}
	}

	//Change the y-axis labeling to give the appearance of normalization.
	QList<double> majorTicks;
	QList<double> mediumTicks;
	QList<double> minorTicks;

	int numOfDivs = 2;
	for (int i = 0; i < numOfDivs + 1; i++)
	{
		majorTicks += (i / (dAxisMax*numOfDivs))*dTotalValue;
	}

	numOfDivs = 10;
	for (int i = 0; i < numOfDivs + 1; i++)
	{
		mediumTicks += (i / (dAxisMax*numOfDivs))*dTotalValue;
	}


	QwtScaleDiv scaleDiv(0, dAxisMax*dTotalValue, minorTicks, mediumTicks, majorTicks);

	QwtScaleDraw *scaleDraw = new NormalizedScaleDraw(dTotalValue);

	m_pPlot->setAxisScaleDiv(QwtPlot::yLeft, scaleDiv);
	m_pPlot->setAxisScaleDraw(QwtPlot::yLeft, scaleDraw);
}


/*! \brief Registers plot with the Task object, so the task can pass on the information to various UI elements, and
 *	intercept/handle scripting language commands.
 */
void BarBase::postDeserialize()
{
	OperatorPlot::postDeserialize();
	
	if (propertyContainer_->getPropertyValue("DisplayLegend").toBool())
	{
		QwtLegend *legend = new QwtLegend;
		legend->setDefaultItemMode(QwtLegendData::ReadOnly);
		m_pPlot->insertLegend(legend, QwtPlot::RightLegend);
	}
	else
	{
		m_pPlot->insertLegend(nullptr);
	}
}

/*!	\brief Clears all values for this plot.
 */
void BarBase::reset()
{
	m_qhdCumulValue.clear();

	m_bDataChanged = true;
}

/*!	\brief Modifies the value in the indicated bin by the passed-in value.
 */
void BarBase::_adjustValue(long bin, double value)
{
	m_bDataChanged = true;

	if (!m_qhdCumulValue.contains(bin))
	{
		_createBin(bin);
	}

	m_qhdCumulValue[bin] += value;
}

/*!	\brief Sets the value in indicated bin to the value.
 */
void BarBase::_setValue(long bin, double value)
{
	m_bDataChanged = true;

	if (!m_qhdCumulValue.contains(bin))
	{
		_createBin(bin);
	}

	m_qhdCumulValue[bin] = value;
}

/*!	\brief Eliminates the values in indicated bin.
 */
void BarBase::_dropBin(long bin)
{
	if (m_qhdCumulValue.contains(bin))
	{
		m_bDataChanged = true;
		_destroyBin(bin);
	}
}

/*!	\brief Returns the value of the indicated bin.
 */
double BarBase::_getValue(long bin) const
{
	if (m_qhdCumulValue.contains(bin))
	{
		return m_qhdCumulValue[bin];
	}

	return 0.0;
}

/*!	\brief Virtual function to define behavior when a new bin needs to be created.
 */
void BarBase::_createBin(long bin)
{
	m_qhdCumulValue[bin] = 0;
}

/*!	\brief Virtual function to define behavior when a bin needs to be destroyed.
 */
void BarBase::_destroyBin(long bin)
{
	m_qhdCumulValue.remove(bin);
}

}; //namespace Picto