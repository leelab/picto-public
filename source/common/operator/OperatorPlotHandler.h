#ifndef _OPERATOR_PLOT_HANDLER_H_
#define _OPERATOR_PLOT_HANDLER_H_

#include "../common.h"
#include "DataViewSpecs.h"

#include <QObject>
#include <QSharedPointer>
#include <QString>
#include <qwt_plot.h>

class QwtPlotItem;

namespace Picto {

/*! \brief A base class for manufacturing Widgets.
 *	\note This exists because Qt requires widgets to be constructed in the UI thread.
 *	\author Trevor Stavropoulos, Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2015
 */
#if defined WIN32 || defined WINCE
class PICTOLIB_API OperatorPlotHandler : public QObject
#else
class OperatorPlotHandler : public QObject
#endif
{
	Q_OBJECT
public:
	OperatorPlotHandler();
	virtual ~OperatorPlotHandler();

	QWidget *getWidget() { return m_pPlot; };
public slots:
	void initializePlot(const QString &xTitle, const QString &yTitle);
	void setTitle(const QString &title);

protected:
	QwtPlot *m_pPlot;
};

}; //namespace Picto

#endif
