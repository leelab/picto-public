#ifndef _OPERATOR_PLOT_H_
#define _OPERATOR_PLOT_H_

#include "../common.h"
#include "DataViewElement.h"
#include <qwt_plot.h>

#include <QHash>

class QwtPlotHistogram;
class QwtPlotIntervalCurve;
class QwtPlot;

namespace Picto {

/*!	\brief A plot populated in realtime during experiments or analysis.
 *
 *	The plot is meant for display in the Operator View.
 *	\note We need to be careful how this is serialized, as sending actual plot data would be wildly redundant.
 *	\author Trevor Stavropoulos, Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2015
 */
#if defined WIN32 || defined WINCE
class PICTOLIB_API OperatorPlot : public DataViewElement
#else
class OperatorPlot : public DataViewElement
#endif
{
	Q_OBJECT
public:
	OperatorPlot();
	virtual ~OperatorPlot(){};

	static QSharedPointer<Asset> Create();

	/*!	\brief Prepares the underlying Plot for rendering.
	 */
	virtual void draw();

	virtual QString friendlyTypeName(){return "Operator Plot";};
	virtual QString getUITemplate(){return "OperatorFeature";};
	virtual QString getUIGroup(){return "Operator Features";};

	/*!	\brief Returns a pointer to the underlying plot element.  This is used by the TaskConfig object to pass the
	 *	widgets to the ViewSelectionWidget.
	 */
	virtual QWidget *getWidget() { return m_pPlot; };

	virtual void setTitle(const QString &newTitle);
	virtual const QString getTitle() const;

	virtual void reset();

public slots:
	void submitValue(long bin, double value);
	void dropBin(long bin);
	void setBin(long bin, double value);
	void setBinSquared(long bin, double value);
	void setBinSamples(long bin, long samples);
	double getBin(long bin);
	double getBinSquared(long bin);
	long getSamples(long bin);

protected:
	virtual void postDeserialize();
	virtual bool validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader);
	virtual void replot();

public:
	//! \brief An Asset-identifying string used with AssetFactory::addAssetType
	static const QString type;

protected:
	//!	\brief The cumulative binwise-sum of submitted values.
	QHash<long,double> m_qhdCumulValue;
	//!	\brief The cumulative binwise-sum of the squares of submitted values.
	QHash<long, double> m_qhdCumulValSq;
	//!	\brief The cumulative binwise-total of the number of submitted values.
	QHash<long, long> m_qhlCumulNum;

	//! A pointer to the OperatorPlot's Qwt Histogram object
	QwtPlotHistogram *m_pHistoPlotItem;
	//! A pointer to the OperatorPlot's IntervalCurve used to represent error bars
	QwtPlotIntervalCurve *m_pErrorBars;
	//! A pointer to the OperatorPlot's underlying Qwt Plot widget
	QwtPlot *m_pPlot;

private:
	//!	\brief Flag for when the bounds need to be recalculated.
	bool m_bBinsChanged;
	//!	\brief Flag for when the plot need to be recalculated.
	bool m_bDataChanged;
	//!	\brief Min Bound.
	long m_lPlotMin;
	//!	\brief Max Bound.
	long m_lPlotMax;
	//!	\brief Last status of ShowErrorBars, used for updating plot on change.
	bool m_bLastErrorBarState;
};


}; //namespace Picto

#endif
