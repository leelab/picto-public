#include <QPalette>
#include <QLabel>
#include <QGridLayout>
#include <QSplitter>

#include "DataViewOrganizer.h"
#include "ViewSelectionFrame.h"
#include "DataViewWidget.h"
#include <QDrag>
#include <QMimeData>

#include "../../common/memleakdetect.h"

using namespace Picto;

/*! \brief Constructs a new DataViewOrganizer.
*/
DataViewOrganizer::DataViewOrganizer()
{
	layout_ = new QGridLayout();

	setLayout(layout_);

	layout_->setContentsMargins(0, 0, 0, 0);
	layout_->setMargin(0);

	layout_->setColumnStretch(0, 1);
	layout_->setColumnStretch(1, 1);
	layout_->setColumnStretch(2, 1);
	layout_->setColumnStretch(3, 1);

	layout_->setColumnMinimumWidth(0, 100);
	layout_->setColumnMinimumWidth(1, 100);
	layout_->setColumnMinimumWidth(2, 100);
	layout_->setColumnMinimumWidth(3, 100);

	layout_->setRowStretch(0, 1);
	layout_->setRowStretch(1, 1);
	layout_->setRowStretch(2, 1);
	layout_->setRowStretch(3, 1);

	layout_->setRowMinimumHeight(0, 100);
	layout_->setRowMinimumHeight(1, 100);
	layout_->setRowMinimumHeight(2, 100);
	layout_->setRowMinimumHeight(3, 100);

	setAcceptDrops(true);
}

/*! \brief DataViewOrganizer destructor
*/
DataViewOrganizer::~DataViewOrganizer()
{

}

/*! \brief A slot emitted by the ViewSelectionFrame to signal that the specified widget should be removed from the
 *	layout.
 */
void DataViewOrganizer::removeWidgetSlot(QWidget *pWidget)
{
	layout_->removeWidget(pWidget);
}

/*! \brief A slot emitted by the ViewSelectionFrame to signal that the specified widget should be added to the
*	layout at the specified position with the indiciated span.
*/
void DataViewOrganizer::addWidgetSlot(QWidget *pWidget, int x, int y, DataViewSize::ViewSize size)
{
	//pWidget->setStyleSheet("border: 1px solid black");
	layout_->addWidget(pWidget, x, y, size, size);
}

void DataViewOrganizer::dragEnterEvent(QDragEnterEvent *event)
{
	if (event->mimeData()->hasFormat("application/x-dnditemdata")) {
		if (event->source() == this) {
			event->setDropAction(Qt::MoveAction);
			event->accept();
		}
		else {
			event->acceptProposedAction();
		}
	}
	else {
		event->ignore();
	}
}

void DataViewOrganizer::dragMoveEvent(QDragMoveEvent *event)
{
	if (event->mimeData()->hasFormat("application/x-dnditemdata")) {
		if (event->source() == this) {
			event->setDropAction(Qt::MoveAction);
			event->accept();
		}
		else {
			event->acceptProposedAction();
		}
	}
	else {
		event->ignore();
	}
}

void DataViewOrganizer::dropEvent(QDropEvent *event)
{
	if (event->mimeData()->hasFormat("application/x-dnditemdata")) {

		QWidget *newWidget = new QWidget();
		QByteArray itemData = event->mimeData()->data("application/x-dnditemdata");
		QDataStream dataStream(&itemData, QIODevice::ReadOnly);

		QPixmap pixmap;
		QPoint offset;

		dataStream >> pixmap >> offset;

		QPainter p;
		p.begin(&pixmap);
		newWidget->render(&p);
		p.end();

		QPoint eventPos = QPoint(event->pos() - offset);
		QPoint pos(this->width() / 4, this->height() / 4);
		int col = event->pos().x() / pos.x();
		int row = event->pos().y() / pos.y();
		QPoint dropLocation(row, col);

		emit widgetDropped(dropLocation);

		if (event->source() == this) {
			event->setDropAction(Qt::MoveAction);
			event->accept();
		}
		else {
			event->acceptProposedAction();
		}


	}
	else {
		event->ignore();
	}

}

void DataViewOrganizer::mousePressEvent(QMouseEvent *event)
{
	if (event->button() == Qt::RightButton)
	{
		DataViewWidget *child = static_cast<DataViewWidget*>(childAt(event->pos()));
		if (!child)
			return;

		QPoint eventPos = event->pos();
		QPoint pos(this->width() / 4, this->height() / 4);

		int col = event->pos().x() / pos.x();
		int row = event->pos().y() / pos.y();

		if (layout_ && layout_->itemAtPosition(row, col))
		{
			QLayoutItem* l_item = layout_->itemAtPosition(row, col);
			if (l_item)
			{
				QWidget* widget = l_item->widget();
				if (widget)
				{
					DataViewWidget* currWidget = dynamic_cast<DataViewWidget*>(widget);
					if (currWidget && currWidget->getCurrentSize() != DataViewSize::VIEW_SIZE_4x4)
					{
						emit widgetDragged(currWidget); //set as current selection

						//if (event->button() == Qt::LeftButton)
						{
							QPixmap pixmap = QPixmap::grabWidget(currWidget);

							QByteArray itemData;
							QDataStream dataStream(&itemData, QIODevice::WriteOnly);

							dataStream << pixmap << QPoint(event->pos() - currWidget->pos());

							QMimeData *mimeData = new QMimeData;
							mimeData->setData("application/x-dnditemdata", itemData);

							QDrag *drag = new QDrag(this);
							drag->setMimeData(mimeData);
							drag->setPixmap(pixmap);
							drag->setHotSpot(event->pos() - currWidget->pos());

							QPixmap tempPixmap = pixmap;
							QPainter painter;
							painter.begin(&tempPixmap);
							painter.fillRect(pixmap.rect(), QColor(127, 127, 127, 127));
							currWidget->render(&painter);
							painter.end();

							if (drag->exec())
							{
								currWidget->show();

								QPainter p;
								p.begin(&pixmap);
								currWidget->render(&p);
								p.end();
							}
						}
					}
				}
			}
		}
	}
}





