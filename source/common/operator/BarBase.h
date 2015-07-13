#ifndef _BAR_BASE_H_
#define _BAR_BASE_H_

#include "../common.h"
#include "OperatorPlot.h"

#include <QHash>

class QwtIntervalSample;

namespace Picto {

class BarBasePlotHandler;

/*!	\brief A plot populated in realtime during experiments or analysis.
 *
 *	The plot is meant for display in the Operator View.
 *	\author Trevor Stavropoulos, Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2015
 */
#if defined WIN32 || defined WINCE
class PICTOLIB_API BarBase : public OperatorPlot
#else
class BarBase : public OperatorPlot
#endif
{
	Q_OBJECT
	/*! \brief Sets/gets the red component of this BarBase's color.*/
	Q_PROPERTY(int red READ getRed WRITE setRed)
	/*! \brief Sets/gets the green component of this BarBase's color.*/
	Q_PROPERTY(int green READ getGreen WRITE setGreen)
	/*! \brief Sets/gets the blue component of this BarBase's color.*/
	Q_PROPERTY(int blue READ getBlue WRITE setBlue)
	/*! \brief Sets/gets the alpha component of this BarBase's color.*/
	Q_PROPERTY(int alpha READ getAlpha WRITE setAlpha)
	/*! \brief Sets/gets the this BarBase's color in the \#rrbbgg format (ex. \#FF2255).
	*	\note This script property is compatible with values read from/written to a ColorParameter.
	*/
	Q_PROPERTY(QVariant color READ getColor WRITE setColor)
public:
	BarBase();
	virtual ~BarBase(){};

	virtual void reset();
	virtual void draw();

	virtual QString friendlyTypeName(){return "Bar Base";};

	//! \brief An Asset-identifying string used with AssetFactory::addAssetType
	static const QString type;

	const QColor getColor() const;

	void setColor(QColor color);
	//! Sets the color of the BarBase.
	void setColor(QVariant color) { setColor(color.value<QColor>()); };

	//! Gets the red component of this BarBase's color.
	int getRed() const { return getColor().red(); };
	//! Gets the green component of this BarBase's color.
	int getGreen() const { return getColor().green(); };
	//! Gets the blue component of this BarBase's color.
	int getBlue() const { return getColor().blue(); };
	//! Gets the alpha component of this BarBase's color.
	int getAlpha() const { return getColor().alpha(); };
	//! Sets the red component of this BarBase's color.
	void setRed(int r){ QColor val = getColor(); val.setRed(r); setColor(val); };
	//! Sets the green component of this BarBase's color.
	void setGreen(int g){ QColor val = getColor(); val.setGreen(g); setColor(val); };
	//! Sets the blue component of this BarBase's color.
	void setBlue(int b){ QColor val = getColor(); val.setBlue(b); setColor(val); };
	//! Sets the alpha component of this BarBase's color.
	void setAlpha(int a){ QColor val = getColor(); val.setAlpha(a); setColor(val); };

public slots:
	//! Clears all of the current plot data.
	void clearPlot() { reset(); };

	//! Sets the current data set name
	void setDataset(QString dataset) { currentSetName_ = dataset; };

	/*! \brief Sets the color of this BarBase.
	 *	\details Inputs are from 0-255.
	 */
	void setColor(int r, int g, int b, int a = 255){ setColor(QColor(r, g, b, a)); };

signals:
	void initializeHistoSig(bool bDisplayLegend, const QColor &barColor, const QColor &canvasColor, int eBarType);
	void updateColumnsSig(const QString &dataSet, const QColor &color);
	void normalizeScaleSig(const QString &dataSet, double dAxisMax, double dTotalValue, const QList<double> &medium, const QList<double> &major);
	void setSamplesSig(const QString &dataSet, const QVector<QwtIntervalSample> &qvSamples);
	void setPointsSig(const QString &dataSet, const QVector<QPointF> &qvPoints);
	void setErrorSamplesSig(const QString &dataSet, const QVector<QwtIntervalSample>&);
	void setErrorBarsVisibleSig(bool);
	void scaleAxisSig(double dBinSize);
	void handleXLabelsSig(long, long);
	void callReplot(const QString &dataSet);
	void resetSig();

protected:
	virtual void replot();

	virtual void replot(const QString &setName);

	virtual void initView();

	void createNormalizedScale(double dMaxValue, double dTotalValue);

	//! Returns the current Bin Size
	virtual double getBinSize() const = 0;
	//! Returns the current Bin Spacing
	virtual double getBinSpacing() const = 0;

	ColumnType::ColumnType getColumnType() const;

	virtual QSharedPointer<OperatorPlotHandler> getNewHandler();
	virtual void connectDataSignals(QSharedPointer<OperatorPlotHandler> plotHandler);

	//! Sets X Labels for children that change the default behavior.  Accepts Lower and Upper bounds as arguments.
	virtual void handleXLabels(long, long) {};

	//! Returns the value of the NormalizedDisplay property.
	bool getNormalized() const { return propertyContainer_->getPropertyValue("NormalizedDisplay").toBool(); };

	virtual double _getSampleValue(long bin) const;

	/*! \brief Optional error calculation for children.  Called start of replot.
	*	\sa BarBase::replot
	*/
	virtual void _handleErrorInitial(QVector<QwtIntervalSample>&) {};

	/*! \brief Optional error calculation for children.  Called on each bin of replot.
	*	\sa BarBase::replot
	*/
	virtual void _handleErrorValue(long, double&, QVector<QwtIntervalSample>&) {};

	/*! \brief Optional error calculation for children.  Called at end of replot.
	*	\sa BarBase::replot
	*/
	virtual void _handleErrorFinal(QVector<QwtIntervalSample>&) {};


	void _adjustValue(long bin, double value);
	void _setValue(long bin, double value);
	double _getValue(long bin) const;

	void _dropBin(long bin);

	virtual void _createBin(long bin);
	virtual void _destroyBin(long bin);

	virtual void _createSet(const QString &setName);

	//! Flag to update the bins
	QHash<QString, bool> m_qhBinsModified;
	//! Flag to update the Colors and Brushes
	QHash<QString, bool> m_qhUpdateBrush;
	//! Flag to indicate the data within a set changed
	QHash<QString, bool> m_qhDataChanged;

	//!	The cumulative binwise values, indexed by dataset name.
	QHash<QString,QHash<long, double>> m_qhCumulValue;
	//! The color of each dataset, indexed by dataset name.
	QHash<QString, QColor> m_qhColor;
	//! A QStringList of Column Types
	QStringList columnTypes_;

	//! The current dataset
	QString currentSetName_;

	//! The cached lowest bin value
	long m_lCachedXMin;
	//! The cached highest bin value
	long m_lCachedXMax;

	//! Flag to indicate a total recalculation of bins is necessary
	bool m_bTotalBinRecalc;

	//! Used to stagger new colors
	char m_cUnasignedNum;
};


}; //namespace Picto

#endif
