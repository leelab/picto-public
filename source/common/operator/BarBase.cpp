#include <QThread>

#include "BarBase.h"
#include "BarBasePlotHandler.h"

#include "../memleakdetect.h"

#include <qwt_samples.h>

namespace Picto {

const QString BarBase::type = "Bar Base";

BarBase::BarBase()
	: m_bBinsModified(false), m_bUpdateBrush(false)
{
	AddDefinableProperty(QVariant::Bool, "DisplayLegend", false);
	AddDefinableProperty(QVariant::Bool, "NormalizedDisplay", false);

	//Indexed by enum ColumnType
	columnTypes_ << "Outline" << "Flat Columns" << "Raised Columns" << "Lines";
	AddDefinableProperty(PropertyContainer::enumTypeId(), "ColumnType", 1, "enumNames", columnTypes_);
	AddDefinableProperty(QVariant::Color, "ColumnColor", QColor(Qt::red));
	AddDefinableProperty(QVariant::Color, "CanvasColor", QColor(Qt::lightGray));
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
		emit setSamplesSig(qvSamples);
		_handleErrorFinal(qvError);

		if (bNormalized)
		{
			createNormalizedScale(1.0, 1.0);
		}

		emit callReplot();
		return;
	}

	QList<long> keyList = m_qhdCumulValue.keys();
	std::sort(keyList.begin(), keyList.end());

	double total = 0.0;
	double dRangeMax = 0.0;
	
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

	emit setSamplesSig(qvSamples);

	_handleErrorFinal(qvError);

	if (bNormalized)
	{
		createNormalizedScale(dRangeMax, total);
	}

	if (m_bBinsModified)
	{
		m_bBinsModified = false;
		emit scaleAxisSig(dBinSize);
		emit handleXLabelsSig(*keyList.begin(), *(keyList.end() - 1));
	}

	if (m_bUpdateBrush)
	{
		m_bUpdateBrush = false;
		emit updateColumnsSig(getColor(), getColumnType());
	}

	emit callReplot();
}


void BarBase::draw()
{
	//Force a replot if you need an update
	if (m_bUpdateBrush || m_bBinsModified )
	{
		m_bDataChanged = true;
	}

	//Call parent draw method.
	OperatorPlot::draw();
}

//! Returns the value of the passed-in bin.
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
		majorTicks += (i / double(numOfDivs))*dTotalValue*dAxisMax;
	}

	numOfDivs = 10;
	for (int i = 0; i < numOfDivs + 1; i++)
	{
		mediumTicks += (i / double(numOfDivs))*dTotalValue*dAxisMax;
	}

	emit normalizeScaleSig(dAxisMax, dTotalValue, mediumTicks, majorTicks);
}

void BarBase::initView()
{
	OperatorPlot::initView();

	emit initializeHistoSig(
		propertyContainer_->getPropertyValue("DisplayLegend").toBool(),
		getColor(),
		propertyContainer_->getPropertyValue("CanvasColor").value<QColor>(),
		getColumnType());
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

	m_bBinsModified = true;
}

/*!	\brief Virtual function to define behavior when a bin needs to be destroyed.
 */
void BarBase::_destroyBin(long bin)
{
	m_qhdCumulValue.remove(bin);

	m_bBinsModified = true;
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


QSharedPointer<BarBasePlotHandler> BarBase::getBarBasePlotHandler()
{
	return m_pPlotHandler.objectCast<BarBasePlotHandler>();
}

void BarBase::connectDataSignals(QSharedPointer<OperatorPlotHandler> plotHandler)
{
	OperatorPlot::connectDataSignals(plotHandler);

	QSharedPointer<BarBasePlotHandler> barPlotHandler = plotHandler.objectCast<BarBasePlotHandler>();

	qRegisterMetaType<QList<double>>("QList<double>");
	qRegisterMetaType<QVector<QwtIntervalSample>>("QVector<QwtIntervalSample>");
	qRegisterMetaType<QList<QwtLegendData>>("QList<QwtLegendData>");

	connect(this, SIGNAL(initializeHistoSig(bool, const QColor&, const QColor&, int)),
		barPlotHandler.data(), SLOT(initializeHisto(bool, const QColor&, const QColor&, int)));
	connect(this, SIGNAL(handleXLabelsSig(long,long)),
		barPlotHandler.data(), SLOT(handleXLabels(long,long)));
	connect(this, SIGNAL(updateColumnsSig(const QColor&, ColumnType::ColumnType)),
		barPlotHandler.data(), SLOT(updateColumns(const QColor&, ColumnType::ColumnType)));
	connect(this, SIGNAL(normalizeScaleSig(double, double, const QList<double>&, const QList<double>&)),
		barPlotHandler.data(), SLOT(normalizeScale(double, double, const QList<double>&, const QList<double>&)));
	connect(this, SIGNAL(setSamplesSig(const QVector<QwtIntervalSample>&)),
		barPlotHandler.data(), SLOT(setSamples(const QVector<QwtIntervalSample>&)));
	connect(this, SIGNAL(setErrorSamplesSig(const QVector<QwtIntervalSample>&)),
		barPlotHandler.data(), SLOT(setErrorSamples(const QVector<QwtIntervalSample>&)));
	connect(this, SIGNAL(setErrorBarsVisibleSig(bool)),
		barPlotHandler.data(), SLOT(setErrorBarsVisible(bool)));
	connect(this, SIGNAL(scaleAxisSig(double)),
		barPlotHandler.data(), SLOT(scaleAxis(double)));
	connect(this, SIGNAL(callReplot()),
		barPlotHandler.data(), SLOT(callReplot()));
}


}; //namespace Picto
