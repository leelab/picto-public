#include <QThread>

#include "RasterBase.h"
#include "RasterBasePlotHandler.h"

#include "../memleakdetect.h"

#include <qwt_samples.h>

namespace Picto {

	const QString RasterBase::type = "Bar Base";

	RasterBase::RasterBase()
		: currentSetName_("Default"), m_lCachedXMin(LONG_MAX), m_lCachedXMax(LONG_MIN), m_bTotalBinRecalc(false),
		m_cUnasignedNum(0), m_bBinOffset(false)
	{
		AddDefinableProperty(QVariant::Bool, "DisplayLegend", false);
		AddDefinableProperty(QVariant::Bool, "NormalizedDisplay", false);

		//Indexed by enum ColumnType
		columnTypes_ << "Outline" << "Flat Columns" << "Raised Columns" << "Lines" << "Curve" << "Filled Curve" << "Sticks";
		AddDefinableProperty(PropertyContainer::enumTypeId(), "ColumnType", 1, "enumNames", columnTypes_);
		AddDefinableProperty(QVariant::Color, "ColumnColor", QColor(Qt::red));
		AddDefinableProperty(QVariant::Color, "CanvasColor", QColor(Qt::lightGray));
	}

	/*!	\brief Reconstructs elements of underlying plot.
	*/
	void RasterBase::replot()
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
			//emit scaleAxisSig(0.05);
			//emit handleXLabelsSig(m_lCachedXMin, m_lCachedXMax);
		}
	}


	//! Replot the data within individual datasets
	void RasterBase::replot(const QString &setName)
	{
		const QString oldCurrentSet = currentSetName_;
		currentSetName_ = setName;
		
		QVector<QPointF> qvPoints;
		QVector<QwtIntervalSample> qvError;

		//const bool bNormalized = getNormalized();
		//const double dBinSize = getBinSize();
		//const double dBinSize = 0.05;
		//const double dBinWidth = 0.5 / (0.01 + 1.0);

		QVector<QPointF> qvMarkerPoints;

		if (m_qhCumulValue[setName].isEmpty())
		{	
			emit setPointsSig(setName, qvPoints);
		
			//markers			
			if (m_pPlotHandler.objectCast<RasterBasePlotHandler>())
				(m_pPlotHandler.objectCast<RasterBasePlotHandler>())->addMarkers(setName, qvMarkerPoints);

			/*if (bNormalized)
			{
				createNormalizedScale(1.0, 1.0);
			}*/

			emit callReplot(setName);
			currentSetName_ = oldCurrentSet;
			return;
		}

		QList<double> keyList = m_qhCumulValue[setName].keys();
		std::sort(keyList.begin(), keyList.end());

		//double total = 0.0;
		//double dRangeMax = 0.0;

	
		foreach(double key, keyList)
		{
			const double dSample = _getSampleValue(key);
			//const double dValue1 = m_bBinOffset ? (key + dBinWidth) * dBinSize : key * dBinSize;
			const double dValue = key;
			qvPoints.push_back(std::move(QPointF(dValue, dSample)));

			/*if (bNormalized)
			{
				if (dSample > dRangeMax)
				{
					dRangeMax = dSample;
				}

				total += dSample;
			}*/
		}

		//emit setPointsSig(setName, qvPoints);
		if (m_pPlotHandler.objectCast<RasterBasePlotHandler>())
			(m_pPlotHandler.objectCast<RasterBasePlotHandler>())->setPoints(setName, qvPoints);

		//add markers
		if (!m_qMarkers[setName].isEmpty())
		{
			QList<double> keyMarkersList = m_qMarkers[setName].keys();
			std::sort(keyMarkersList.begin(), keyMarkersList.end());

			foreach(double time, keyMarkersList)
			{
				const int dLabel = m_qMarkers[setName][time];
				const double dTime = time;
				qvMarkerPoints.push_back(std::move(QPointF(dTime, dLabel)));
			}
			if (m_pPlotHandler.objectCast<RasterBasePlotHandler>())
				(m_pPlotHandler.objectCast<RasterBasePlotHandler>())->addMarkers(setName, qvMarkerPoints);
		}

	
		/*if (bNormalized)
		{
			createNormalizedScale(dRangeMax, total);
		}*/

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

	//! Sets the color of the RasterBase.
	void RasterBase::setColor(QColor color)
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

	//! Gets the color of the RasterBase.
	const QColor RasterBase::getColor() const
	{
		// Return the current set's color if it has been declared
		if (m_qhColor.contains(currentSetName_))
		{
			return m_qhColor[currentSetName_];
		}

		//Otherwise use the property value
		return propertyContainer_->getPropertyValue("ColumnColor").value<QColor>();
	}

	void RasterBase::draw()
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
	double RasterBase::_getSampleValue(double bin) const
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
	void RasterBase::createNormalizedScale(double dMaxValue, double dTotalValue)
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
	}

	void RasterBase::initView()
	{
		OperatorPlot::initView();

		emit initializePlotSig(
			propertyContainer_->getPropertyValue("DisplayLegend").toBool(),
			getColor(),
			propertyContainer_->getPropertyValue("CanvasColor").value<QColor>());

		m_qhColor["Default"] = getColor();
	}


	/*!	\brief Clears all values for this plot.
	*/
	void RasterBase::reset()
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
		foreach(QString dataset, m_qMarkers.keys())
		{
			m_qMarkers[dataset].clear();
		}
		emit resetSig();
	}

	
	/*!	\brief Sets the value in indicated bin to the value.
	*/
	void RasterBase::_setValue(double bin, double value)
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


	/*!	\brief Virtual function to define behavior when a new bin needs to be created.
	*/
	void RasterBase::_createBin(double bin)
	{
		m_qhCumulValue[currentSetName_][bin] = 0;
		m_qhBinsModified[currentSetName_] = true;
	}

	/*!	\brief Virtual function to define behavior when a new set needs to be created.
	*/
	void RasterBase::_createSet(const QString &setName)
	{
		m_qhCumulValue[setName] = QHash<double, double>();
		m_qhBinsModified[setName] = true;

		m_qMarkers[setName] = QHash<double, int>();

		if (!m_qhColor.contains(setName))
		{
			/*
			char myColorNum = m_cUnasignedNum++;
			// Scramble up the digits in a periodic way
			myColorNum = ((myColorNum & 0x0F) << 4) | ((myColorNum & 0xF0) >> 4);
			myColorNum = ((myColorNum & 0x33) << 2) | ((myColorNum & 0xCC) >> 2);
			myColorNum = ((myColorNum & 0x55) << 1) | ((myColorNum & 0xAA) >> 1);

			int red = char(sin(0.0245 * myColorNum) * 127) + 128;
			int green = char(sin(0.0245 * myColorNum + 2) * 127) + 128;
			int blue = char(sin(0.0245 * myColorNum + 4) * 127) + 128;
			int alpha = 255;

			m_qhColor[setName] = QColor(red, green, blue, alpha);
			*/

			m_qhColor[setName] = QColor(0,0,0);
		}
		m_qhUpdateBrush[currentSetName_] = true;

	}


	QSharedPointer<OperatorPlotHandler> RasterBase::getNewHandler()
	{
		return QSharedPointer<OperatorPlotHandler>(new RasterBasePlotHandler());
	}

	void RasterBase::connectDataSignals(QSharedPointer<OperatorPlotHandler> plotHandler)
	{
		OperatorPlot::connectDataSignals(plotHandler);

		QSharedPointer<RasterBasePlotHandler> barPlotHandler = plotHandler.objectCast<RasterBasePlotHandler>();

		connect(this, SIGNAL(initializePlotSig(bool, const QColor&, const QColor&)),
			barPlotHandler.data(), SLOT(initializePlot(bool, const QColor&, const QColor&)));
		connect(this, SIGNAL(handleXLabelsSig(long, long)),
			barPlotHandler.data(), SLOT(handleXLabels(long, long)));
		connect(this, SIGNAL(updateColumnsSig(const QString &, const QColor&)),
			barPlotHandler.data(), SLOT(updateProperties(const QString &, const QColor&)));		
		connect(this, SIGNAL(setPointsSig(const QString &, const QVector<QPointF>&)),
			barPlotHandler.data(), SLOT(setPoints(const QString &, const QVector<QPointF>&)));		
		connect(this, SIGNAL(scaleAxisSig(double)),
			barPlotHandler.data(), SLOT(scaleAxis(double)));
		connect(this, SIGNAL(callReplot(const QString &)),
			barPlotHandler.data(), SLOT(callReplot(const QString &)));
		connect(this, SIGNAL(resetSig()),
			barPlotHandler.data(), SLOT(resetSlot()));
	}
	/*!	\brief Sets the value for indicated marker
	*/
	void RasterBase::_addMarker(int eventID, double time)
	{
		if (!m_qMarkers.contains(currentSetName_))
		{
			m_qMarkers[currentSetName_] = QHash<double, int>();
		}

		if (!m_qMarkers[currentSetName_].contains(time))
		{
			m_qMarkers[currentSetName_][time] = 0;
		}

		m_qMarkers[currentSetName_][time] = eventID;

	}
}; //namespace Picto
