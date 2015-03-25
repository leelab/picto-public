#include <QPalette>
#include <QLabel>

#include "DataViewWidget.h"
#include "ViewSelectionWidget.h"

#include "../../common/memleakdetect.h"

using namespace Picto;

/*! \brief Constructs a new DataViewWidget with the specified name and containing the passed Widget
*/
DataViewWidget::DataViewWidget(const QString cqsName, QWidget *pqwWidget)
	: currentlyDisplayed_(false), lastViewSize_(VIEW_SIZE_1x1), xPos_(0), yPos_(0)
{
	layout_ = new QGridLayout();
	layout_->addWidget(new QLabel(cqsName),0,0);
	layout_->addWidget(pqwWidget,1,0);

	layout_->setRowStretch(0, 0);
	layout_->setRowStretch(1, 1);

	setLayout(layout_);

	name_ = cqsName;
}

/*! \brief DataViewWidget destructor
*/
DataViewWidget::~DataViewWidget()
{

}

/*! \brief Hides the default DataViewWidget Title, for when the attached widget comes with
 *	its own title.
 */
void DataViewWidget::hideDefaultTitle()
{
	layout_->itemAtPosition(0, 0)->widget()->hide();
}

/*! \brief Sets whether the widget is currently displayed
 */
void DataViewWidget::setDisplayed(bool enable)
{
	currentlyDisplayed_ = enable;
}

/*! \brief Sets the grid position for the widget
*/
void DataViewWidget::setPosition(int xPos, int yPos)
{
	Q_ASSERT(xPos < VIEWGRIDWIDTH);
	Q_ASSERT(yPos < VIEWGRIDHEIGHT);
	xPos_ = xPos;
	yPos_ = yPos;
}

/*! \brief Returns the grid position for the widget
*/
void DataViewWidget::getPosition(int &xPos, int &yPos)
{
	xPos = xPos_;
	yPos = yPos_;
}


/*! \brief Returns whether or not the indicated position is occupied by this widget.
 *	\note This does not check whether or not the current widget is set to be displayed.
*/
bool DataViewWidget::containsPosition(int xPos, int yPos)
{
	if (xPos >= xPos_ && xPos < xPos_ + lastViewSize_)
	{
		if (yPos >= yPos_ && yPos < yPos_ + lastViewSize_)
		{
			return true;
		}
	}

	return false;
}