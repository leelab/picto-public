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

/*! \brief A plot populated in realtime during experiments or analysis.
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
	static const QString type;

	/* \brief Prepares the underlying Plot for rendering.
	 */
	virtual void draw();

	virtual QString friendlyTypeName(){return "Operator Plot";};
	virtual QString getUITemplate(){return "OperatorFeature";};
	virtual QString getUIGroup(){return "Operator Features";};

	virtual QWidget *getWidget() { return m_pPlot; };

	virtual void setTitle(const QString &newTitle);

	virtual const QString getTitle() const;
	/*	\brief Clears all values for this plot.
	*/
	virtual void reset();
public slots:
	/*	\brief Accumulates a value in the Histogram's data in the given bin.  The displayed value is the average of all
	 *	values inserted in said bin.
	 */
	void submitValue(long bin, double value);
	/*	\brief Eliminates the values in indicated bin.
	 */
	void dropBin(long bin);
	/*	\brief Sets the value in indicated bin to the value.
	*/
	void setBin(long bin, double value);
	/*	\brief Sets the value of the square of the indicated bin's submissions.
	*/
	void setBinSquared(long bin, double value);
	/*	\brief Sets the number of samples taken in indicated bin to the value.
	*/
	void setBinSamples(long bin, long samples);
	/*	\brief Returns the value of the indicated bin.
	*/
	double getBin(long bin);
	/*	\brief Returns the square of the values of the indicated bin's submissions.
	*/
	double getBinSquared(long bin);
	/*	\brief Returns the number of Samples taken for the indicated bin.
	*/
	long getSamples(long bin);
protected:
	virtual void postDeserialize();
	virtual bool validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader);

	/*! \brief The cumulative binwise-sum of submitted values.
	 */
	QHash<long,double> m_qhdCumulValue;

	/*! \brief The cumulative binwise-sum of the squares of submitted values.
	 */
	QHash<long, double> m_qhdCumulValSq;

	/*! \brief The cumulative binwise-total of the number of submitted values.
	 */
	QHash<long, long> m_qhlCumulNum;

	/*! \brief Reconstructs elements of underlying plot.
	 */
	virtual void replot();

	QwtPlotHistogram *m_pHistoPlotItem;
	QwtPlotIntervalCurve *m_pErrorBars;
	QwtPlot *m_pPlot;

private:
	/*! \brief Flag for when the bounds need to be recalculated.
	 */
	bool m_bBinsChanged;
	/*! \brief Flag for when the plot need to be recalculated.
	 */
	bool m_bDataChanged;
	/*! \brief Min Bound.
	 */
	long m_lPlotMin;
	/*! \brief Max Bound.
	 */
	long m_lPlotMax;
	/*! \brief Last status of ShowErrorBars, used for updating plot on change.
	*/
	bool m_bLastErrorBarState;
};


}; //namespace Picto

#endif
