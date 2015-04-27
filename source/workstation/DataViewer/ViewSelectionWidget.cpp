#include <QLabel>

#include "ViewSelectionWidget.h"
#include "DataViewWidget.h"
#include "DataViewLayout.h"
#include "PlotViewWidget.h"

#include "../../common/storage/TaskConfig.h"
#include "../../common/memleakdetect.h"


/*! \brief Constructs a new ViewSelectionWidget
*/
ViewSelectionWidget::ViewSelectionWidget()
	: nextIndex_(1)
{
	selectionLayout_ = new QGridLayout();
	selectionLayout_->addWidget(new QLabel("View Selection"),0,0,1,4, Qt::AlignCenter);

	//Plot Selection Widget
	plotSelection_ = new QComboBox();
	plotSelection_->setSizeAdjustPolicy(QComboBox::AdjustToContents);
	plotSelection_->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Fixed);
	plotSelection_->setInsertPolicy(QComboBox::InsertAtBottom);
	plotSelection_->setEditable(false);

	connect(plotSelection_, SIGNAL(currentIndexChanged(int)), this, SLOT(selectedPlotIndexChanged(int)));

	selectionLayout_->addWidget(new QLabel("Plot"), 1, 0);
	selectionLayout_->addWidget(plotSelection_,1,2,1,2);

	//Size Selection Widget
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

	selectionLayout_->addWidget(new QLabel("Size"),2,0);
	selectionLayout_->addWidget(sizeSelection_,2,2,1,2);

	//Position Selection Widget
	selectionLayout_->addWidget(new QLabel("Position"),3,0);

	QGridLayout *gridLayout = new QGridLayout();

	//Fill grid with checkboxes
	for (int x = 0; x < VIEWGRIDWIDTH; x++)
	{
		for (int y = 0; y < VIEWGRIDHEIGHT; y++)
		{
			positionCheckBoxes_[x][y] = new QCheckBox();
			connect(positionCheckBoxes_[x][y], SIGNAL(clicked(bool)), this, SLOT(checkClicked(bool)));
			coordsMap[positionCheckBoxes_[x][y]] = QSharedPointer<WidgetCoords>(new WidgetCoords(x, y));
			gridLayout->addWidget(positionCheckBoxes_[x][y], x, y);
		}
	}

	selectionLayout_->addLayout(gridLayout,3,2,4,1);

	//Set the Label column to remain a fixed width.  The 1st column also remains a fixed width to prevent weird behavior
	//of the grid of checkboxes, while the third is allowed to stretch to fill the space with the ComboBoxes
	selectionLayout_->setColumnStretch(0, 0);
	selectionLayout_->setColumnStretch(1, 0);
	selectionLayout_->setColumnMinimumWidth(1, 20);
	selectionLayout_->setColumnStretch(2, 0);
	selectionLayout_->setColumnStretch(3, 0);

	setLayout(selectionLayout_);
}

/*! \brief ViewSelectionWidget destructor
*/
ViewSelectionWidget::~ViewSelectionWidget()
{

}

/*! \brief Callback when a new Plot is selected.
*/
void ViewSelectionWidget::selectedPlotIndexChanged(int)
{
	updateSizeAndPositionOptions();
}

/*! \brief Callback when a new Plot Size is selected.
*/
void ViewSelectionWidget::selectedSizeIndexChanged(int)
{
	//The current widget
	DataViewWidget *pCurrentView = widgetIndexMap_[plotSelection_->currentData().toInt()];

	pCurrentView->setCurrentSize((DataViewSize::ViewSize)sizeSelection_->currentData().toInt());

	int x, y;
	pCurrentView->getPosition(x, y);

	if (!isWidgetPosValid(pCurrentView, x, y))
	{
		pCurrentView->setDisplayed(false);
		emit(widgetRemoved(pCurrentView));
		pCurrentView->hide();
	}
	else if (pCurrentView->getDisplayed())
	{
		emit(widgetRemoved(pCurrentView));
		emit(widgetAdded(pCurrentView, x, y, pCurrentView->getCurrentSize()));
	}

	updateSizeAndPositionOptions();
}


/*! \brief Callback helper when a checkbox is clicked.
*/
void ViewSelectionWidget::checkClicked(bool)
{
	QCheckBox *pCheck = (QCheckBox *)QObject::sender();
	if (coordsMap.contains(pCheck))
	{
		checkboxChanged(pCheck->isChecked(), coordsMap[pCheck]->x_, coordsMap[pCheck]->y_);
	}
}

/*! \brief Callback when a checkbox is clicked.  This updates the associated values about the position in the DataViewWidget,
 *	and the signal emitted removes and re-adds the widget to the connected DataViewLayout.  Finally, the current statuses of
 *	the Checkboxes and ComboBoxes are reset to their current values.
 */
void ViewSelectionWidget::checkboxChanged(bool bNewValue, int x, int y)
{
	//The current widget
	DataViewWidget *pCurrentView = widgetIndexMap_[plotSelection_->currentData().toInt()];

	if (bNewValue)
	{
		while (!isWidgetPosValid(pCurrentView, x, y) && pCurrentView->getCurrentSize() > DataViewSize::VIEW_SIZE_1x1)
		{
			pCurrentView->setCurrentSize((DataViewSize::ViewSize)(pCurrentView->getCurrentSize() - 1));
		}

		pCurrentView->setPosition(x, y);
		pCurrentView->setDisplayed(true);
		emit(widgetRemoved(pCurrentView));
		emit(widgetAdded(pCurrentView, x, y, pCurrentView->getCurrentSize()));
		pCurrentView->show();
	}
	else
	{
		pCurrentView->setPosition(0, 0);
		pCurrentView->setDisplayed(false);
		emit(widgetRemoved(pCurrentView));
		pCurrentView->hide();
	}

	updateSizeAndPositionOptions();
}

/*! \brief Adds a new DataViewWidget to the list of widgets to select from.
 *	\note the ViewSelectionWidget will be responsible for deleting the DataViewWidget, but the DataViewWidget will release
 *	control of its owned widget prior to doing so.  Currently, all DataViewWidgets contain externally-managed widgets.
 *	This may create memory issues down the road.
 */
void ViewSelectionWidget::registerView(DataViewWidget *pNewView)
{
	Q_ASSERT(pNewView);
	//IMPORTANT: FIGURE OUT HOW WE WILL BE MANAGING WIDGET MEMORY
	//TO DO TODO !!!!!
	if (!dataViewWidgets_.contains(pNewView))
	{
		dataViewWidgets_.push_back(pNewView);
		widgetIndexMap_[nextIndex_] = pNewView;
		plotSelection_->addItem(pNewView->getName(), nextIndex_);
		++nextIndex_;
	}
}

/*! \brief Adds a new DataViewWidget to the list of widgets to select from.
 *	\note the ViewSelectionWidget will be responsible for deleting the DataViewWidget, but the DataViewWidget will release
 *	control of its owned widget prior to doing so.  Currently, all DataViewWidgets contain externally-managed widgets.
 *	This may create memory issues down the road.
 */
void ViewSelectionWidget::addContainer(QWidget *pNewView)
{
	Q_ASSERT(pNewView);
	
	if (!viewContainerMap_.contains(pNewView))
	{
		DataViewWidget *pNewViewContainer = createDataViewWidget(pNewView);
		viewContainerMap_[pNewView] = pNewViewContainer;
		dataViewWidgets_.push_back(pNewViewContainer);
		widgetIndexMap_[nextIndex_] = pNewViewContainer;
		plotSelection_->addItem(pNewViewContainer->getName(), nextIndex_);
		++nextIndex_;
	}
}

/*! \brief Runs through widgets and updates the Size menu with the current option, as well as position checkboxes.
 */
void ViewSelectionWidget::updateSizeAndPositionOptions()
{
	//Update size combobox to reflect current widget's size 
	DataViewWidget *pCurrentView = nullptr;
	int iNewSizeIndex = 0;

	if (widgetIndexMap_.contains(plotSelection_->currentData().toInt()))
	{
		pCurrentView = widgetIndexMap_[plotSelection_->currentData().toInt()];
		iNewSizeIndex = sizeSelection_->findData(pCurrentView->getCurrentSize());
	}

	sizeSelection_->setCurrentIndex(iNewSizeIndex);

	//Initialize the state of the checkboxes.
	for (int x = 0; x < VIEWGRIDWIDTH; x++)
	{
		for (int y = 0; y < VIEWGRIDHEIGHT; y++)
		{
			positionCheckBoxes_[x][y]->setDisabled(false);
			positionCheckBoxes_[x][y]->setCheckState(Qt::Unchecked);
		}
	}

	//Update position checkboxes to reflect current options
	foreach(DataViewWidget *pWidget, dataViewWidgets_)
	{
		if (pWidget->getDisplayed())
		{
			int xPos, yPos;
			pWidget->getPosition(xPos, yPos);

			if (pWidget != pCurrentView)
			{
				for (int x = xPos; x < xPos + pWidget->getCurrentSize(); x++)
				{
					for (int y = yPos; y < yPos + pWidget->getCurrentSize(); y++)
					{
						Q_ASSERT(x < VIEWGRIDWIDTH);
						Q_ASSERT(y < VIEWGRIDHEIGHT);
						positionCheckBoxes_[x][y]->setDisabled(true);
						positionCheckBoxes_[x][y]->setCheckState(Qt::Unchecked);
					}
				}
			}
			else
			{
				for (int x = xPos; x < xPos + pWidget->getCurrentSize(); x++)
				{
					for (int y = yPos; y < yPos + pWidget->getCurrentSize(); y++)
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
		}
	}

	if (pCurrentView)
	{
		//Disable impossible positions
		for (int x = VIEWGRIDWIDTH - pCurrentView->getCurrentSize() + 1; x < VIEWGRIDWIDTH; x++)
		{
			for (int y = 0; y < VIEWGRIDHEIGHT; y++)
			{
				positionCheckBoxes_[x][y]->setDisabled(true);
			}
		}

		for (int x = 0; x < VIEWGRIDWIDTH; x++)
		{
			for (int y = VIEWGRIDHEIGHT - pCurrentView->getCurrentSize() + 1; y < VIEWGRIDHEIGHT; y++)
			{
				positionCheckBoxes_[x][y]->setDisabled(true);
			}
		}
	}
}

/*! \brief Runs through widgets and checks to see if the new widget size is valid.
 */
bool ViewSelectionWidget::isWidgetPosValid(DataViewWidget *pTestWidget, int testX, int testY)
{
	bool positionFull[VIEWGRIDWIDTH][VIEWGRIDHEIGHT] = { false };

	foreach(DataViewWidget *pWidget, dataViewWidgets_)
	{
		if (pWidget->getDisplayed() && pWidget != pTestWidget)
		{
			int xPos, yPos;
			pWidget->getPosition(xPos, yPos);

			for (int x = xPos; x < xPos + pWidget->getCurrentSize(); x++)
			{
				for (int y = yPos; y < yPos + pWidget->getCurrentSize(); y++)
				{
					Q_ASSERT(x < VIEWGRIDWIDTH);
					Q_ASSERT(y < VIEWGRIDHEIGHT);
					positionFull[x][y] = true;
				}
			}
		}
	}

	for (int x = 0; x < pTestWidget->getCurrentSize(); x++)
	{
		for (int y = 0; y < pTestWidget->getCurrentSize(); y++)
		{

			if (testX + x >= VIEWGRIDWIDTH || testY + y >= VIEWGRIDHEIGHT)
			{
				//The new widget size extends over the borders
				return false;
			}

			if (positionFull[testX + x][testY + y])
			{
				//The new widget size extends over other widget
				return false;
			}
		}
	}

	return true;
}

/*! \brief Connects the Signals of the ViewSelectionWidget with the Slots of the DataViewLayout.
 */
void ViewSelectionWidget::connectToViewerLayout(DataViewLayout *pLayout)
{
	connect(this, SIGNAL(widgetRemoved(QWidget*)), pLayout, SLOT(removeWidgetSlot(QWidget*)));
	connect(this, SIGNAL(widgetAdded(QWidget*, int, int, DataViewSize::ViewSize)), pLayout, SLOT(addWidgetSlot(QWidget*, int, int, DataViewSize::ViewSize)));

}

/*! \brief Connects the Slots of the ViewSelectionWidget with the Signals of the TaskConfig.
*/
void ViewSelectionWidget::connectToTaskConfig(QSharedPointer<TaskConfig> pTaskConfig)
{
	currentTaskConfig_ = pTaskConfig;
	
	connect(currentTaskConfig_.data(), SIGNAL(widgetAddedToMap(QWidget*)), this, SLOT(addWidgetContainer(QWidget*)));
	connect(currentTaskConfig_.data(), SIGNAL(widgetRemovedFromMap(QWidget*)), this, SLOT(removeWidgetContainer(QWidget*)));
}

/*! \brief Update the container for the passed-in widget.  The title is queried from the TaskConfig.
*/
void ViewSelectionWidget::updateWidgetContainer(QWidget *pWidget)
{
	if (viewContainerMap_.contains(pWidget))
	{
		viewContainerMap_[pWidget]->setName(currentTaskConfig_->getName(pWidget));
		
		plotSelection_->setItemText(plotSelection_->findData(widgetIndexMap_.key(viewContainerMap_[pWidget])), currentTaskConfig_->getName(pWidget));
	}
}

/*! \brief Remove the container for the passed-in widget.
*/
void ViewSelectionWidget::removeWidgetContainer(QWidget *pWidget)
{
	//TEST THIS CAREFULLY!
	if (viewContainerMap_.contains(pWidget))
	{
		removeDataViewWidget(viewContainerMap_[pWidget]);
	}
}

/*! \brief Add a new container for the passed-in widget.  The title is queried from the TaskConfig.
*/
void ViewSelectionWidget::addWidgetContainer(QWidget *pWidget)
{
	addContainer(pWidget);
}

/*! \brief Sets a default widget to be shown.
 */
bool ViewSelectionWidget::setDefaultView(DataViewWidget *pDefaultView, int x, int y, DataViewSize::ViewSize eSize)
{
	Q_ASSERT(pDefaultView);

	//Register the view if it's not already
	registerView(pDefaultView);
	pDefaultView->setCurrentSize(eSize);

	//Return false if we can't set the view
	if (!isWidgetPosValid(pDefaultView, x, y))
	{
		return false;
	}

	pDefaultView->setPosition(x, y);
	pDefaultView->setDisplayed(true);
	emit(widgetRemoved(pDefaultView));
	emit(widgetAdded(pDefaultView, x, y, pDefaultView->getCurrentSize()));

	updateSizeAndPositionOptions();

	return true;
}

/*! \brief Clears and reconstructs entire set of views.
 */
void ViewSelectionWidget::rebuild()
{
	clear();
	currentTaskConfig_->requestUpdate();

	QList<QWidget*> newWidgets = currentTaskConfig_->getWidgets();

	foreach(QWidget* pWidget, newWidgets)
	{
		addWidgetContainer(pWidget);
	}
}

/*! \brief Clears all registered Widgets with a RetentionPolicy of Clear.
 */
void ViewSelectionWidget::clear()
{
	QMutableVectorIterator<DataViewWidget *> iter(dataViewWidgets_);
	while (iter.hasNext())
	{
		//Clear all widgets whose RetentionPolicy is not DVW_RETAIN
		if (iter.next()->getRetentionPolicy() != DVW_RETAIN)
		{
			removeDataViewWidget(iter.value(), false);
			iter.remove();
		}
	}
	updateSizeAndPositionOptions();
}

/*! \brief A specialized factory for finding the proper associated DataViewWidget child class for the passed-in widget.
 *	\note Most of the complexity here is an attempt to keep a solid boundary between the libPicto base and the other
 *	projects which include it.  This is meant to expand to support other DataViewWidget types.
 */
DataViewWidget *ViewSelectionWidget::createDataViewWidget(QWidget *pWidget)
{
	return new PlotViewWidget(currentTaskConfig_->getName(pWidget), pWidget, currentTaskConfig_->getAsset(pWidget));
}

/*! \brief Deletes all references to the passed-in widget.
*/
void ViewSelectionWidget::removeDataViewWidget(DataViewWidget *pWidget, bool bRemoveFromVector)
{
	if (pWidget->getDisplayed())
	{
		emit widgetRemoved(pWidget);
		//This probably isn't necessary
		//iter.next()->hide();
	}

	int findKey = widgetIndexMap_.key(pWidget);
	if (findKey)
	{
		plotSelection_->removeItem(plotSelection_->findData(widgetIndexMap_.key(pWidget)));
		plotSelection_->setCurrentIndex(0);
		widgetIndexMap_.remove(findKey);
	}

	QWidget *pFindWidget = viewContainerMap_.key(pWidget);
	if (pFindWidget)
	{
		viewContainerMap_.remove(pFindWidget);
	}

	if (bRemoveFromVector)
	{
		dataViewWidgets_.remove(dataViewWidgets_.indexOf(pWidget));
	}


	delete pWidget;
}
