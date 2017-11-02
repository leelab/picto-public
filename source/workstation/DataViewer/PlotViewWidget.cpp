#include <QLabel>
#include <QWeakPointer>

#include "PlotViewWidget.h"
#include "ViewSelectionFrame.h"
#include "../../common/operator/DataViewElement.h"

#include "../../common/memleakdetect.h"

#include <QMenu>

using namespace Picto;

/*! \brief Constructs a new DataViewWidget with the specified name and containing the passed Widget
 */
PlotViewWidget::PlotViewWidget(const QString cqsName, QWidget *pqwWidget, const ViewProperties defaultViewProperties)
	: DataViewWidget(cqsName, pqwWidget, DVW_CLEAR)
{
	setCurrentSize(defaultViewProperties.size_);
	setPosition(defaultViewProperties.x_, defaultViewProperties.y_);
	hideDefaultTitle();
	
	sameTabAsTask_ = true;	
}

/*! \brief DataViewWidget destructor automatically dissociates itself from its view widget so the owning asset can control
 *	its memory.
 *	\sa DataViewWidget::dissociate()
 */
PlotViewWidget::~PlotViewWidget()
{
	dissociate();
}
void PlotViewWidget::newTab()
{	
	sameTabAsTask_ = false;
	emit openInNewTab((QWidget*)(this));
}
bool PlotViewWidget::tabInfo()
{
	return sameTabAsTask_;
}
void PlotViewWidget::mouseDoubleClickEvent(QMouseEvent * e)
{
	if (e->button() == Qt::LeftButton)
	{
		newTab();
	}
}
