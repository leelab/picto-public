#include "DataViewWidget.h"
#include "ViewSelectionFrame.h"

#include "../../common/memleakdetect.h"

using namespace Picto;

/*! \brief Constructs a new DataViewWidget with the specified name and containing the passed Widget
*/
DataViewWidget::DataViewWidget(const QString cqsName, QWidget *pqwWidget, DVW_RetentionPolicy ePolicy)
	: currentlyDisplayed_(false), lastViewSize_(DataViewSize::VIEW_SIZE_1x1), xPos_(0), yPos_(0), eRetentionPolicy_(ePolicy),
	myViewWidget_(pqwWidget)
{
	layout_ = new QGridLayout();
	layout_->setMargin(0);

	myLabel_ = new QLabel(cqsName);
	layout_->addWidget(myLabel_, 0, 0);
	layout_->addWidget(myViewWidget_, 1, 0);

	layout_->setRowStretch(0, 0);
	layout_->setRowStretch(1, 1);

	setLayout(layout_);

	name_ = cqsName;
}

/*! \brief DataViewWidget destructor automatically dissociates itself from its view widget so the owning asset can control
 *	its memory.
 *	\sa DataViewWidget::dissociate()
 */
DataViewWidget::~DataViewWidget()
{
	dissociate();
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

/*! \brief This segregates the view widget from this DataViewWidget container prior to deletion, so that the actual view
 *	item can be managed by its true owner, the Asset responsible for spawning it.
 */
void DataViewWidget::dissociate()
{
	if (myViewWidget_)
	{
		layout_->removeWidget(myViewWidget_);
		myViewWidget_->setParent(nullptr);
		myViewWidget_ = nullptr;
	}
}

/*!	\brief Sets the current name and label of this widget
 */
void DataViewWidget::setName(const QString &newName)
{
	name_ = newName;
	myLabel_->setText(newName);
}

