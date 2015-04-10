#ifndef _SAMPLING_HISTOGRAM_PLOT_H_
#define _SAMPLING_HISTOGRAM_PLOT_H_

#include "../common.h"
#include "SamplingBarBase.h"

namespace Picto {

/*!	\brief A plot populated in realtime during experiments or analysis.
 *
 *	The plot is meant for display in the Operator View.
 *	\note We need to be careful how this is serialized, as sending actual plot data would be wildly redundant.
 *	\author Trevor Stavropoulos, Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2015
 */
#if defined WIN32 || defined WINCE
class PICTOLIB_API SamplingHistogramPlot : public SamplingBarBase
#else
class SamplingHistogramPlot : public SamplingBarBase
#endif
{
	Q_OBJECT
public:
	SamplingHistogramPlot();
	virtual ~SamplingHistogramPlot(){};

	static QSharedPointer<Asset> Create();

	virtual QString friendlyTypeName(){return "Sampling Histogram Plot";};

	//! \brief An Asset-identifying string used with AssetFactory::addAssetType
	static const QString type;

public slots:
	void submitValue(double bin, double value);
	double getValue(double bin) const;

	void setCumulativeValue(double bin, double value);
	double getCumulativeValue(double bin) const;

	void setSamples(double bin, long value);
	long getSamples(double bin) const;

	void setCumulativeValueSquared(double bin, double value);
	double getCumulativeValueSquared(double bin) const;

	void dropBin(double bin);

protected:
	//! \brief Returns the current Bin Size
	virtual double getBinSize() const { return propertyContainer_->getPropertyValue("BinSize").toDouble(); };
};


}; //namespace Picto

#endif
