#include <QThread>

#include "BarBase.h"
#include "BarBasePlotHandler.h"

#include "../memleakdetect.h"

#include <qwt_samples.h>

namespace Picto {

const QString BarBase::type = "Bar Base";

BarBase::BarBase()
	: currentSetName_("Default"), m_lCachedXMin(LONG_MAX), m_lCachedXMax(LONG_MIN), m_bTotalBinRecalc(false), m_cUnasignedNum(0)
{
	AddDefinableProperty(QVariant::Bool, "DisplayLegend", false);
	AddDefinableProperty(QVariant::Bool, "NormalizedDisplay", false);

	//Indexed by enum ColumnType
	columnTypes_ << "Outline" << "Flat Columns" << "Raised Columns" << "Lines" << "Curve" << "Filled Curve";
	AddDefinableProperty(PropertyContainer::enumTypeId(), "ColumnType", 1, "enumNames", columnTypes_);
	AddDefinableProperty(QVariant::Color, "ColumnColor", QColor(Qt::red));
	AddDefinableProperty(QVariant::Color, "CanvasColor", QColor(Qt::lightGray));
}

/*!	\brief Reconstructs elements of underlying plot.
 */
void BarBase::replot()
{
	bool bBinsModified = false;
	foreach(QString setName, m_qhCumulValue.keys())
	{
		if (m_qhBinsModified[setName])
		{
			bBinsModified = true;
		}

		if (m_qhDataChanged[setName] || m_bTotalBinRecalc)
		{
			replot(setName);
			m_qhDataChanged[setName] = false;
		}
	}

	m_bTotalBinRecalc = false;

	if (bBinsModified)
	{
		emit scaleAxisSig(getBinSize());
		emit handleXLabelsSig(m_lCachedXMin, m_lCachedXMax);
	}
}

//! Replot the data within individual datasets
void BarBase::replot(const QString &setName)
{
	const QString oldCurrentSet = currentSetName_;
	currentSetName_ = setName;

	QVector<QwtIntervalSample> qvSamples;
	QVector<QPointF> qvPoints;
	QVector<QwtIntervalSample> qvError;

	const bool bNormalized = getNormalized();
	const double dBinSize = getBinSize();

	_handleErrorInitial(qvError);

	if (m_qhCumulValue[setName].isEmpty())
	{
		//Branch point accumulating based on ColumnType
		if (getColumnType() == ColumnType::COLUMN_CURVE ||
			getColumnType() == ColumnType::COLUMN_FILLED_CURVE)
		{
			emit setPointsSig(setName, qvPoints);
		}
		else
		{
			emit setSamplesSig(setName, qvSamples);
		}

		_handleErrorFinal(qvError);

		if (bNormalized)
		{
			createNormalizedScale(1.0, 1.0);
		}

		emit callReplot(setName);
		currentSetName_ = oldCurrentSet;
		return;
	}

	QList<long> keyList = m_qhCumulValue[setName].keys();
	std::sort(keyList.begin(), keyList.end());

	double total = 0.0;
	double dRangeMax = 0.0;

	//Branch point accumulating based on ColumnType
	if (getColumnType() == ColumnType::COLUMN_CURVE ||
		getColumnType() == ColumnType::COLUMN_FILLED_CURVE)
	{
		foreach(long key, keyList)
		{
			const double dSample = _getSampleValue(key);
			qvPoints.push_back(std::move(QPointF(key*dBinSize, dSample)));

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

		emit setPointsSig(setName, qvPoints);
	}
	else
	{
		foreach(long key, keyList)
		{
			const double dSample = _getSampleValue(key);
			const double dBinWidth = 0.5 / (getBinSpacing() + 1.0);
			const double dMin = (key - dBinWidth) * dBinSize;
			const double dMax = (key + dBinWidth) * dBinSize;
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

		emit setSamplesSig(setName, qvSamples);
	}


	_handleErrorFinal(qvError);

	if (bNormalized)
	{
		createNormalizedScale(dRangeMax, total);
	}

	//Stretch out the bins if necessary
	if (m_qhBinsModified[setName] || m_bTotalBinRecalc)
	{
		m_qhBinsModified[setName] = false;
		if (*keyList.begin() < m_lCachedXMin)
		{
			m_lCachedXMin = *keyList.begin();
		}

		if (*(keyList.end() - 1) > m_lCachedXMax)
		{
			m_lCachedXMax = *(keyList.end() - 1);
		}
	}

	if (m_qhUpdateBrush[setName])
	{
		m_qhUpdateBrush[setName] = false;
		emit updateColumnsSig(setName, getColor());
	}

	emit callReplot(setName);
	currentSetName_ = oldCurrentSet;
}

//! Sets the color of the BarBase.
void BarBase::setColor(QColor color)
{
	//Set the color to the property so it is used as the default for new datasets.
	propertyContainer_->setPropertyValue("ColumnColor", color);
	//CumulValue is the best Hash to use to find out if the dataset has been created yet.
	if (!m_qhCumulValue.contains(currentSetName_))
	{
		_createSet(currentSetName_);
	}

	m_qhColor[currentSetName_] = color;
	m_qhUpdateBrush[currentSetName_] = true;
}

//! Gets the color of the BarBase.
const QColor BarBase::getColor() const
{
	// Return the current set's color if it has been declared
	if (m_qhColor.contains(currentSetName_))
	{
		return m_qhColor[currentSetName_];
	}

	//Otherwise use the property value
	return propertyContainer_->getPropertyValue("ColumnColor").value<QColor>();
}

void BarBase::draw()
{
	//Force a replot if you need an update
	foreach(QString setName, m_qhCumulValue.keys())
	{
		if (m_qhUpdateBrush[setName] || m_qhBinsModified[setName])
		{
			m_bDataChanged = true;
			m_qhDataChanged[setName] = true;
		}
	}

	//Call parent draw method.
	OperatorPlot::draw();
}

//! Returns the value of the passed-in bin.
double BarBase::_getSampleValue(long bin) const
{
	if (m_qhCumulValue.contains(currentSetName_) && m_qhCumulValue[currentSetName_].contains(bin))
	{
		return m_qhCumulValue[currentSetName_][bin];
	}

	return 0.0;
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
		majorTicks += (i / double(numOfDivs))*dTotalValue*dAxisMax;
	}

	numOfDivs = 10;
	for (int i = 0; i < numOfDivs + 1; i++)
	{
		mediumTicks += (i / double(numOfDivs))*dTotalValue*dAxisMax;
	}

	//emit normalizeScaleSig(currentSetName_, dAxisMax, dTotalValue, mediumTicks, majorTicks);
}

void BarBase::initView()
{
	OperatorPlot::initView();

	emit initializeHistoSig(
		propertyContainer_->getPropertyValue("DisplayLegend").toBool(),
		getColor(),
		propertyContainer_->getPropertyValue("CanvasColor").value<QColor>(),
		getColumnType());

	m_qhColor["Default"] = getColor();
}


/*!	\brief Clears all values for this plot.
 */
void BarBase::reset()
{
	currentSetName_ = "Default";

	m_lCachedXMin = LONG_MAX;
	m_lCachedXMax = LONG_MIN;

	m_bDataChanged = true;
	m_bTotalBinRecalc = true;

	foreach(QString dataset, m_qhDataChanged.keys())
	{
		m_qhCumulValue[dataset].clear();
		m_qhDataChanged[dataset] = true;
	}

	emit resetSig();
}

/*!	\brief Modifies the value in the indicated bin by the passed-in value.
 */
void BarBase::_adjustValue(long bin, double value)
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
}

/*!	\brief Sets the value in indicated bin to the value.
 */
void BarBase::_setValue(long bin, double value)
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

	m_qhCumulValue[currentSetName_][bin] = value;
}

/*!	\brief Eliminates the values in indicated bin.
 */
void BarBase::_dropBin(long bin)
{
	if (m_qhCumulValue.contains(currentSetName_) && m_qhCumulValue[currentSetName_].contains(bin))
	{
		m_bDataChanged = true;
		m_qhDataChanged[currentSetName_] = true;
		_destroyBin(bin);
	}
}

/*!	\brief Returns the value of the indicated bin.
 */
double BarBase::_getValue(long bin) const
{
	if (m_qhCumulValue.contains(currentSetName_) && m_qhCumulValue[currentSetName_].contains(bin))
	{
		return m_qhCumulValue[currentSetName_][bin];
	}

	return 0.0;
}

/*!	\brief Virtual function to define behavior when a new bin needs to be created.
 */
void BarBase::_createBin(long bin)
{
	m_qhCumulValue[currentSetName_][bin] = 0;
	m_qhBinsModified[currentSetName_] = true;
}

/*!	\brief Virtual function to define behavior when a new set needs to be created.
*/
void BarBase::_createSet(const QString &setName)
{
	m_qhCumulValue[setName] = QHash<long, double>();
	m_qhBinsModified[setName] = true;

	if (!m_qhColor.contains(setName))
	{
		char myColorNum = m_cUnasignedNum++;
		// Scramble up the digits in a periodic way
		myColorNum = ((myColorNum & 0x0F) << 4) | ((myColorNum & 0xF0) >> 4);
		myColorNum = ((myColorNum & 0x33) << 2) | ((myColorNum & 0xCC) >> 2);
		myColorNum = ((myColorNum & 0x55) << 1) | ((myColorNum & 0xAA) >> 1);

		int red = char(sin(0.0245 * myColorNum) * 127) + 128;
		int green = char(sin(0.0245 * myColorNum + 2) * 127) + 128;
		int blue = char(sin(0.0245 * myColorNum + 4) * 127) + 128;
		int alpha = 255;

		switch (getColumnType())
		{
		case ColumnType::COLUMN_FLAT_COLUMN:
		case ColumnType::COLUMN_RAISED_COLUMN:
		case ColumnType::COLUMN_FILLED_CURVE:
			alpha = 160;
			break;
		case ColumnType::COLUMN_OUTLINE:
		case ColumnType::COLUMN_LINES:
		case ColumnType::COLUMN_CURVE:
			alpha = 255;
			break;
		default:
			break;
		}

		m_qhColor[setName] = QColor(red, green, blue, alpha);
	}
	m_qhUpdateBrush[currentSetName_] = true;

}

/*!	\brief Virtual function to define behavior when a bin needs to be destroyed.
 */
void BarBase::_destroyBin(long bin)
{
	m_qhCumulValue[currentSetName_].remove(bin);
	m_qhBinsModified[currentSetName_] = true;
	if (bin == m_lCachedXMin || bin == m_lCachedXMax)
	{
		m_bTotalBinRecalc = true;
	}
}

//! Returns the current ColumnType parameter
ColumnType::ColumnType BarBase::getColumnType() const
{
	return (ColumnType::ColumnType)propertyContainer_->getPropertyValue("ColumnType").toInt();
}

QSharedPointer<OperatorPlotHandler> BarBase::getNewHandler()
{
	return QSharedPointer<OperatorPlotHandler>(new BarBasePlotHandler());
}

void BarBase::connectDataSignals(QSharedPointer<OperatorPlotHandler> plotHandler)
{
	OperatorPlot::connectDataSignals(plotHandler);

	QSharedPointer<BarBasePlotHandler> barPlotHandler = plotHandler.objectCast<BarBasePlotHandler>();

	connect(this, SIGNAL(initializeHistoSig(bool, const QColor&, const QColor&, int)),
		barPlotHandler.data(), SLOT(initializeHisto(bool, const QColor&, const QColor&, int)));
	connect(this, SIGNAL(handleXLabelsSig(long,long)),
		barPlotHandler.data(), SLOT(handleXLabels(long,long)));
	connect(this, SIGNAL(updateColumnsSig(const QString &, const QColor&)),
		barPlotHandler.data(), SLOT(updateProperties(const QString &, const QColor&)));
	connect(this, SIGNAL(normalizeScaleSig(const QString &, double, double, const QList<double>&, const QList<double>&)),
		barPlotHandler.data(), SLOT(normalizeScale(const QString &, double, double, const QList<double>&, const QList<double>&)));
	connect(this, SIGNAL(setSamplesSig(const QString &, const QVector<QwtIntervalSample>&)),
		barPlotHandler.data(), SLOT(setSamples(const QString &, const QVector<QwtIntervalSample>&)));
	connect(this, SIGNAL(setPointsSig(const QString &, const QVector<QPointF>&)),
		barPlotHandler.data(), SLOT(setPoints(const QString &, const QVector<QPointF>&)));
	connect(this, SIGNAL(setErrorSamplesSig(const QString &, const QVector<QwtIntervalSample>&)),
		barPlotHandler.data(), SLOT(setErrorSamples(const QString &, const QVector<QwtIntervalSample>&)));
	connect(this, SIGNAL(setErrorBarsVisibleSig(bool)),
		barPlotHandler.data(), SLOT(setErrorBarsVisible(bool)));
	connect(this, SIGNAL(scaleAxisSig(double)),
		barPlotHandler.data(), SLOT(scaleAxis(double)));
	connect(this, SIGNAL(callReplot(const QString &)),
		barPlotHandler.data(), SLOT(callReplot(const QString &)));
	connect(this, SIGNAL(resetSig()),
		barPlotHandler.data(), SLOT(resetSlot()));
}


}; //namespace Picto
