#ifndef _HISTOGRAM_PLOT_H_
#define _HISTOGRAM_PLOT_H_

#include "../common.h"
#include "BarBase.h"

namespace Picto {

/*!	\brief A plot populated in realtime during experiments or analysis.
 *
 *	The plot is meant for display in the Operator View.
 *	\note We need to be careful how this is serialized, as sending actual plot data would be wildly redundant.
 *	\author Trevor Stavropoulos, Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2015
 */
#if defined WIN32 || defined WINCE
class PICTOLIB_API HistogramPlot : public BarBase
#else
class HistogramPlot : public BarBase
#endif
{
	Q_OBJECT
public:
	HistogramPlot();
	virtual ~HistogramPlot(){};

	static QSharedPointer<Asset> Create();

	virtual QString friendlyTypeName(){return "Histogram Plot";};

	//! \brief An Asset-identifying string used with AssetFactory::addAssetType
	static const QString type;

public slots:
	void adjustValue(double bin, double value);
	void setValue(double bin, double value);
	double getValue(double bin);

	void dropBin(double bin);

protected:
	//! \brief Returns the current Bin Size
	virtual double getBinSize() const { return propertyContainer_->getPropertyValue("BinSize").toDouble(); };
	//! Returns the current Bin Spacing
	virtual double getBinSpacing() const { return 0.0; };

	virtual QSharedPointer<OperatorPlotHandler> getNewHandler();
};


}; //namespace Picto

#endif
