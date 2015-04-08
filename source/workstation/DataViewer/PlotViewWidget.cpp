#include <QLabel>
#include <QWeakPointer>

#include "PlotViewWidget.h"
#include "ViewSelectionWidget.h"
#include "../../common/operator/DataViewElement.h"

#include "../../common/memleakdetect.h"

using namespace Picto;

/*! \brief Constructs a new DataViewWidget with the specified name and containing the passed Widget
 */
PlotViewWidget::PlotViewWidget(const QString cqsName, QWidget *pqwWidget, DataViewElement *pOwningAsset)
	: DataViewWidget(cqsName, pqwWidget, DVW_CLEAR)
{
	owningAsset_ = pOwningAsset;
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

/*! \brief Returns the name of the associated DataViewElement
 */
const QString PlotViewWidget::getName() const
{
	return owningAsset_->getName();
}

/*! \brief Sets the name of the associated DataViewElement
 */
void PlotViewWidget::setName(const QString &newName)
{
	owningAsset_->setName(newName);
}