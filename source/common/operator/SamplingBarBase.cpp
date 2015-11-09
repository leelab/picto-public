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
	if (m_qhCumulValue.contains(currentSetName_) && m_qhCumulValue[currentSetName_].contains(bin))
	{
		return m_qhCumulValue[currentSetName_][bin] / double(m_qhCumulNum[currentSetName_][bin]);
	}

	return 0.0;
}

/*! \brief Calculates the standard error of each bin and pushes the value into the vector.
 *	\sa BarBase::replot
 */
void SamplingBarBase::_handleErrorValue(long bin, double &rdRangeMax, QVector<QwtIntervalSample> &qvError)
{
	if (m_bLastErrorBarState)
	{
		const double dBinWidth = 0.5 / (getBinSpacing() + 1.0);

		const double dAvg = m_qhCumulValue[currentSetName_][bin] / double(m_qhCumulNum[currentSetName_][bin]);
		const double dValue = m_bBinOffset ? (bin + dBinWidth) * getBinSize() : bin * getBinSize();
		const double stdErr = (sqrt(m_qhCumulValSq[currentSetName_][bin] - dAvg*m_qhCumulValue[currentSetName_][bin])) / double(m_qhCumulNum[currentSetName_][bin]);
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
	emit setErrorSamplesSig(currentSetName_, qvError);
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
	BarBase::reset();

	foreach(QString setName, m_qhCumulValue.keys())
	{
		m_qhCumulValSq[setName].clear();
		m_qhCumulNum[setName].clear();
	}
}

/*!	\brief Sets the value of the square of the indicated bin's submissions.
 */
void SamplingBarBase::_setValueSquared(long bin, double value)
{
	m_bDataChanged = true;
	m_qhDataChanged[currentSetName_] = true;

	if (!m_qhCumulValSq.contains(currentSetName_))
	{
		_createSet(currentSetName_);
	}

	if (!m_qhCumulValSq[currentSetName_].contains(bin))
	{
		_createBin(bin);
	}

	m_qhCumulValSq[currentSetName_][bin] = value;
}

/*!	\brief Returns the square of the values of the indicated bin's submissions.
 */
double SamplingBarBase::_getValueSquared(long bin) const
{
	if (m_qhCumulValSq.contains(currentSetName_) && m_qhCumulValSq[currentSetName_].contains(bin))
	{
		return m_qhCumulValSq[currentSetName_][bin];
	}

	return 0.0;
}

/*!	\brief Sets the number of samples taken in indicated bin to the value.
 */
void SamplingBarBase::_setSamples(long bin, long samples)
{
	m_bDataChanged = true;
	m_qhDataChanged[currentSetName_] = true;

	if (!m_qhCumulValue.contains(currentSetName_))
	{
		_createSet(currentSetName_);
	}

	if (!m_qhCumulValue[currentSetName_].contains(bin))
	{
		_createBin(bin);
	}

	m_qhCumulNum[currentSetName_][bin] = samples;
}

/*!	\brief Returns the number of Samples taken for the indicated bin.
 */
long SamplingBarBase::_getSamples(long bin) const
{
	if (m_qhCumulNum.contains(currentSetName_) && m_qhCumulNum[currentSetName_].contains(bin))
	{
		return m_qhCumulNum[currentSetName_][bin];
	}

	return 0L;
}

/*!	\brief Adds a value to the collected samples.
*/
void SamplingBarBase::_submitValue(long bin, double value)
{
	m_bDataChanged = true;
	m_qhDataChanged[currentSetName_] = true;

	if (!m_qhCumulValue.contains(currentSetName_))
	{
		_createSet(currentSetName_);
	}

	if (!m_qhCumulValue[currentSetName_].contains(bin))
	{
		_createBin(bin);
	}

	m_qhCumulValue[currentSetName_][bin] += value;
	m_qhCumulValSq[currentSetName_][bin] += value*value;
	++m_qhCumulNum[currentSetName_][bin];

}

/*!	Function to define behavior when a new bin needs to be created.
 */
void SamplingBarBase::_createBin(long bin)
{
	BarBase::_createBin(bin);

	m_qhCumulNum[currentSetName_][bin] = 0;
	m_qhCumulValSq[currentSetName_][bin] = 0;
}

/*! Function to define behavior when a new set needs to be created.
*/
void SamplingBarBase::_createSet(const QString &setName)
{
	BarBase::_createSet(setName);

	m_qhCumulNum[currentSetName_] = QHash<long, long>();
	m_qhCumulValue[currentSetName_] = QHash<long, double>();

}

/*!	\brief Virtual function to define behavior when a bin needs to be destroyed.
 */
void SamplingBarBase::_destroyBin(long bin)
{
	BarBase::_destroyBin(bin);

	m_qhCumulNum[currentSetName_].remove(bin);
	m_qhCumulValSq[currentSetName_].remove(bin);
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
