#include "SamplingBarBase.h"
#include "SamplingBarBasePlotHandler.h"

#include "../memleakdetect.h"

#include <qwt_samples.h>

namespace Picto {

const QString SamplingBarBase::type = "Sampling Bar Base";

SamplingBarBase::SamplingBarBase()
	: m_bLastErrorBarState(false)
{
	AddDefinableProperty(QVariant::Bool, "DisplayStdErr", false);
}

void SamplingBarBase::draw()
{
	if (m_bLastErrorBarState != _getDisplayErrBar())
	{
		m_bLastErrorBarState = !m_bLastErrorBarState;
		m_bDataChanged = true;
		emit setErrorBarsVisibleSig(m_bLastErrorBarState);
	}

	BarBase::draw();
}

//! Returns the the average of submitted samples for the indicated bin.
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
	emit setErrorSamplesSig(qvError);
}


/*! \brief Registers plot with the Task object, so the task can pass on the information to various UI elements, and
 *	intercept/handle scripting language commands.
 */
void SamplingBarBase::postDeserialize()
{
	BarBase::postDeserialize();

	setPropertyRuntimeEditable("DisplayStdErr");
	m_bLastErrorBarState = _getDisplayErrBar();
	propertyContainer_->getProperty("DisplayStdErr")->enableInitRunValueSync(true);
}

void SamplingBarBase::initView()
{
	BarBase::initView();

	emit initializeSamplingSig();
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

	m_bBinsModified = true;
}

/*!	\brief Virtual function to define behavior when a bin needs to be destroyed.
 */
void SamplingBarBase::_destroyBin(long bin)
{
	m_qhdCumulValue.remove(bin);
	m_qhlCumulNum.remove(bin);
	m_qhdCumulValSq.remove(bin);

	m_bBinsModified = true;
}

QSharedPointer<OperatorPlotHandler> SamplingBarBase::getNewHandler()
{
	return QSharedPointer<OperatorPlotHandler>(new SamplingBarBasePlotHandler());
}

void SamplingBarBase::connectDataSignals(QSharedPointer<OperatorPlotHandler> plotHandler)
{
	BarBase::connectDataSignals(plotHandler);

	QSharedPointer<SamplingBarBasePlotHandler> barPlotHandler = plotHandler.objectCast<SamplingBarBasePlotHandler>();

	connect(this, SIGNAL(initializeSamplingSig()),
		barPlotHandler.data(), SLOT(initializeSampling()));

}

}; //namespace Picto
