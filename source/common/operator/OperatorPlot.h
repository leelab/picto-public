#ifndef _OPERATOR_PLOT_H_
#define _OPERATOR_PLOT_H_

#include "../common.h"
#include "DataViewElement.h"

#include <QHash>

#include <qwt_plot.h>
#include <qwt_plot_histogram.h>


namespace Picto {

/*! \brief A plot populated in realtime during experiments or analysis.
 *
 *	The plot is meant for display in the Operator View.
 *	\note We need to be careful how this is serialized, as sending actual plot data
 *  would be wildly redundant.
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
	static const QString type;

	virtual void draw();

	virtual QString friendlyTypeName(){return "Operator Plot";};
	virtual QString getUITemplate(){return "OperatorFeature";};
	virtual QString getUIGroup(){return "Operator Features";};

	virtual void submitValue(long iBin, double dValue);
	virtual void dropBin(long iBin);

	virtual QWidget *getWidget() { return m_pPlot; };

	virtual void setTitle(QString newTitle);

protected:
	virtual void postDeserialize();
	virtual bool validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader);

	/*! \brief The cumulative binwise-sum of submitted values. */
	QHash<long,double> m_qhdCumulValue;

	/*! \brief The cumulative binwise-sum of the squares of submitted values. */
	QHash<long, double> m_qhdCumulValSq;

	/*! \brief The cumulative binwise-total of the number of submitted values. */
	QHash<long, long> m_qhlCumulNum;

	virtual void replot();

	QwtPlotHistogram *m_pHistoPlotItem;
	QwtPlot *m_pPlot;

private:
	/*! \brief Flag for when the bounds need to be recalculated. */
	bool m_bBinsChanged;
	/*! \brief Flag for when the plot need to be recalculated. */
	bool m_bDataChanged;
	/*! \brief Min Bound. */
	long m_lPlotMin;
	/*! \brief Max Bound. */
	long m_lPlotMax;
};


}; //namespace Picto

#endif
