#include <QPalette>
#include <QLabel>

#include "DataViewLayout.h"
#include "ViewSelectionWidget.h"

#include "../../common/memleakdetect.h"

using namespace Picto;

/*! \brief Constructs a new DataViewLayout.
*/
DataViewLayout::DataViewLayout()
{
	setColumnStretch(0, 1);
	setColumnStretch(1, 1);
	setColumnStretch(2, 1);
	setColumnStretch(3, 1);

	setColumnMinimumWidth(0, 100);
	setColumnMinimumWidth(1, 100);
	setColumnMinimumWidth(2, 100);
	setColumnMinimumWidth(3, 100);

	setRowStretch(0, 1);
	setRowStretch(1, 1);
	setRowStretch(2, 1);
	setRowStretch(3, 1);

	setRowMinimumHeight(0, 100);
	setRowMinimumHeight(1, 100);
	setRowMinimumHeight(2, 100);
	setRowMinimumHeight(3, 100);
}

/*! \brief DataViewLayout destructor
*/
DataViewLayout::~DataViewLayout()
{

}

/*! \brief A slot emitted by the ViewSelectionWidget to signal that the specified widget should be removed from the
 *	layout.
 */
void DataViewLayout::removeWidgetSlot(QWidget *pWidget)
{
	removeWidget(pWidget);
}

/*! \brief A slot emitted by the ViewSelectionWidget to signal that the specified widget should be added to the
*	layout at the specified position with the indiciated span.
*/
void DataViewLayout::addWidgetSlot(QWidget *pWidget, int x, int y, DataViewSize::ViewSize size)
{
	addWidget(pWidget, x, y, size, size);
}
