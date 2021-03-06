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
 *	\author Vered Zafrany, Trevor Stavropoulos, Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2017
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

	//! Returns a child-appropriate data-selection widget
	virtual QWidget *plotOptionsWidget() { return nullptr; };
	//! Virtual function to allow the toggling of the dataSelection part of the Plot Options widget
	virtual void hideDataSelectionWidget(bool) {};

public slots:
	void initializePlot(const QString &xTitle, const QString &yTitle);
	void setTitle(const QString &title);
	QString getTitle(){ return m_tmpTitle;};
	void connectToTaskConfig(QSharedPointer<TaskConfig> pTaskConfig);

	void exportPlot(int type, int size, const QString fileName);

protected:
	QWidget *m_pDataViewWidget;
	QwtPlot *m_pPlot;
	QString m_tmpTitle;
	int index;
};

}; //namespace Picto

#endif
