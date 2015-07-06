#ifndef _OPERATOR_PLOT_HANDLER_H_
#define _OPERATOR_PLOT_HANDLER_H_

#include "../common.h"
#include "DataViewSpecs.h"

#include <QObject>
#include <QSharedPointer>
#include <QString>

#include <qwt_plot.h>

namespace Picto {

class TaskConfig;

/*! \brief A Handler for plots derived from all OperatorPlots
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
	void connectToTaskConfig(QSharedPointer<TaskConfig> pTaskConfig);

	void exportPlot(int type, int size, const QString fileName);
	void requestExport(ExportType::ExportType type);

protected:
	QwtPlot *m_pPlot;
	QString m_tmpTitle;
	int index;
};

}; //namespace Picto

#endif
