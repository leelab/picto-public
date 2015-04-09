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
	PlotViewWidget(const QString cqsName, QWidget *pqwWidget, DataViewElement *pOwningAsset);
	virtual ~PlotViewWidget();

	virtual const QString getName() const;
	virtual void setName(const QString &newName);
private:
	//! A raw pointer to the Asset that created the associated plot.
	DataViewElement *owningAsset_;
};

#endif
