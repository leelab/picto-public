#ifndef _DATA_VIEW_ORGANIZER_H_
#define _DATA_VIEW_ORGANIZER_H_

#include <QString>
#include <QVector>
#include <QWidget>

#include "../viewer.h"
#include "../../common/operator/DataViewSpecs.h"


QT_BEGIN_NAMESPACE
class QGridLayout;
QT_END_NAMESPACE

using namespace Picto;

/*!	\brief A widget to contain objects meant to be displayed in the Experiment Viewer
 *  \details The DataViewOrganizer is a simple widget responsible for positioning and manipulating DataViewWidgets.
 *	The real bulk of the work is done by the ViewSelectionFrame.
 *	\author Trevor Stavropoulos, Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2015
 */
class DataViewOrganizer : public QWidget
{
	Q_OBJECT
public:
	DataViewOrganizer();
	virtual ~DataViewOrganizer();
public slots:
	void removeWidgetSlot(QWidget *pWidget);
	void addWidgetSlot(QWidget *pWidget, int x, int y, DataViewSize::ViewSize size);
protected:
	QGridLayout *layout_;
};

#endif
