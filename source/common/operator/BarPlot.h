#ifndef _BAR_PLOT_H_
#define _BAR_PLOT_H_

#include "../common.h"
#include "BarBase.h"

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
class PICTOLIB_API BarPlot : public BarBase
#else
class BarPlot : public BarBase
#endif
{
	Q_OBJECT
public:
	BarPlot();
	virtual ~BarPlot(){};

	static QSharedPointer<Asset> Create();

	virtual QString friendlyTypeName(){return "Bar Plot";};

	//! An Asset-identifying string used with AssetFactory::addAssetType
	static const QString type;

public slots:
	//! Modifies the value in the indicated bin by the passed-in value.
	void adjustValue(long bin, double value) { _adjustValue(bin, value); };
	//! Sets the value in indicated bin to the value.
	void setValue(long bin, double value) { _setValue(bin, value); };
	//! Returns the value of the indicated bin.
	double getValue(long bin) { return _getValue(bin); };

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
