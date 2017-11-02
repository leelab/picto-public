#ifndef _DATA_VIEW_ORGANIZER_H_
#define _DATA_VIEW_ORGANIZER_H_

#include <QString>
#include <QVector>
#include <QWidget>
#include <QSplitter>

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

signals:
	void widgetDropped(QPoint offset);
	void widgetDragged(QWidget *pWidget);

public slots:
	void removeWidgetSlot(QWidget *pWidget);
	void addWidgetSlot(QWidget *pWidget, int x, int y, DataViewSize::ViewSize size);
	
protected:	

	void dragEnterEvent(QDragEnterEvent *event) override;
	void dragMoveEvent(QDragMoveEvent *event) override;
	void dropEvent(QDropEvent *event) override;
	void mousePressEvent(QMouseEvent *event) override;

	QGridLayout *layout_;
	QSplitter *vSplitter_;
	QSplitter *hSplitter1_;
	QSplitter *hSplitter2_;
	QSplitter *hSplitter3_;
	QSplitter *hSplitter4_;

};

#endif
