#include <QLabel>
#include <QWeakPointer>

#include "PlotViewWidget.h"
#include "ViewSelectionFrame.h"
#include "../../common/operator/DataViewElement.h"

#include "../../common/memleakdetect.h"

using namespace Picto;

/*! \brief Constructs a new DataViewWidget with the specified name and containing the passed Widget
 */
PlotViewWidget::PlotViewWidget(const QString cqsName, QWidget *pqwWidget, DataViewSize::ViewSize defaultSize)
	: DataViewWidget(cqsName, pqwWidget, DVW_CLEAR)
{
	setCurrentSize(defaultSize);
	hideDefaultTitle();
}

/*! \brief DataViewWidget destructor automatically dissociates itself from its view widget so the owning asset can control
 *	its memory.
 *	\sa DataViewWidget::dissociate()
 */
PlotViewWidget::~PlotViewWidget()
{
	dissociate();
}