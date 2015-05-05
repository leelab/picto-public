#include <QPalette>
#include <QLabel>
#include <QGridLayout>

#include "DataViewOrganizer.h"
#include "ViewSelectionFrame.h"

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
