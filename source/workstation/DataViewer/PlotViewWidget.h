#ifndef _PLOT_VIEW_WIDGET_H_
#define _PLOT_VIEW_WIDGET_H_

#include <QString>

#include "../viewer.h"
#include "DataViewWidget.h"

using namespace Picto;

/*!	\brief A widget to contain plots meant to be displayed in the Experiment Viewer
 *  \details Details forthcoming
 *	\author Trevor Stavropoulos, Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2015
 */
class PlotViewWidget : public DataViewWidget
{
	Q_OBJECT
public:
	PlotViewWidget(const QString cqsName, QWidget *pqwWidget, const ViewProperties defaultViewProperties);
	virtual ~PlotViewWidget();

	bool tabInfo();
	
	//! Returns true if the widget is still in the same original tab as the Task, returns false if it was open in a new tab.
	bool sameTabAsTask_;

public slots:	
	void newTab();
	void mouseDoubleClickEvent(QMouseEvent * e)  override;

signals:
	void openInNewTab(QWidget* pWidget);
};

#endif
