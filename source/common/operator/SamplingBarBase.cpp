#include "SamplingBarBase.h"

#include "../memleakdetect.h"

#include <qwt_text.h>
#include <qwt_plot.h>
#include <qwt_plot_item.h>
#include <qwt_plot_histogram.h>
#include <qwt_plot_intervalcurve.h>
#include <qwt_interval_symbol.h>

namespace Picto {

const QString SamplingBarBase::type = "Sampling Bar Base";

SamplingBarBase::SamplingBarBase()
	: m_pErrorBars(nullptr), m_bLastErrorBarState(false)
{
	QwtIntervalSymbol *errorBar = new QwtIntervalSymbol(QwtIntervalSymbol::Bar);
	errorBar->setWidth(8);
	errorBar->setPen(QColor(Qt::blue));
	
	m_pErrorBars = new QwtPlotIntervalCurve("Standard Error");
	m_pErrorBars->setStyle(QwtPlotIntervalCurve::NoCurve);
	m_pErrorBars->setSymbol(errorBar);
	m_pErrorBars->setRenderHint(QwtPlotItem::RenderAntialiased, false);
	m_pErrorBars->setPen(Qt::white);
	m_pErrorBars->setBrush(QBrush(QColor(Qt::blue)));

	m_pErrorBars->attach(m_pPlot);
	m_pErrorBars->setZ(m_pHistoPlotItem->z() + 1);


	AddDefinableProperty(QVariant::Bool, "DisplayStdErr", false);
}

void SamplingBarBase::draw()
{
	if (m_bLastErrorBarState != _getDisplayErr())
	{
		m_bLastErrorBarState = !m_bLastErrorBarState;
		m_bDataChanged = true;
		m_pErrorBars->setVisible(m_bLastErrorBarState);
	}

	BarBase::draw();
}

double SamplingBarBase::_getSampleValue(long bin) const
{
	return m_qhdCumulValue[bin] / double(m_qhlCumulNum[bin]);
}

/*! \brief Calculates the standard error of each bin and pushes the value into the vector.
 *	\sa BarBase::replot
 */
void SamplingBarBase::_handleErrorValue(long bin, double &rdRangeMax, QVector<QwtIntervalSample> &qvError)
{
	if (m_bLastErrorBarState)
	{
		const double dAvg = m_qhdCumulValue[bin] / double(m_qhlCumulNum[bin]);
		const double dValue = bin * getBinSize();
		const double stdErr = (sqrt(m_qhdCumulValSq[bin] - dAvg*m_qhdCumulValue[bin])) / double(m_qhlCumulNum[bin]);
		const double dMax = dAvg + stdErr;

		qvError.push_back(std::move(QwtIntervalSample(dValue, dAvg - stdErr, dMax)));

		if (getNormalized())
		{
			if (dMax > rdRangeMax)
			{
				rdRangeMax = dMax;
			}
		}
	}
}

/*! \brief Passes the accumulated QwtIntervalSample to the error bars.
 *	\note This Vector is empty if m_bLastErrorBarState was false.
 *	\sa BarBase::replot, SamplingBarBase::_handleErrorValue
 */
void SamplingBarBase::_handleErrorFinal(QVector<QwtIntervalSample> &qvError)
{
	m_pErrorBars->setSamples(new QwtIntervalSeriesData(qvError));
}


/*! \brief Registers plot with the Task object, so the task can pass on the information to various UI elements, and
 *	intercept/handle scripting language commands.
 */
void SamplingBarBase::postDeserialize()
{
	BarBase::postDeserialize();

	setPropertyRuntimeEditable("DisplayStdErr");
	m_bLastErrorBarState = _getDisplayErr();
	propertyContainer_->getProperty("DisplayStdErr")->enableInitRunValueSync(true);
}

/*!	\brief Clears all values for this plot.
 */
void SamplingBarBase::reset()
{
	m_qhdCumulValue.clear();
	m_qhdCumulValSq.clear();
	m_qhlCumulNum.clear();

	m_bDataChanged = true;
}

/*!	\brief Sets the value of the square of the indicated bin's submissions.
 */
void SamplingBarBase::_setValueSquared(long bin, double value)
{
	m_bDataChanged = true;

	if (!m_qhdCumulValSq.contains(bin))
	{
		_createBin(bin);
	}

	m_qhdCumulValSq[bin] = value;
}

/*!	\brief Returns the square of the values of the indicated bin's submissions.
 */
double SamplingBarBase::_getValueSquared(long bin) const
{
	if (m_qhdCumulValSq.contains(bin))
	{
		return m_qhdCumulValSq[bin];
	}

	return 0.0;
}

/*!	\brief Sets the number of samples taken in indicated bin to the value.
 */
void SamplingBarBase::_setSamples(long bin, long samples)
{
	m_bDataChanged = true;

	if (!m_qhlCumulNum.contains(bin))
	{
		_createBin(bin);
	}

	m_qhlCumulNum[bin] = samples;
}

/*!	\brief Returns the number of Samples taken for the indicated bin.
 */
long SamplingBarBase::_getSamples(long bin) const
{
	if (m_qhlCumulNum.contains(bin))
	{
		return m_qhlCumulNum[bin];
	}

	return 0L;
}

/*!	\brief Adds a value to the collected samples.
*/
void SamplingBarBase::_submitValue(long bin, double value)
{
	m_bDataChanged = true;

	if (!m_qhdCumulValue.contains(bin))
	{
		_createBin(bin);
	}

	m_qhdCumulValue[bin] += value;
	m_qhdCumulValSq[bin] += value*value;
	++m_qhlCumulNum[bin];

}

/*!	\brief Virtual function to define behavior when a new bin needs to be created.
 */
void SamplingBarBase::_createBin(long bin)
{
	m_qhlCumulNum[bin] = 0;
	m_qhdCumulValue[bin] = 0;
	m_qhdCumulValSq[bin] = 0;
}

/*!	\brief Virtual function to define behavior when a bin needs to be destroyed.
 */
void SamplingBarBase::_destroyBin(long bin)
{
	m_qhdCumulValue.remove(bin);
	m_qhlCumulNum.remove(bin);
	m_qhdCumulValSq.remove(bin);
}

}; //namespace Picto
