#ifndef _SAMPLING_BAR_PLOT_H_
#define _SAMPLING_BAR_PLOT_H_

#include "../common.h"
#include "SamplingBarBase.h"

namespace Picto {

class BarAxisHandler;
/*!	\brief A plot populated in realtime during experiments or analysis.
 *
 *	The plot is meant for display in the Operator View.
 *	\note We need to be careful how this is serialized, as sending actual plot data would be wildly redundant.
 *	\author Trevor Stavropoulos, Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2015
 */
#if defined WIN32 || defined WINCE
class PICTOLIB_API SamplingBarPlot : public SamplingBarBase
#else
class SamplingBarPlot : public SamplingBarBase
#endif
{
	Q_OBJECT
public:
	SamplingBarPlot();
	virtual ~SamplingBarPlot(){};

	static QSharedPointer<Asset> Create();

	virtual QString friendlyTypeName(){return "Sampling Bar Plot";};

	//! \brief An Asset-identifying string used with AssetFactory::addAssetType
	static const QString type;

public slots:
	//! Modifies the indicated bin with the passed-in sample.
	void submitValue(long bin, double value) { _submitValue(bin, value); };
	//! Returns the average of the samples submitted to the indicated bin.
	double getValue(long bin) const { return _getSamples(bin) > 0 ? (_getValue(bin) / _getSamples(bin)) : 0.0; };

	//! Sets the accumulated value in indicated bin to the value.
	void setCumulativeValue(long bin, double value) { _setValue(bin, value); };
	//! Returns the accumulated value in the indicated bin.
	double getCumulativeValue(long bin) const { return _getValue(bin); };

	//! Sets the number of samples received for the indicated bin to the value.
	void setSamples(long bin, long value) { _setSamples(bin, value); };
	//! Returns the number of samples received for the indicated bin.
	long getSamples(long bin) const { return _getSamples(bin); };

	//! Sets the accumualted sample-value-squared for the indicated bin to the value.
	void setCumulativeValueSquared(long bin, double value) { _setValueSquared(bin, value); };
	//! Returns the accumulated sample-value-squared for the indicated bin.
	double getCumulativeValueSquared(long bin) const { return _getValueSquared(bin); };

	const QString getLabel(long bin) const;
	void setLabel(long bin, QString name);

	//! Eliminates the values in indicated bin.
	void dropBin(long bin) { _dropBin(bin); };

protected:
	virtual void postDeserialize();
	virtual double getBinSize() const { return 1.0; };
	virtual double getBinSpacing() const { return propertyContainer_->getPropertyValue("BinSpacing").toDouble(); };
	virtual void handleXLabels(long lLowerBound, long lUpperBound);

	//! Manages the storing and rendering of axis names
	BarAxisHandler *m_pAxisHandler;
};


}; //namespace Picto

#endif
