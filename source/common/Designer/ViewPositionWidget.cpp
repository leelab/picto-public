
#include <QVBoxLayout>
#include <QCheckBox>
#include <QGridLayout>
#include <QLabel>
#include <QComboBox>

#include "ViewPositionWidget.h"
#include "../../common/memleakdetect.h"

/*! \brief Constructs a new ScriptWidget
 *	\details manager and property are used to copy/set the value of the underlying QtProperty to/from the underlying
 *	ScriptTextEdit.  editorState is used to support search functionality.  Single line indicates if this ScriptWidget
 *	should contain a single line text box rather than a multi-line text editor.
 */
ViewPositionWidget::ViewPositionWidget(QtVariantPropertyManager* manager, QtProperty* property, QSharedPointer<EditorState> editorState, QWidget *parent) :
	QWidget(parent),
	manager_(manager),
	property_(property),
	layout_(new QHBoxLayout()),
	editorState_(editorState)
{
	QWidget *propertyWidget = new QWidget(this);

	QGridLayout *propertyLayout = new QGridLayout(propertyWidget);
	propertyWidget->setLayout(propertyLayout);

	//Size Selection
	sizeSelection_ = new QComboBox();
	sizeSelection_->setSizeAdjustPolicy(QComboBox::AdjustToContents);
	sizeSelection_->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Fixed);
	sizeSelection_->setInsertPolicy(QComboBox::InsertAtBottom);
	sizeSelection_->setEditable(false);

	sizeSelection_->addItem("1x1", DataViewSize::VIEW_SIZE_1x1);
	sizeSelection_->addItem("2x2", DataViewSize::VIEW_SIZE_2x2);
	sizeSelection_->addItem("3x3", DataViewSize::VIEW_SIZE_3x3);
	sizeSelection_->addItem("4x4", DataViewSize::VIEW_SIZE_4x4);

	connect(sizeSelection_, SIGNAL(currentIndexChanged(int)), this, SLOT(selectedSizeIndexChanged(int)));

	propertyLayout->addWidget(new QLabel("Size"), 0, 0);
	propertyLayout->addWidget(sizeSelection_, 0, 2, 1, 2);


	//Position Selection
	QWidget *checkboxWidget = new QWidget(propertyWidget);
	QGridLayout *checkboxGridLayout = new QGridLayout(checkboxWidget);

	//Fill grid with checkboxes
	for (int x = 0; x < VIEWGRIDWIDTH; x++)
	{
		for (int y = 0; y < VIEWGRIDHEIGHT; y++)
		{
			positionCheckBoxes_[x][y] = new QCheckBox();
			connect(positionCheckBoxes_[x][y], SIGNAL(clicked(bool)), this, SLOT(checkboxClicked(bool)));
			coordsMap[positionCheckBoxes_[x][y]] = QSharedPointer<WidgetCoords>(new WidgetCoords(x, y));
			checkboxGridLayout->addWidget(positionCheckBoxes_[x][y], x, y);
		}
	}
	checkboxWidget->setLayout(checkboxGridLayout);

	propertyLayout->addWidget(new QLabel("Position"), 1, 0);
	propertyLayout->addWidget(checkboxWidget, 1, 2, 4, 1);

	layout_->addWidget(propertyWidget);
	((QHBoxLayout*)layout_)->addStretch();

	layout_->setContentsMargins(QMargins(0, 0, 0, 0));
	setLayout(layout_);

	currentViewProperties_ = manager_->value(property).value<ViewProperties>();

	if (sizeSelection_->currentData() != currentViewProperties_.size_)
	{
		sizeSelection_->setCurrentIndex(sizeSelection_->findData(currentViewProperties_.size_));
	}

	rectifyNewProperties();
}

/*! \brief Called when the underlying ViewProperties changes.  Updates the underlying QtProperty value accordingly.
 */
void ViewPositionWidget::pushChange()
{
	manager_->setValue(property_, QVariant::fromValue(getViewProperties()));
}


const Picto::ViewProperties &ViewPositionWidget::getViewProperties() const
{
	return currentViewProperties_;
}

void ViewPositionWidget::checkboxClicked(bool)
{
	QCheckBox *pCheck = (QCheckBox *)QObject::sender();
	if (coordsMap.contains(pCheck))
	{
		if (pCheck->isChecked())
		{
			currentViewProperties_.x_ = coordsMap[pCheck]->x_;
			currentViewProperties_.y_ = coordsMap[pCheck]->y_;
		}
		else
		{
			currentViewProperties_.x_ = -1;
			currentViewProperties_.y_ = -1;
		}
	}

	rectifyNewProperties();
	pushChange();
}

void ViewPositionWidget::rectifyNewProperties()
{
	for (int x = 0; x < VIEWGRIDWIDTH; x++)
	{
		for (int y = 0; y < VIEWGRIDHEIGHT; y++)
		{
			positionCheckBoxes_[x][y]->setDisabled(false);
			positionCheckBoxes_[x][y]->setCheckState(Qt::Unchecked);
		}
	}

	const int xPos = currentViewProperties_.x_;
	const int yPos = currentViewProperties_.y_;
	const int size = (int)currentViewProperties_.size_;

	if (xPos > -1 && yPos > -1)
	{
		for (int x = xPos; x < xPos + size; x++)
		{
			for (int y = yPos; y < yPos + size; y++)
			{
				Q_ASSERT(x < VIEWGRIDWIDTH);
				Q_ASSERT(y < VIEWGRIDHEIGHT);
				if (x == xPos && y == yPos)
				{
					positionCheckBoxes_[x][y]->setCheckState(Qt::Checked);
				}
				else
				{
					positionCheckBoxes_[x][y]->setCheckState(Qt::PartiallyChecked);
				}
			}
		}
	}
	//Disable impossible positions
	for (int x = VIEWGRIDWIDTH - size + 1; x < VIEWGRIDWIDTH; x++)
	{
		for (int y = 0; y < VIEWGRIDHEIGHT; y++)
		{
			positionCheckBoxes_[x][y]->setDisabled(true);
		}
	}

	for (int x = 0; x < VIEWGRIDWIDTH; x++)
	{
		for (int y = VIEWGRIDHEIGHT - size + 1; y < VIEWGRIDHEIGHT; y++)
		{
			positionCheckBoxes_[x][y]->setDisabled(true);
		}
	}
}


/*! \brief Callback when a new Plot Size is selected.
*/
void ViewPositionWidget::selectedSizeIndexChanged(int)
{
	if (currentViewProperties_.size_ == (DataViewSize::ViewSize)sizeSelection_->currentData().toInt())
	{
		return;
	}

	currentViewProperties_.size_ = (DataViewSize::ViewSize)sizeSelection_->currentData().toInt();

	const int x = currentViewProperties_.x_;
	const int y = currentViewProperties_.y_;
	const int size = (int)currentViewProperties_.size_;

	if ((x + size > VIEWGRIDWIDTH) || (y + size > VIEWGRIDHEIGHT))
	{
		currentViewProperties_.x_ = -1;
		currentViewProperties_.y_ = -1;
	}

	rectifyNewProperties();
	pushChange();
}
