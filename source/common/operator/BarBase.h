#ifndef _BAR_BASE_H_
#define _BAR_BASE_H_

#include "../common.h"
#include "OperatorPlot.h"

#include <QHash>

class QwtPlotHistogram;
class QwtIntervalSample;

namespace Picto {

/*!	\brief A plot populated in realtime during experiments or analysis.
 *
 *	The plot is meant for display in the Operator View.
 *	\note We need to be careful how this is serialized, as sending actual plot data would be wildly redundant.
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
public:
	BarBase();
	virtual ~BarBase(){};

	virtual void reset();

	virtual QString friendlyTypeName(){return "Bar Base";};

	//! \brief An Asset-identifying string used with AssetFactory::addAssetType
	static const QString type;

public slots:
	//! Clears all of the current plot data.
	void clearPlot() { reset(); };

protected:
	virtual void postDeserialize();
	virtual void replot();
	void createNormalizedScale(double dMaxValue, double dTotalValue);

	//! Flag to update the bins
	bool m_bBinsModified;
	//! Returns the current Bin Size
	virtual double getBinSize() const = 0;
	//! Returns the current Bin Spacing
	virtual double getBinSpacing() const = 0;
	//! Sets X Labels for children that change the default behavior.  Accepts Lower and Upper bounds as arguments.
	virtual void handleXLabels(long, long) {};

	//! Returns the value of the NormalizedDisplay property.
	bool getNormalized() const { return propertyContainer_->getPropertyValue("NormalizedDisplay").toBool(); };

	virtual double _getSampleValue(long bin) const;

	/*! \brief Optional error calculation for future children.  Called start of replot.
	*	\sa BarBase::replot
	*/
	virtual void _handleErrorInitial(QVector<QwtIntervalSample>&) {};

	/*! \brief Optional error calculation for future children.  Called on each bin of replot.
	*	\sa BarBase::replot
	*/
	virtual void _handleErrorValue(long, double&, QVector<QwtIntervalSample>&) {};

	/*! \brief Optional error calculation for future children.  Called at end of replot.
	*	\sa BarBase::replot
	*/
	virtual void _handleErrorFinal(QVector<QwtIntervalSample>&) {};

	void _adjustValue(long bin, double value);
	void _setValue(long bin, double value);
	double _getValue(long bin) const;

	void _dropBin(long bin);

	virtual void _createBin(long bin);
	virtual void _destroyBin(long bin);

	//!	\brief The cumulative binwise values.
	QHash<long, double> m_qhdCumulValue;
	//! A pointer to the BarBase's Qwt Histogram object
	QwtPlotHistogram *m_pHistoPlotItem;
};


}; //namespace Picto

#endif
