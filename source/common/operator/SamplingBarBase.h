#ifndef _SAMPLING_BAR_BASE_H_
#define _SAMPLING_BAR_BASE_H_

#include "../common.h"
#include "BarBase.h"

#include <QHash>

class QwtPlotIntervalCurve;

namespace Picto {

/*!	\brief A plot populated in realtime during experiments or analysis.
 *
 *	The plot is meant for display in the Operator View.
 *	\note We need to be careful how this is serialized, as sending actual plot data would be wildly redundant.
 *	\author Trevor Stavropoulos, Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2015
 */
#if defined WIN32 || defined WINCE
class PICTOLIB_API SamplingBarBase : public BarBase
#else
class SamplingBarBase : public BarBase
#endif
{
	Q_OBJECT
public:
	SamplingBarBase();
	virtual ~SamplingBarBase(){};

	virtual void draw();
	virtual void reset();

	virtual QString friendlyTypeName(){return "Sampling Bar Base";};

	//! \brief An Asset-identifying string used with AssetFactory::addAssetType
	static const QString type;

protected:
	virtual void postDeserialize();

	virtual double _getSampleValue(long bin) const;

	virtual void _handleErrorInitial(QVector<QwtIntervalSample>&) {};
	virtual void _handleErrorValue(long bin, double &rdRangeMax, QVector<QwtIntervalSample> &qvError);
	virtual void _handleErrorFinal(QVector<QwtIntervalSample> &qvError);

	virtual void _createBin(long bin);
	virtual void _destroyBin(long bin);

	void _submitValue(long bin, double value);

	void _setSamples(long bin, long samples);
	long _getSamples(long bin) const;

	void _setValueSquared(long bin, double value);
	double _getValueSquared(long bin) const;

	//! Returns the value of the DisplayStdErr property.
	bool _getDisplayErr() const { return propertyContainer_->getPropertyValue("DisplayStdErr").toBool(); };

	//!	The cumulative binwise-sum of the squares of submitted values.
	QHash<long, double> m_qhdCumulValSq;
	//!	The cumulative binwise-total of the number of submitted values.
	QHash<long, long> m_qhlCumulNum;

	//! A pointer to the SamplingBarBase's IntervalCurve used to represent error bars
	QwtPlotIntervalCurve *m_pErrorBars;

	//!	Last status of ShowErrorBars, used for updating plot on change.
	bool m_bLastErrorBarState;
};


}; //namespace Picto

#endif