#ifndef _DATA_VIEW_LAYOUT_H_
#define _DATA_VIEW_LAYOUT_H_

#include <QString>
#include <QVector>
#include <QWidget>
#include <QGridLayout>

#include "../viewer.h"

using namespace Picto;

enum ViewSize : int;

/*!	\brief A widget to contain objects meant to be displayed in the Experiment Viewer
 *  \details Details forthcoming
 *	\author Trevor Stavropoulos, Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2015
 */
class DataViewLayout : public QGridLayout
{
	Q_OBJECT
public:
	DataViewLayout();
	virtual ~DataViewLayout();
public slots:
	void removeWidgetSlot(QWidget *pWidget);
	void addWidgetSlot(QWidget *pWidget, int x, int y, ViewSize size);
};

#endif
