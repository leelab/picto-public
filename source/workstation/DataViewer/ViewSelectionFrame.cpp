#include <QLabel>
#include <QLineEdit>
#include <QSettings>

#include "ViewSelectionFrame.h"
#include "DataViewWidget.h"
#include "DataViewOrganizer.h"
#include "PlotViewWidget.h"

#include "../../common/storage/TaskConfig.h"
#include "../../common/memleakdetect.h"
#include "../../common/operator/BarBase.h"
#include "../../common/operator/NeuralDataListener.h"

#include "../../common/variable/NumberVariable.h"
#include "../../common/parameter/BooleanParameter.h"
#include "../../common/globals.h"

/*! \brief Constructs a new ViewSelectionFrame
*/
ViewSelectionFrame::ViewSelectionFrame(bool replay)
	: nextIndex_(1), pVisualTargetHost_(nullptr), NoCallBack_(false), customAnalysisIndex_(0), activatePlots_(false)
{
	selectionLayout_ = new QGridLayout();

	neuronsSelWidget_ = new QWidget();
	typeSelWidget_ = new QWidget();
	flagSelWidget_ = new QWidget();
	//selectionLayout_->addWidget(new QLabel("View Selection"), 0, 0, Qt::AlignCenter);

	if (replay)
	{
		QGridLayout* SelLayout = new QGridLayout();

		//Pre/Post Flags
		QGridLayout* lFlagSel = new QGridLayout();

		saveSettings_ = new QPushButton("Save Selection");
		recoverSettings_ = new QPushButton("Recover Selection");
		saveSettings_->setEnabled(false);
		recoverSettings_->setEnabled(false);

		connect(saveSettings_, SIGNAL(clicked(bool)), this, SLOT(writeSettings()));
		connect(recoverSettings_, SIGNAL(clicked(bool)), this, SLOT(readSettings()));

		lFlagSel->addWidget(saveSettings_, 0, 0);
		lFlagSel->addWidget(recoverSettings_, 0, 1);

		preflagEdit_ = new lblEdit("PreFlagWindow", "0");
		postflagEdit_ = new lblEdit("PostFlagWindow", "6000");
		preflagEdit_->setEnabled(false);
		postflagEdit_->setEnabled(false);
		lFlagSel->addWidget(preflagEdit_, 1, 0);
		lFlagSel->addWidget(postflagEdit_, 1, 1);

		flagSelWidget_->setLayout(lFlagSel);
		selectionLayout_->addWidget(flagSelWidget_, 0, 0, Qt::AlignCenter);

		//Plots		
		SelLayout->addWidget(new QLabel("Neurons"), 0, 0, 1, 1, Qt::AlignCenter);
		SelLayout->addWidget(new QLabel("Events"), 0, 1, 1, 1, Qt::AlignCenter);
		SelLayout->addWidget(new QLabel("Alignment"), 0, 2, 1, 1, Qt::AlignCenter);

		//Levels Selection Widget - Neurons
		levelsSelectionNeur_ = new QListWidget();
		//levelsSelectionNeur_->setSortingEnabled(true);
		SelLayout->addWidget(levelsSelectionNeur_, 1, 0, 1, 1);
		levelsSelectionNeur_->setEnabled(false);
		connect(levelsSelectionNeur_, SIGNAL(itemChanged(QListWidgetItem*)), this, SLOT(selectedLevelIndexChangedNeur()));

		//Levels Selection Widget - Events
		levelsSelectionHor_ = new QListWidget();
		//levelsSelectionHor_->setSortingEnabled(true);
		SelLayout->addWidget(levelsSelectionHor_, 1, 1, 1, 1);
		levelsSelectionHor_->setEnabled(false);
		connect(levelsSelectionHor_, SIGNAL(itemChanged(QListWidgetItem*)), this, SLOT(selectedLevelIndexChangedHor()));

		//Alignment Selection Widget
		levelsSelectionAl_ = new QListWidget();
		SelLayout->addWidget(levelsSelectionAl_, 1, 2, 1, 1);
		levelsSelectionAl_->setEnabled(false);
		connect(levelsSelectionAl_, SIGNAL(itemChanged(QListWidgetItem*)), this, SLOT(selectedLevelIndexChangedAl()));

		//Markers Selection Widget
		SelLayout->addWidget(new QLabel("Markers"), 2, 0, 1, 1, Qt::AlignCenter);
		//Scriptables Selection Widget
		SelLayout->addWidget(new QLabel("Conditions"), 2, 1, 1, 1, Qt::AlignCenter);
		//Values (for Scriptables) Selection Widget
		SelLayout->addWidget(new QLabel("Values"), 2, 2, 1, 1, Qt::AlignCenter);		
	

		//Markers Selection Widget
		levelsSelectionMark_ = new QListWidget();
		SelLayout->addWidget(levelsSelectionMark_, 3, 0, 1, 1);
		levelsSelectionMark_->setEnabled(false);

		connect(levelsSelectionMark_, SIGNAL(itemChanged(QListWidgetItem*)), this, SLOT(selectedLevelIndexChangedMark()));
		neuronsSelWidget_->setLayout(SelLayout);
		selectionLayout_->addWidget(neuronsSelWidget_, 1, 0, Qt::AlignCenter);


		//Scriptables Selection Widget
		levelsSelectionSc_ = new QListWidget();
		SelLayout->addWidget(levelsSelectionSc_, 3, 1, 1, 1);
		levelsSelectionSc_->setEnabled(false);

		connect(levelsSelectionSc_, SIGNAL(itemChanged(QListWidgetItem*)), this, SLOT(selectedLevelIndexChangedSc()));

		//Values Selection Widget
		levelsSelectionVal_ = new QListWidget();
		SelLayout->addWidget(levelsSelectionVal_, 3, 2, 1, 1);
		levelsSelectionVal_->setEnabled(false);
		
	

		//Plot Selection Widget	
		QGridLayout* lTypeSel = new QGridLayout();
		lTypeSel->addWidget(new QLabel("Type    "), 0, 0);

		plotType_ = new QComboBox();
		plotType_->setSizeAdjustPolicy(QComboBox::AdjustToContents);
		//plotType_->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Fixed);
		plotType_->setInsertPolicy(QComboBox::InsertAtBottom);
		plotType_->setEditable(false);

		plotType_->addItem("Select");
		plotType_->addItem("PSTH Plot");
		plotType_->addItem("Raster Plot");
		plotType_->setCurrentIndex(0);
		plotType_->setEnabled(false);

		connect(plotType_, SIGNAL(currentIndexChanged(int)), this, SLOT(selectedPlotTypeChanged(int)));

		lTypeSel->addWidget(plotType_, 0, 1, 1, 1);

		lTypeSel->setColumnStretch(0, 0);
		lTypeSel->setColumnStretch(1, 0);
		//lTypeSel->setColumnMinimumWidth(1, 20);
		//lTypeSel->setColumnStretch(2, 0);
		//lTypeSel->setColumnStretch(3, 0);

		lTypeSel->setContentsMargins(11, 0, 0, 0);

		typeSelWidget_->setLayout(lTypeSel);
		selectionLayout_->addWidget(typeSelWidget_, 2, 0, Qt::AlignCenter);

		updateUI(false);
	}
	//end Plots

	//Plot Selection Widget
	QWidget* wSizeAndPos = new QWidget();
	QGridLayout* lSizeAndPos = new QGridLayout();
	lSizeAndPos->addWidget(new QLabel("Plot"), 0, 0);

	plotSelection_ = new QComboBox();
	plotSelection_->setSizeAdjustPolicy(QComboBox::AdjustToContents);
	//plotSelection_->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Fixed);
	plotSelection_->setInsertPolicy(QComboBox::InsertAtBottom);
	plotSelection_->setEditable(false);
	if (activatePlots_)
		plotSelection_->setEnabled(false);

	connect(plotSelection_, SIGNAL(currentIndexChanged(int)), this, SLOT(selectedPlotIndexChanged(int)));

	lSizeAndPos->addWidget(plotSelection_, 0, 1, 1, 1);

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

	lSizeAndPos->addWidget(new QLabel("Size"), 1, 0);
	lSizeAndPos->addWidget(sizeSelection_, 1, 1, 1, 1);

	//Position Selection Widget
	lSizeAndPos->addWidget(new QLabel("Position"), 2, 0);

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

	lSizeAndPos->addLayout(gridLayout, 2, 1, 4, 1);

	//Set the Label column to remain a fixed width.  The 1st column also remains a fixed width to prevent weird behavior
	//of the grid of checkboxes, while the third is allowed to stretch to fill the space with the ComboBoxes
	lSizeAndPos->setColumnStretch(0, 0);
	lSizeAndPos->setColumnStretch(1, 0);
	lSizeAndPos->setColumnMinimumWidth(1, 20);
	lSizeAndPos->setColumnStretch(2, 0);
	lSizeAndPos->setColumnStretch(3, 0);

	wSizeAndPos->setLayout(lSizeAndPos);
	selectionLayout_->addWidget(wSizeAndPos, 4, 0, Qt::AlignCenter);

	setLayout(selectionLayout_);


}

/*! \brief ViewSelectionFrame destructor
*/
ViewSelectionFrame::~ViewSelectionFrame()
{

}

/*! \brief Callback when a new Plot is selected.
*/
void ViewSelectionFrame::selectedPlotIndexChanged(int)
{
	updateSizeAndPositionOptions();
}

/*! \brief Callback when a new Plot Size is selected.
*/
void ViewSelectionFrame::selectedSizeIndexChanged(int)
{
	//The current widget
	DataViewWidget *pCurrentView = nullptr;

	if (widgetIndexMap_.contains(plotSelection_->currentData().toInt()))
	{
		pCurrentView = widgetIndexMap_[plotSelection_->currentData().toInt()];
	}

	if (!pCurrentView)
	{
		return;
	}

	if (activatePlots_)
	{
		//Plots
		PlotViewWidget* pPlotWidget = (PlotViewWidget*)pCurrentView;
		if (pPlotWidget)
		{
			bool sameTabAsTask = pPlotWidget->tabInfo();
			if (!sameTabAsTask)
				return;
		}
		//end Plots
	}

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
void ViewSelectionFrame::checkClicked(bool)
{
	QCheckBox *pCheck = (QCheckBox *)QObject::sender();
	if (coordsMap.contains(pCheck))
	{
		checkboxChanged(pCheck->isChecked(), coordsMap[pCheck]->x_, coordsMap[pCheck]->y_);
	}
}

/*! \brief Callback when a checkbox is clicked.  This updates the associated values about the position in the
*	DataViewWidget, and the signal emitted removes and re-adds the widget to the connected DataViewOrganizer.
*	Finally, the current statuses of the Checkboxes and ComboBoxes are reset to their current values.
*/
void ViewSelectionFrame::checkboxChanged(bool bNewValue, int x, int y)
{
	//The current widget
	DataViewWidget *pCurrentView = widgetIndexMap_[plotSelection_->currentData().toInt()];

	//for Plots
	if (activatePlots_)
	{
		PlotViewWidget* pPlotWidget = (PlotViewWidget*)pCurrentView;
		if (pPlotWidget)
		{
			bool sameTabAsTask = pPlotWidget->tabInfo();
			if (!sameTabAsTask)
				return;
		}
	}
	//end Plots

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
*	\note the ViewSelectionFrame will be responsible for deleting the DataViewWidget, but the DataViewWidget will release
*	control of its owned widget prior to doing so.  Currently, all DataViewWidgets contain externally-managed widgets.
*	This may create memory issues down the road.
*/
void ViewSelectionFrame::registerView(DataViewWidget *pNewView)
{
	Q_ASSERT(pNewView);

	if (!dataViewWidgets_.contains(pNewView))
	{
		dataViewWidgets_.push_back(pNewView);
		widgetIndexMap_[nextIndex_] = pNewView;
		plotSelection_->addItem(pNewView->getName(), nextIndex_);
		++nextIndex_;
	}
}

/*! \brief Adds a new DataViewWidget to the list of widgets to select from.
*	\note the ViewSelectionFrame will be responsible for deleting the DataViewWidget, but the DataViewWidget will release
*	control of its owned widget prior to doing so.  Currently, all DataViewWidgets contain externally-managed widgets.
*	This may create memory issues down the road.
*/
void ViewSelectionFrame::addContainer(QWidget *pNewView, bool signal)
{
	Q_ASSERT(pNewView);

	if (!viewContainerMap_.contains(pNewView))
	{
		DataViewWidget *pNewViewContainer = createDataViewWidget(pNewView);
		if (activatePlots_) {
			//for Plots		
			connect(pNewViewContainer, SIGNAL(openInNewTab(QWidget*)), this, SIGNAL(openInNewTab(QWidget*)));
			//end Plots
		}
		viewContainerMap_[pNewView] = pNewViewContainer;
		dataViewWidgets_.push_back(pNewViewContainer);
		widgetIndexMap_[nextIndex_] = pNewViewContainer;
		plotSelection_->addItem(pNewViewContainer->getName(), nextIndex_);
		++nextIndex_;

		const int iConfigIndex = configIndex(currentTaskConfig_);
		if (cachedViewSetup_[iConfigIndex].contains(pNewView))
		{
			//Set the properties on the DataViewWidget
			pNewViewContainer->setPosition(
				cachedViewSetup_[iConfigIndex][pNewView].x_,
				cachedViewSetup_[iConfigIndex][pNewView].y_);
			pNewViewContainer->setCurrentSize(cachedViewSetup_[iConfigIndex][pNewView].size_);
			pNewViewContainer->setDisplayed(true);

			emit widgetAdded(pNewViewContainer,
				cachedViewSetup_[iConfigIndex][pNewView].x_,
				cachedViewSetup_[iConfigIndex][pNewView].y_,
				cachedViewSetup_[iConfigIndex][pNewView].size_);

			cachedViewSetup_[iConfigIndex].remove(pNewView);
			updateSizeAndPositionOptions();
		}
		else if (signal)
		{
			tryDefault(pNewView);
		}
	}
}

void ViewSelectionFrame::tryDefault(QWidget *pNewView)
{
	DataViewWidget *pViewContainer = viewContainerMap_[pNewView];

	if (!pViewContainer->getDisplayed())
	{
		int x, y;
		pViewContainer->getPosition(x, y);
		if (x > -1 && y > -1)
		{
			if (isWidgetPosValid(pViewContainer, x, y))
			{
				pViewContainer->setDisplayed(true);
				emit widgetAdded(pViewContainer, x, y, pViewContainer->getCurrentSize());
				updateSizeAndPositionOptions();
			}
			else
			{
				pViewContainer->setPosition(0, 0);
			}
		}
	}
}

/*! \brief Runs through widgets and updates the Size menu with the current option, as well as position checkboxes.
*/
void ViewSelectionFrame::updateSizeAndPositionOptions()
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

	if (activatePlots_)
	{
		//for Plots
		//Disable the checkboxes.
		PlotViewWidget* pPlotWidget = (PlotViewWidget*)pCurrentView;
		if (pPlotWidget)
		{
			bool sameTabAsTask = pPlotWidget->tabInfo();
			if (!sameTabAsTask)
			{
				for (int x = 0; x < VIEWGRIDWIDTH; x++)
				{
					for (int y = 0; y < VIEWGRIDHEIGHT; y++)
					{
						positionCheckBoxes_[x][y]->setCheckState(Qt::Unchecked);
						positionCheckBoxes_[x][y]->setDisabled(true);
					}
				}
				return;
			}
		}
	}
	//end Plots

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
			if (activatePlots_) {
				//for Plots
				PlotViewWidget* pPlotWidget = (PlotViewWidget*)pCurrentView;
				if (pPlotWidget)
				{
					bool sameTabAsTask = pPlotWidget->tabInfo();
					if (!sameTabAsTask)
						break;
				}
				//end Plots
			}
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
bool ViewSelectionFrame::isWidgetPosValid(DataViewWidget *pTestWidget, int testX, int testY)
{
	bool positionFull[VIEWGRIDWIDTH][VIEWGRIDHEIGHT] = { false };

	if (activatePlots_) {
		//for Plots
		PlotViewWidget* pPlotWidget = (PlotViewWidget*)pTestWidget;
		if (pPlotWidget)
		{
			bool sameTabAsTask = pPlotWidget->tabInfo();
			if (!sameTabAsTask)
				return true;
		}
		//end Plots
	}

	foreach(DataViewWidget *pWidget, dataViewWidgets_)
	{
		if (pWidget->getDisplayed() && pWidget != pTestWidget)
		{
			if (activatePlots_) {
				//for Plots
				PlotViewWidget* pPWidget = (PlotViewWidget*)pWidget;
				if (pPWidget)
				{
					bool sameTabAsTask = pPWidget->tabInfo();
					if (!sameTabAsTask)
						break;
				}
				//end Plots
			}
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

/*! \brief Connects the Signals of the ViewSelectionFrame with the Slots of the DataViewOrganizer.
*/
void ViewSelectionFrame::connectToViewerLayout(DataViewOrganizer *pLayout)
{
	connect(this, SIGNAL(widgetRemoved(QWidget*)), pLayout, SLOT(removeWidgetSlot(QWidget*)));
	connect(this, SIGNAL(widgetAdded(QWidget*, int, int, DataViewSize::ViewSize)), pLayout, SLOT(addWidgetSlot(QWidget*, int, int, DataViewSize::ViewSize)));

	//Drag and Drop
	//for Plots	
	connect(pLayout, SIGNAL(widgetDropped(QPoint)), this, SLOT(widgetDropped(QPoint)));
	connect(pLayout, SIGNAL(widgetDragged(QWidget*)), this, SLOT(widgetDragged(QWidget*)));
	//end Plots

}

/*! \brief Connects the Slots of the ViewSelectionFrame with the Signals of the TaskConfig.
*/
void ViewSelectionFrame::connectToTaskConfig(QSharedPointer<TaskConfig> pTaskConfig)
{
	if (currentTaskConfig_ == pTaskConfig)
		return;

	if (currentTaskConfig_)
	{
		emit managerConnectionEstablished(false);

		disconnect(currentTaskConfig_.data(), SIGNAL(widgetAddedToMap(QWidget*)), this, SLOT(addWidgetContainer(QWidget*)));
		disconnect(currentTaskConfig_.data(), SIGNAL(widgetRemovedFromMap(QWidget*)), this, SLOT(removeWidgetContainer(QWidget*)));
		disconnect(currentTaskConfig_.data(), SIGNAL(plotHandlerRequested(const QString)),
			this, SLOT(plotHandlerRequested(const QString)));
		disconnect(currentTaskConfig_.data(), SIGNAL(requestCachePlotHandler(QSharedPointer<OperatorPlotHandler>, const QString)),
			this, SLOT(cachePlotHandler(QSharedPointer<OperatorPlotHandler>, const QString)));
		disconnect(currentTaskConfig_.data(), SIGNAL(requestClearPlotHandlers()),
			this, SLOT(clearPlotHandlers()));
		disconnect(this, SIGNAL(returnPlotHandler(QSharedPointer<OperatorPlotHandler>, const QString)),
			currentTaskConfig_.data(), SLOT(receivePlotHandler(QSharedPointer<OperatorPlotHandler>, const QString)));
		disconnect(this, SIGNAL(managerConnectionEstablished(bool)),
			currentTaskConfig_.data(), SLOT(managerConnectionEstablished(bool)));
	}
	lastTaskConfig_ = currentTaskConfig_.toWeakRef();
	currentTaskConfig_ = pTaskConfig;

	connect(currentTaskConfig_.data(), SIGNAL(widgetAddedToMap(QWidget*)), this, SLOT(addWidgetContainer(QWidget*)));
	connect(currentTaskConfig_.data(), SIGNAL(widgetRemovedFromMap(QWidget*)), this, SLOT(removeWidgetContainer(QWidget*)));
	connect(currentTaskConfig_.data(), SIGNAL(plotHandlerRequested(const QString)),
		this, SLOT(plotHandlerRequested(const QString)));
	connect(currentTaskConfig_.data(), SIGNAL(requestCachePlotHandler(QSharedPointer<OperatorPlotHandler>, const QString)),
		this, SLOT(cachePlotHandler(QSharedPointer<OperatorPlotHandler>, const QString)));
	connect(currentTaskConfig_.data(), SIGNAL(requestClearPlotHandlers()),
		this, SLOT(clearPlotHandlers()));
	connect(this, SIGNAL(returnPlotHandler(QSharedPointer<OperatorPlotHandler>, const QString)),
		currentTaskConfig_.data(), SLOT(receivePlotHandler(QSharedPointer<OperatorPlotHandler>, const QString)));
	connect(this, SIGNAL(managerConnectionEstablished(bool)),
		currentTaskConfig_.data(), SLOT(managerConnectionEstablished(bool)));

	emit managerConnectionEstablished(true);

	QStringList keys = localAnalysisDisplayState.keys();
	QList<bool> selections = localAnalysisDisplayState.values();
	for (int i = 0; i < keys.count(); i++)
	{
		currentTaskConfig_->notifyAnalysisSelection(keys[i], selections[i]);
	}

	keys = importedAnalysisDisplayState.keys();
	selections = importedAnalysisDisplayState.values();
	for (int i = 0; i < keys.count(); i++)
	{
		currentTaskConfig_->notifyAnalysisSelection(keys[i], selections[i]);
	}

	QList<QWeakPointer<TaskConfig>>::iterator iter = cachedConfigList_.begin();
	while (iter != cachedConfigList_.end())
	{
		if ((*iter).isNull())
		{
			cachedViewSetup_.removeAt(cachedConfigList_.indexOf(*iter));
			iter = cachedConfigList_.erase(iter);
		}
		else
		{
			iter++;
		}
	}
}

//! Slot to receive information about which analyses are currently active.
void ViewSelectionFrame::notifyAnalysisSelection(const QString &name, bool selected, bool local)
{
	QMap<QString, bool> *currentMap = nullptr;

	if (local)
	{
		currentMap = &localAnalysisDisplayState;
	}
	else
	{
		currentMap = &importedAnalysisDisplayState;
	}

	if (currentMap->contains(name) && (*currentMap)[name] == selected)
		return;

	(*currentMap)[name] = selected;

	if (currentTaskConfig_)
	{
		currentTaskConfig_->notifyAnalysisSelection(name, selected);
	}

	updateSizeAndPositionOptions();
}

//! Signal from AnalysisSelector to clear a subset of our cached analysis display states
void ViewSelectionFrame::clearAnalysisSelection(bool local)
{
	if (local)
	{
		localAnalysisDisplayState.clear();
	}
	else
	{
		importedAnalysisDisplayState.clear();
	}
}


/*! \brief Update the container for the passed-in widget.  The title is queried from the TaskConfig.
*/
void ViewSelectionFrame::updateWidgetContainer(QWidget *pWidget)
{
	if (viewContainerMap_.contains(pWidget))
	{
		viewContainerMap_[pWidget]->setName(currentTaskConfig_->getName(pWidget));

		plotSelection_->setItemText(plotSelection_->findData(widgetIndexMap_.key(viewContainerMap_[pWidget])), currentTaskConfig_->getName(pWidget));
	}
}

/*! \brief Remove the container for the passed-in widget.
*/
void ViewSelectionFrame::removeWidgetContainer(QWidget *pWidget)
{
	if (viewContainerMap_.contains(pWidget))
	{
		removeDataViewWidget(viewContainerMap_[pWidget]);
	}
}

/*! \brief Add a new container for the passed-in widget.  The title is queried from the TaskConfig.
*/
void ViewSelectionFrame::addWidgetContainer(QWidget *pWidget)
{
	addContainer(pWidget);
}

void ViewSelectionFrame::setVisualTargetHost(QWidget *pTarget)
{
	pVisualTargetHost_ = pTarget;
}

/*! \brief Clears and reconstructs entire set of views.
*/
void ViewSelectionFrame::rebuild()
{
	if (!lastTaskConfig_.isNull())
	{
		const int iConfigIndex = configIndex(lastTaskConfig_);
		cachedViewSetup_[iConfigIndex].clear();

		QList<QWidget*> widgets = viewContainerMap_.keys();
		QList<DataViewWidget*> viewWidgets = viewContainerMap_.values();

		for (int i = 0; i < widgets.count(); i++)
		{
			if (viewWidgets[i]->getDisplayed())
			{
				int x, y;
				viewWidgets[i]->getPosition(x, y);
				cachedViewSetup_[iConfigIndex][widgets[i]] = std::move(ViewProperties(viewWidgets[i]->getCurrentSize(), x, y));
			}
		}
	}

	lastTaskConfig_ = currentTaskConfig_;

	clear();

	if (pVisualTargetHost_)
	{
		addContainer(pVisualTargetHost_, false);
	}

	if (currentTaskConfig_)
	{
		currentTaskConfig_->requestUpdate();

		QList<QWidget*> newWidgets = currentTaskConfig_->getWidgets();

		foreach(QWidget* pWidget, newWidgets)
		{
			addContainer(pWidget, false);
		}

		foreach(QWidget* pWidget, newWidgets)
		{
			tryDefault(pWidget);
		}
	}
}

/*! \brief Clears all registered Widgets with a RetentionPolicy of Clear.
*/
void ViewSelectionFrame::clear()
{
	QMutableVectorIterator<DataViewWidget *> iter(dataViewWidgets_);
	while (iter.hasNext())
	{
		//Clear all widgets whose RetentionPolicy is not DVW_RETAIN
		if (iter.next()->getRetentionPolicy() != DVW_RETAIN)
		{
			//We pass false as bRemoveFromVector in order to allow us to call iter.remove().  Otherwise we would have
			//	screwed up the iteration.
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
DataViewWidget *ViewSelectionFrame::createDataViewWidget(QWidget *pWidget)
{
	DataViewWidget *newViewWidget = nullptr;
	if (pWidget == pVisualTargetHost_)
	{
		newViewWidget = new DataViewWidget("Task", pWidget);
		ViewProperties props(DataViewSize::VIEW_SIZE_4x4, 0, 0);
		if (currentTaskConfig_)
		{
			props = currentTaskConfig_->getTaskViewProperties();
		}

		int iConfigIndex = configIndex(currentTaskConfig_);

		if (!cachedViewSetup_[iConfigIndex].contains(pWidget))
		{
			cachedViewSetup_[iConfigIndex][pWidget] = std::move(props);
		}
	}
	else
	{
		newViewWidget = new PlotViewWidget(currentTaskConfig_->getName(pWidget),
			pWidget,
			currentTaskConfig_->getDisplayWidgetProperties(pWidget));

		newViewWidget->setName(currentTaskConfig_->getName(pWidget));
	}

	return newViewWidget;
}

//! Deletes all references to the passed-in widget.
void ViewSelectionFrame::removeDataViewWidget(DataViewWidget *pWidget, bool bRemoveFromVector)
{
	if (pWidget->getDisplayed())
	{
		emit widgetRemoved(pWidget);
		updateSizeAndPositionOptions();
	}

	int findKey = widgetIndexMap_.key(pWidget);
	if (findKey)
	{
		plotSelection_->removeItem(plotSelection_->findData(widgetIndexMap_.key(pWidget)));
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

//! Incoming signal from TaskConfig, indicating an OperatorPlot has requested its PlotHandler
void ViewSelectionFrame::plotHandlerRequested(const QString plotPath)
{
	if (cachedHandlers_.contains(plotPath))
	{
		emit returnPlotHandler(cachedHandlers_[plotPath], plotPath);
	}
	else
	{
		emit returnPlotHandler(QSharedPointer<OperatorPlotHandler>(), plotPath);
	}
}

//! Incoming signal from TaskConfig, indicating an OperatorPlot has created a new PlotHandler
void ViewSelectionFrame::cachePlotHandler(QSharedPointer<OperatorPlotHandler> handler, const QString plotPath)
{
	cachedHandlers_[plotPath] = handler;
}

//! Incoming signal from TaskConfig, incidating the PlotHandler Cache should be cleared
void ViewSelectionFrame::clearPlotHandlers()
{
	if (cachedHandlers_.size())
		cachedHandlers_.clear();
}

//! Returns the index of the TaskConfig's cached ViewProperties, or creates one if it doesn't exist already.
int ViewSelectionFrame::configIndex(QWeakPointer<TaskConfig> referenceConfig)
{
	if (!cachedConfigList_.contains(referenceConfig))
	{
		cachedConfigList_ << referenceConfig;
		cachedViewSetup_ << QHash<QWidget*, ViewProperties>();
	}
	return cachedConfigList_.indexOf(referenceConfig);
}


//! Adds Channel/Unit to the UI Spikes list - Remote live mode
void ViewSelectionFrame::spikeAdded(int channel, int unit)
{
	if (!activatePlots_)
		return;

	if (NoCallBack_)
		return;

	NoCallBack_ = true;

	//should update the ui combo lists here based on incoming spikes - remote/test mode only
	QPair<int, int> chUnit(channel, unit);
	bool neuronsAdded = false;
	if (!spikesList_.contains(chUnit))
	{
		neuronsAdded = true;
		spikesList_.append(chUnit);

		QString assetPath = "Channel " + QString::number(chUnit.first) + " - Unit " + QString::number(chUnit.second);
		//Add event to Horizontal list
		QListWidgetItem *newEvent = new QListWidgetItem(assetPath, levelsSelectionNeur_);
		newEvent->setCheckState(Qt::Unchecked);
		levelsSelectionNeur_->addItem(newEvent);
	}
	if (neuronsAdded)
	{
		levelsSelectionNeur_->setEnabled(true);
		levelsSelectionNeur_->sortItems(Qt::AscendingOrder);
		saveSettings_->setEnabled(true);
		recoverSettings_->setEnabled(true);
		preflagEdit_->setEnabled(true);
		postflagEdit_->setEnabled(true);
	}

	NoCallBack_ = false;
}
void ViewSelectionFrame::resetDesignRoot(QSharedPointer<DesignRoot> designRoot)
{
	if (!activatePlots_)
		return;

	if (!designRoot_)
		designRoot_ = designRoot;
	else if (designRoot_ != designRoot)
	{
		designRoot_ = designRoot;
	}
}
//Preloads alignment events, elements and spikes info from experiment/session file and adds them to the UI list
void ViewSelectionFrame::preloadEvents(QSharedPointer<DesignRoot> designRoot, QMap<int, QList<int>> alignElements, QList<QPair<int, int>> spikes)
{
	if (!activatePlots_)
		return;

	if (NoCallBack_)
		return;

	resetPlotsList(1);

	NoCallBack_ = true;

	resetDesignRoot(designRoot);

	alignElements_ = alignElements;
	spikesList_ = spikes;

	//Display list of events
	foreach(int assetID, alignElements.keys())
	{
		if (designRoot &&  designRoot->getExperiment()->getDesignConfig())
		{
			QSharedPointer<Asset> asset = designRoot->getExperiment()->getDesignConfig()->getAsset(assetID);
			if (asset)
			{
				QString assetPath = QString::number(assetID) + " - " + asset->getPath().split("::").last();
				if (assetPath != "")
				{
					//Add event to Horizontal list
					QListWidgetItem *newEvent = new QListWidgetItem(assetPath, levelsSelectionAl_);
					newEvent->setCheckState(Qt::Unchecked);
					levelsSelectionAl_->addItem(newEvent);
					NoCallBack_ = true;

					//Add event to Horizontal list
					QListWidgetItem *newHorEvent = new QListWidgetItem(assetPath, levelsSelectionHor_);
					newHorEvent->setCheckState(Qt::Unchecked);
					levelsSelectionHor_->addItem(newHorEvent);
					levelsSelectionHor_->setEnabled(true);
					NoCallBack_ = true;

					//Add event to Markers list
					QListWidgetItem *newMarkEvent = new QListWidgetItem(assetPath, levelsSelectionMark_);
					newMarkEvent->setCheckState(Qt::Unchecked);
					levelsSelectionMark_->addItem(newMarkEvent);
					NoCallBack_ = true;
				}
			}
		}		
	}

	//Display list of Spikes channel/units
	bool neuronsAdded = false;
	for (int i = 0; i < spikes.count(); i++)
	{
		neuronsAdded = true;
		QPair<int, int> chUnit = spikes[i];
		QString assetPath = "Channel " + QString::number(chUnit.first) + " - Unit " + QString::number(chUnit.second);
		//Add event to Horizontal list
		QListWidgetItem *newEvent = new QListWidgetItem(assetPath, levelsSelectionNeur_);
		newEvent->setCheckState(Qt::Unchecked);
		levelsSelectionNeur_->addItem(newEvent);
		NoCallBack_ = true;
	}
	if (neuronsAdded)
	{
		levelsSelectionNeur_->setEnabled(true);
		//levelsSelectionNeur_->sortItems(Qt::AscendingOrder);	
	}	
	saveSettings_->setEnabled(true);
	recoverSettings_->setEnabled(true);
	preflagEdit_->setEnabled(true);
	postflagEdit_->setEnabled(true);

	NoCallBack_ = false;
}/*! \brief Callback when a new Level is selected.
*/
void ViewSelectionFrame::selectedLevelIndexChangedNeur()
{
	if (NoCallBack_)
		return;

	resetPlotsList();
	NoCallBack_ = true;
	levelsSelectionAl_->setEnabled(true);
	levelsSelectionSc_->setEnabled(true);
	levelsSelectionVal_->setEnabled(true);
	levelsSelectionMark_->setEnabled(true);
	preflagEdit_->setEnabled(true);
	postflagEdit_->setEnabled(true);
	saveSettings_->setEnabled(true);
	recoverSettings_->setEnabled(true);

	for (int i = 0; i < levelsSelectionNeur_->count(); ++i)
	{
		//reset Align Parameter
		QListWidgetItem* pWidgetItem = levelsSelectionNeur_->item(i);
		if (pWidgetItem)
		{
			QString widgetName = pWidgetItem->text();
			int n = widgetName.indexOf(" - ");
			if (n > -1)
			{
				int AssetId = (widgetName.left(n)).toInt();
				if (designRoot_ &&  designRoot_->getExperiment()->getDesignConfig())
				{
					QSharedPointer<Asset> asset = designRoot_->getExperiment()->getDesignConfig()->getAsset(AssetId);
					if (asset && asset.objectCast<StateMachineElement>())
					{
						asset.objectCast<StateMachineElement>()->setSelEventParam(false);
					}
				}
			}
		}
	}
	//Update selection parameter
	for (int i = 0; i < levelsSelectionNeur_->count(); ++i)
	{
		QListWidgetItem* pWidgetItem = levelsSelectionNeur_->item(i);
		if (pWidgetItem && pWidgetItem->checkState() == Qt::Checked)
		{
			QString widgetName = pWidgetItem->text();
			int n = widgetName.indexOf(" - ");
			if (n > -1)
			{
				//1. set as sel element
				int AssetId = (widgetName.left(n)).toInt();
				if (designRoot_ &&  designRoot_->getExperiment()->getDesignConfig())
				{
					QSharedPointer<Asset> alignToAsset = designRoot_->getExperiment()->getDesignConfig()->getAsset(AssetId);
					if (alignToAsset && alignToAsset.objectCast<StateMachineElement>())
						alignToAsset.objectCast<StateMachineElement>()->setSelEventParam(true);		
				}
			}
		}
	}
	
	NoCallBack_ = false;
	updateSizeAndPositionOptions();
}
void ViewSelectionFrame::selectedLevelIndexChangedHor()
{
	if (NoCallBack_)
		return;

	resetPlotsList();
	NoCallBack_ = true;
	levelsSelectionAl_->setEnabled(true);
	levelsSelectionSc_->setEnabled(true);
	levelsSelectionVal_->setEnabled(true);
	levelsSelectionMark_->setEnabled(true);
	preflagEdit_->setEnabled(true);
	postflagEdit_->setEnabled(true);
	saveSettings_->setEnabled(true);
	recoverSettings_->setEnabled(true);

	for (int i = 0; i < levelsSelectionHor_->count(); ++i)
	{
		//reset selection Parameter
		QListWidgetItem* pWidgetItem = levelsSelectionHor_->item(i);
		if (pWidgetItem)
		{
			QString widgetName = pWidgetItem->text();
			int n = widgetName.indexOf(" - ");
			if (n > -1)
			{
				int AssetId = (widgetName.left(n)).toInt();
				if (designRoot_ &&  designRoot_->getExperiment()->getDesignConfig())
				{
					QSharedPointer<Asset> asset = designRoot_->getExperiment()->getDesignConfig()->getAsset(AssetId);
					if (asset && asset.objectCast<StateMachineElement>())
					{
						asset.objectCast<StateMachineElement>()->setSelEventParam(false);
					}
				}
			}
		}
	}
	//Update selection parameter
	for (int i = 0; i < levelsSelectionHor_->count(); ++i)
	{
		QListWidgetItem* pWidgetItem = levelsSelectionHor_->item(i);
		if (pWidgetItem && pWidgetItem->checkState() == Qt::Checked)
		{
			QString widgetName = pWidgetItem->text();
			int n = widgetName.indexOf(" - ");
			if (n > -1)
			{
				//1. set as sel element
				int AssetId = (widgetName.left(n)).toInt();
				if (designRoot_ &&  designRoot_->getExperiment()->getDesignConfig())
				{
					QSharedPointer<Asset> alignToAsset = designRoot_->getExperiment()->getDesignConfig()->getAsset(AssetId);
					if (alignToAsset && alignToAsset.objectCast<StateMachineElement>())
						alignToAsset.objectCast<StateMachineElement>()->setSelEventParam(true);
				}
			}
		}
	}

	NoCallBack_ = false;
	updateSizeAndPositionOptions();
}
void ViewSelectionFrame::selectedLevelIndexChangedAl()
{
	if (NoCallBack_)
		return;

	NoCallBack_ = true;
	if (levelsSelectionSc_->count()>0)
		levelsSelectionSc_->clear();
	if (levelsSelectionVal_->count()>0)
		levelsSelectionVal_->clear();

	resetPlotsList();

	plotType_->setEnabled(true);
	plotType_->setCurrentIndex(0);
	plotSelection_->setEnabled(true);
	preflagEdit_->setEnabled(true);
	postflagEdit_->setEnabled(true);
	saveSettings_->setEnabled(true);
	recoverSettings_->setEnabled(true);
	levelsSelectionMark_->setEnabled(true);

	for (int i = 0; i < levelsSelectionAl_->count(); ++i)
	{
		//reset Align Parameter for all
		QListWidgetItem* pWidgetItem = levelsSelectionAl_->item(i);
		if (pWidgetItem)
		{
			QString widgetName = pWidgetItem->text();
			int n = widgetName.indexOf(" - ");
			if (n > -1)
			{
				int AssetId = (widgetName.left(n)).toInt();
				if (designRoot_ &&  designRoot_->getExperiment()->getDesignConfig())
				{
					QSharedPointer<Asset> alignToAsset = designRoot_->getExperiment()->getDesignConfig()->getAsset(AssetId);
					if (alignToAsset && alignToAsset.objectCast<StateMachineElement>())
						alignToAsset.objectCast<StateMachineElement>()->setAlignParam(false);
				}
			}
		}
	}

	//Update scriptables list
	for (int i = 0; i < levelsSelectionAl_->count(); ++i)
	{
		QListWidgetItem* pWidgetItem = levelsSelectionAl_->item(i);
		if (pWidgetItem && pWidgetItem->checkState() == Qt::Checked)
		{
			//Disaply corresponding Scriptables
			QString widgetName = pWidgetItem->text();
			int n = widgetName.indexOf(" - ");
			if (n > -1)
			{
				//1. set as align element
				int AssetId = (widgetName.left(n)).toInt();
				if (designRoot_ &&  designRoot_->getExperiment()->getDesignConfig())
				{
					QSharedPointer<Asset> alignToAsset = designRoot_->getExperiment()->getDesignConfig()->getAsset(AssetId);
					if (alignToAsset && alignToAsset.objectCast<StateMachineElement>())
						alignToAsset.objectCast<StateMachineElement>()->setAlignParam(true);
				}

				//2. retrieve corresponding scriptables (conditions)
				QList<int> scriptableMembers = alignElements_.value(AssetId);
				qSort(scriptableMembers.begin(), scriptableMembers.end());
				foreach(int scriptableID, scriptableMembers)
				{
					if (designRoot_ &&  designRoot_->getExperiment()->getDesignConfig())
					{
						QSharedPointer<Asset> asset = designRoot_->getExperiment()->getDesignConfig()->getAsset(scriptableID);
						if (asset)
						{
							if (asset && (asset.objectCast<NumberVariable>() || asset.objectCast<BooleanParameter>()))
							{
								QString assetPath = QString::number(AssetId) + ";" + QString::number(scriptableID) + " - " + asset->getPath().split("::").last();
								if (assetPath != "")
								{
									//Add event to Scriptables list
									NoCallBack_ = true;
									QListWidgetItem *newEvent = new QListWidgetItem(assetPath, levelsSelectionSc_);
									newEvent->setCheckState(Qt::Unchecked);
									levelsSelectionSc_->addItem(newEvent);
									NoCallBack_ = false;
								}
							}
						}
					}
				}
			}
		}
	}

	NoCallBack_ = false;
	updateSizeAndPositionOptions();
}
void ViewSelectionFrame::selectedLevelIndexChangedSc()
{
	if (NoCallBack_)
		return;

	updateSizeAndPositionOptions();

	NoCallBack_ = true;
	if (levelsSelectionVal_->count()>0)
		levelsSelectionVal_->clear();

	resetPlotsList();

	plotType_->setEnabled(true);
	plotType_->setCurrentIndex(0);
	plotSelection_->setEnabled(true);
	plotSelection_->setCurrentIndex(0);
	preflagEdit_->setEnabled(true);
	postflagEdit_->setEnabled(true);
	saveSettings_->setEnabled(true);
	recoverSettings_->setEnabled(true);
	levelsSelectionMark_->setEnabled(true);

	for (int i = 0; i < levelsSelectionSc_->count(); ++i)
	{
		//reset Script for all
		QListWidgetItem* pWidgetItem = levelsSelectionSc_->item(i);
		if (pWidgetItem)
		{
			QString widgetName = pWidgetItem->text();
			int n = widgetName.indexOf(" - ");
			if (n > -1)
			{
				int AssetId = (widgetName.left(n)).toInt();
				if (designRoot_ &&  designRoot_->getExperiment()->getDesignConfig())
				{
					QSharedPointer<Asset> asset = designRoot_->getExperiment()->getDesignConfig()->getAsset(AssetId);
					if (asset && asset.objectCast<Scriptable>())
					{
						asset.objectCast<Scriptable>()->setScriptParam(false);
					}
				}
			}
		}
	}
	
	//Update scriptables list
	for (int i = 0; i < levelsSelectionSc_->count(); ++i)
	{
		QListWidgetItem* pWidgetItem = levelsSelectionSc_->item(i);
		if (pWidgetItem && pWidgetItem->checkState() == Qt::Checked)
		{
			//Display selected Scriptables with line for values
			QString widgetName = pWidgetItem->text();
			int n = widgetName.indexOf(" - ");
			if (n > -1)
			{
				QString widgetIdStr = widgetName.left(n);
				int m = widgetIdStr.indexOf(";");
				if (m > -1)
				{
					QString ScrIdStr = widgetIdStr.mid(m + 1);
					int ScrId = ScrIdStr.toInt();
					if (designRoot_ &&  designRoot_->getExperiment()->getDesignConfig())
					{
						QSharedPointer<Asset> ScrAsset = designRoot_->getExperiment()->getDesignConfig()->getAsset(ScrId);
						if (ScrAsset)
						{
							//1. set as Scriptable element
							if (ScrAsset.objectCast<Scriptable>())
								ScrAsset.objectCast<Scriptable>()->setScriptParam(true);

							NoCallBack_ = true;
							QListWidgetItem *newEvent = new QListWidgetItem(levelsSelectionVal_);

							lblEdit* scrValsEv = new lblEdit(widgetIdStr);
							QLineEdit* lineEdit = scrValsEv->getLineEdit();

							//get edit insert from the lblEdit and connect signals
							connect(lineEdit, SIGNAL(textChanged(const QString &)), this, SLOT(selectedLevelIndexChangedVal()));

							newEvent->setSizeHint(scrValsEv->sizeHint());
							levelsSelectionVal_->addItem(newEvent);
							NoCallBack_ = false;
							levelsSelectionVal_->setItemWidget(newEvent, scrValsEv);
						}
					}
				}
			}
		}
	}
	NoCallBack_ = false;
	updateSizeAndPositionOptions();
}
void ViewSelectionFrame::selectedLevelIndexChangedVal()
{
	if (NoCallBack_)
		return;

	resetPlotsList();
	NoCallBack_ = true;

	plotType_->setEnabled(true);
	plotType_->setCurrentIndex(0);
	plotSelection_->setEnabled(true);
	plotSelection_->setCurrentIndex(0);
	preflagEdit_->setEnabled(true);
	postflagEdit_->setEnabled(true);
	saveSettings_->setEnabled(true);
	recoverSettings_->setEnabled(true);
	levelsSelectionMark_->setEnabled(true);

	NoCallBack_ = false;
	updateSizeAndPositionOptions();
}
void ViewSelectionFrame::selectedLevelIndexChangedMark()
{
	if (NoCallBack_)
	return;

	resetPlotsList();
	NoCallBack_ = true;

	plotType_->setEnabled(true);
	plotType_->setCurrentIndex(0);
	plotSelection_->setEnabled(true);
	plotSelection_->setCurrentIndex(0);
	preflagEdit_->setEnabled(true);
	postflagEdit_->setEnabled(true);
	saveSettings_->setEnabled(true);
	recoverSettings_->setEnabled(true);

	for (int i = 0; i < levelsSelectionMark_->count(); ++i)
	{
		//reset MarkerParam for all
		QListWidgetItem* pWidgetItem = levelsSelectionMark_->item(i);
		if (pWidgetItem)
		{
			QString widgetName = pWidgetItem->text();
			int n = widgetName.indexOf(" - ");
			if (n > -1)
			{
				int AssetId = (widgetName.left(n)).toInt();
				if (designRoot_ &&  designRoot_->getExperiment()->getDesignConfig())
				{
					QSharedPointer<Asset> asset = designRoot_->getExperiment()->getDesignConfig()->getAsset(AssetId);
					if (asset && asset.objectCast<StateMachineElement>())
					{
						asset.objectCast<StateMachineElement>()->setMarkerParam(false);
					}
				}
			}
		}
	}
	//Update marker param
	for (int i = 0; i < levelsSelectionMark_->count(); ++i)
	{
		QListWidgetItem* pWidgetItem = levelsSelectionMark_->item(i);
		if (pWidgetItem && pWidgetItem->checkState() == Qt::Checked)
		{
			QString widgetName = pWidgetItem->text();
			int n = widgetName.indexOf(" - ");
			if (n > -1)
			{
				//1. set as marker element
				int AssetId = (widgetName.left(n)).toInt();
				if (designRoot_ &&  designRoot_->getExperiment()->getDesignConfig())
				{
					QSharedPointer<Asset> alignToAsset = designRoot_->getExperiment()->getDesignConfig()->getAsset(AssetId);
					if (alignToAsset && alignToAsset.objectCast<StateMachineElement>())
						alignToAsset.objectCast<StateMachineElement>()->setMarkerParam(true);
				}
			}
		}
	}

	NoCallBack_ = false;
	updateSizeAndPositionOptions(); 
}
void ViewSelectionFrame::selectedPlotTypeChanged(int)
{
	if (NoCallBack_)
		return;

	selectedType_ = plotType_->currentText();
	int currIndex = plotType_->currentIndex();
	resetPlotsList();
	NoCallBack_ = true;

	plotSelection_->setEnabled(true);
	plotSelection_->setCurrentIndex(0);
	plotType_->setEnabled(true);
	plotType_->setCurrentIndex(currIndex);
	saveSettings_->setEnabled(true);
	recoverSettings_->setEnabled(true);
	levelsSelectionMark_->setEnabled(true);

	if (selectedType_ == "Select")
	{
		NoCallBack_ = false;

		saveSettings_->setEnabled(true);
		recoverSettings_->setEnabled(true);
		preflagEdit_->setEnabled(true);
		postflagEdit_->setEnabled(true);

		return;
	}
	else
	{
		preflagEdit_->setEnabled(false);
		postflagEdit_->setEnabled(false);

		if (selectedType_ == "PSTH Plot")
		{
			//No Markers for PSTH Plots
			for (int i = 0; i < levelsSelectionMark_->count(); ++i)
			{
				//reset MarkerParam for all
				QListWidgetItem* pWidgetItem = levelsSelectionMark_->item(i);
				if (pWidgetItem)
				{
					pWidgetItem->setCheckState(Qt::Unchecked);
					QString widgetName = pWidgetItem->text();
					int n = widgetName.indexOf(" - ");
					if (n > -1)
					{
						int AssetId = (widgetName.left(n)).toInt();
						if (designRoot_ &&  designRoot_->getExperiment()->getDesignConfig())
						{
							QSharedPointer<Asset> asset = designRoot_->getExperiment()->getDesignConfig()->getAsset(AssetId);
							if (asset && asset.objectCast<StateMachineElement>())
							{
								asset.objectCast<StateMachineElement>()->setMarkerParam(false);
							}
						}
					}
				}
			}
			levelsSelectionMark_->setEnabled(false);
		}
	}

	if (!customAnalysis_)
		customAnalysis_ = CreateCustomAnalysis();

	QList<QString> eventsList;

	//Parse List of Neurons
	for (int row = 0; row < levelsSelectionNeur_->count(); ++row)
	{
		QListWidgetItem *pWidgetItem = levelsSelectionNeur_->item(row);
		if (pWidgetItem && pWidgetItem->checkState() == Qt::Checked)
		{
			QString dataSetName = pWidgetItem->text();
			eventsList.append(dataSetName);
		}
	}
	//Parse List of Events (Hor)
	for (int row = 0; row < levelsSelectionHor_->count(); ++row)
	{
		QListWidgetItem *pWidgetItem = levelsSelectionHor_->item(row);
		if (pWidgetItem && pWidgetItem->checkState() == Qt::Checked)
		{
			QString dataSetName = pWidgetItem->text();
			eventsList.append(dataSetName);
		}
	}
	foreach (QString dataSetName, eventsList)
	{
		//Parse align elements
		for (int ind = 0; ind < levelsSelectionAl_->count(); ind++)
		{
			QListWidgetItem *pAlWidgetItem = levelsSelectionAl_->item(ind);

			int selectedAlignID = 0;
			if (pAlWidgetItem && pAlWidgetItem->checkState() == Qt::Checked)
			{
				//for each align event						
				QString alignStr = "";
				QString alignIDStr = pAlWidgetItem->text(); //2049 - DisplayDots
				int n = alignIDStr.indexOf(" - ");
				if (n > -1)
				{
					alignStr = alignIDStr.mid(n + 3); //DisplayDots
					selectedAlignID = alignIDStr.left(n).toInt(); //2049
				}

				//Make sure we have at least elements selected for Neurons and Alignment (Scriptables and Values are optional)
				QList<QPair<QString, QString>> selScriptables;
				//Parse Values List (Alignment/Scriptables/Values)
				for (int i = 0; i < levelsSelectionVal_->count(); ++i)
				{
					QListWidgetItem* pValWidgetItem = levelsSelectionVal_->item(i);
					if (pValWidgetItem)
					{
						lblEdit* pLineEditWidgetItem = qobject_cast<lblEdit*>(levelsSelectionVal_->itemWidget(pValWidgetItem));
						if (pLineEditWidgetItem)
						{
							QString scriptable = pLineEditWidgetItem->getLabel(); // <alignment;condition>
							n = scriptable.indexOf(";");
							if (n > -1)
							{
								int alignmentID = scriptable.left(n).toInt(); //alignment
								if (alignmentID == selectedAlignID)
								{
									QLineEdit* lineEdit = pLineEditWidgetItem->getLineEdit();
									QString pVals = lineEdit->text();
									QPair<QString, QString> scrSelPair;
									scrSelPair.first = scriptable.mid(n + 1); // 676 //condition
									scrSelPair.second = pVals; //values
									selScriptables.append(scrSelPair); // list of pairs <5314,1;2;3> <condition,1;2;3>
								}
							}
						}
					}
				}

				//generate list of scriptables combinations and create corresponding plots for this Align Event
				QList<QList<QPair<int, int>>> scrCombList;
				QList<QPair<int, int>> scComb;
				QList<int> alignIDList;
				alignIDList.append(selectedAlignID);
				int scrCombIndex = 1;
				if (selScriptables.size() > 0)
				{
					scrCombList = generateScrComb(selScriptables);
					foreach(scComb, scrCombList)
					{
						createCustomWidget(dataSetName, alignIDList, scComb, QString::number(scrCombIndex));
						++scrCombIndex;
					}
				}
				else
				{
					//No Condition was selected: create Plot aligned to align event
					createCustomWidget(dataSetName, alignIDList, scComb);
				}
			}
		}		
	}
	NoCallBack_ = false;
	updateSizeAndPositionOptions();

}
void ViewSelectionFrame::resetPlotsList(int resetSelection)
{
	if (!activatePlots_)
		return;

	NoCallBack_ = true;

	if (resetSelection > 0)
	{
		if (levelsSelectionNeur_->count() > 0 && (resetSelection==1))
			levelsSelectionNeur_->clear();
		else
		{
			for (int i = 0; i < levelsSelectionNeur_->count(); ++i)
			{
				//uncheck all
				QListWidgetItem* pWidgetItem = levelsSelectionNeur_->item(i);
				pWidgetItem->setCheckState(Qt::Unchecked);
			}
		}
		if (levelsSelectionHor_->count() > 0 && (resetSelection == 1))
			levelsSelectionHor_->clear();
		else
		{
			for (int i = 0; i < levelsSelectionHor_->count(); ++i)
			{
				//uncheck all
				QListWidgetItem* pWidgetItem = levelsSelectionHor_->item(i);
				pWidgetItem->setCheckState(Qt::Unchecked);
			}
		}
		if (levelsSelectionAl_->count() > 0)
		{
			for (int i = 0; i < levelsSelectionAl_->count(); ++i)
			{
				//reset Align Parameter
				QListWidgetItem* pWidgetItem = levelsSelectionAl_->item(i);
				if (pWidgetItem && pWidgetItem->checkState() == Qt::Checked)
				{
					QString widgetName = pWidgetItem->text();
					int n = widgetName.indexOf(" - ");
					if (n > -1)
					{
						int AssetId = (widgetName.left(n)).toInt();
						if (designRoot_ &&  designRoot_->getExperiment()->getDesignConfig())
						{
							QSharedPointer<Asset> alignToAsset = designRoot_->getExperiment()->getDesignConfig()->getAsset(AssetId);
							if (alignToAsset && alignToAsset.objectCast<StateMachineElement>())
								alignToAsset.objectCast<StateMachineElement>()->setAlignParam(false);
						}
					}
					pWidgetItem->setCheckState(Qt::Unchecked);
				}
			}

			if (resetSelection == 1)
				levelsSelectionAl_->clear();
			else
			{
				for (int i = 0; i < levelsSelectionAl_->count(); ++i)
				{
					//uncheck all
					QListWidgetItem* pWidgetItem = levelsSelectionAl_->item(i);
					pWidgetItem->setCheckState(Qt::Unchecked);
				}
			}
		}
		if (levelsSelectionSc_->count() > 0)
		{
			for (int i = 0; i < levelsSelectionSc_->count(); ++i)
			{
				//reset Align Parameter
				QListWidgetItem* pWidgetItem = levelsSelectionSc_->item(i);
				if (pWidgetItem && pWidgetItem->checkState() == Qt::Checked)
				{
					QString widgetName = pWidgetItem->text();
					int n = widgetName.indexOf(" - ");
					if (n > -1)
					{
						int AssetId = (widgetName.left(n)).toInt();
						if (designRoot_ &&  designRoot_->getExperiment()->getDesignConfig())
						{
							QSharedPointer<Asset> alignToAsset = designRoot_->getExperiment()->getDesignConfig()->getAsset(AssetId);
							if (alignToAsset && alignToAsset.objectCast<StateMachineElement>())
								alignToAsset.objectCast<StateMachineElement>()->setScriptParam(false);
						}
					}
					pWidgetItem->setCheckState(Qt::Unchecked);
				}
			}

			if (resetSelection == 1)
				levelsSelectionSc_->clear();
			else
			{
				for (int i = 0; i < levelsSelectionSc_->count(); ++i)
				{
					//uncheck all
					QListWidgetItem* pWidgetItem = levelsSelectionSc_->item(i);
					pWidgetItem->setCheckState(Qt::Unchecked);
				}
			}
		}
		
		for (int i = 0; i < levelsSelectionVal_->count(); ++i)
		{
			QListWidgetItem* pValWidgetItem = levelsSelectionVal_->item(i);
			if (pValWidgetItem)
			{	
				lblEdit* pLineEditWidgetItem = qobject_cast<lblEdit*>(levelsSelectionVal_->itemWidget(pValWidgetItem));
				if (pLineEditWidgetItem)
				{					
					QLineEdit* lineEdit = pLineEditWidgetItem->getLineEdit();
					lineEdit->clear();					
				}
			}
		}
		if (levelsSelectionVal_->count() > 0) //in any case (when resetSelection is >0 ) this list should be cleared
			levelsSelectionVal_->clear();

		if (levelsSelectionMark_->count() > 0)
		{
			for (int i = 0; i < levelsSelectionMark_->count(); ++i)
			{
				//reset Align Parameter
				QListWidgetItem* pWidgetItem = levelsSelectionMark_->item(i);
				if (pWidgetItem && pWidgetItem->checkState() == Qt::Checked)
				{
					QString widgetName = pWidgetItem->text();
					int n = widgetName.indexOf(" - ");
					if (n > -1)
					{
						int AssetId = (widgetName.left(n)).toInt();
						if (designRoot_ &&  designRoot_->getExperiment()->getDesignConfig())
						{
							QSharedPointer<Asset> alignToAsset = designRoot_->getExperiment()->getDesignConfig()->getAsset(AssetId);
							if (alignToAsset && alignToAsset.objectCast<StateMachineElement>())
								alignToAsset.objectCast<StateMachineElement>()->setMarkerParam(false);
						}
					}
					pWidgetItem->setCheckState(Qt::Unchecked);
				}
			}

			if (resetSelection == 1)
				levelsSelectionMark_->clear();
			else
			{
				for (int i = 0; i < levelsSelectionMark_->count(); ++i)
				{
					//uncheck all
					QListWidgetItem* pWidgetItem = levelsSelectionMark_->item(i);
					pWidgetItem->setCheckState(Qt::Unchecked);
				}
			}
		}

		levelsSelectionAl_->setEnabled(false);
		levelsSelectionSc_->setEnabled(false);
		levelsSelectionVal_->setEnabled(false);
		levelsSelectionMark_->setEnabled(false);
	}

	QList<QWidget*> widgets = viewContainerMap_.keys();
	foreach(QWidget *widget, widgets)
	{
		removeFromList(widget); //this reset NoCallBack_ to false;
		NoCallBack_ = true;
	}

	if (currentTaskConfig_)
		currentTaskConfig_->removeNeuralDataListeners();

	if (customAnalysis_)
		DeleteCustomAnalysis();

	//now that custom analysis was deleted, can safely release designRoot_
	if ((resetSelection==1) && designRoot_)
		designRoot_.clear();

	updateSizeAndPositionOptions();

	plotSelection_->setEnabled(false);
	plotType_->setCurrentIndex(0);
	plotType_->setEnabled(false);
	preflagEdit_->setEnabled(false);
	postflagEdit_->setEnabled(false);
	saveSettings_->setEnabled(false);
	recoverSettings_->setEnabled(false);

	NoCallBack_ = false;
}
void ViewSelectionFrame::removeFromList(QWidget* widget)
{
	if (!activatePlots_)
		return;

	if (viewContainerMap_.contains(widget))
	{
		DataViewWidget* pDataViewWidget = viewContainerMap_[widget];
		if (pDataViewWidget && pDataViewWidget->getRetentionPolicy() != DVW_RETAIN)
		{
			QString widgName = pDataViewWidget->getName();
			if (widgName != "Task")
			{
				int n = widgName.indexOf(" - Raster Plot");
				int m = widgName.indexOf(" - PSTH Plot");
				
				if ((n>-1) || (m>-1)) //do not delete legacy plots
					//delete actual widget - takes care of dataViewWidgets_,  viewContainerMap_ and plotSelection_
					//removeDataViewWidget(pDataViewWidget);
					currentTaskConfig_->removeWidget(widget); //this calls slot removeDataViewWidget too
			}
		}
		NoCallBack_ = true;
		plotSelection_->setCurrentIndex(0);
		NoCallBack_ = false;
	}
}
void ViewSelectionFrame::updateUI(bool activatePlots)
{
	activatePlots_ = activatePlots;
	if (!activatePlots_)
	{
		neuronsSelWidget_->setVisible(false);
		selectionLayout_->removeWidget(neuronsSelWidget_);
		typeSelWidget_->setVisible(false);
		selectionLayout_->removeWidget(typeSelWidget_);
		flagSelWidget_->setVisible(false);
		selectionLayout_->removeWidget(flagSelWidget_);
	}
	else
	{
		levelsSelectionNeur_->setEnabled(false);
		levelsSelectionHor_->setEnabled(false);
		levelsSelectionMark_->setEnabled(false);

		flagSelWidget_->setVisible(true);
		selectionLayout_->addWidget(flagSelWidget_, 0, 0, Qt::AlignCenter);
		neuronsSelWidget_->setVisible(true);
		selectionLayout_->addWidget(neuronsSelWidget_, 1, 0, Qt::AlignCenter);
		typeSelWidget_->setVisible(true);
		selectionLayout_->addWidget(typeSelWidget_, 2, 0, Qt::AlignCenter);
	}
}
//for Plots
void ViewSelectionFrame::hideCurrentWidget(QWidget *pWidget)
{
	if (!activatePlots_)
		return;

	DataViewWidget *pCurrentView = (DataViewWidget*)pWidget;
	if (!pCurrentView)
		return;

	pCurrentView->setPosition(0, 0);
	pCurrentView->setDisplayed(false);
	emit(widgetRemoved(pCurrentView));
	pCurrentView->hide();
	
	if (pWidget && widgetIndexMap_.key((DataViewWidget*)pWidget))
	{
		plotSelection_->setCurrentIndex(plotSelection_->findData(widgetIndexMap_.key((DataViewWidget*)pWidget)));
	}

	updateSizeAndPositionOptions();
}
void ViewSelectionFrame::setTaskName(QString taskName)
{
	currentTask_ = taskName;
}

QPair<int, int> ViewSelectionFrame::getFlags()
{
	QPair<int, int> pair;
	double preFlag = 0;
	double postFlag = 4000; //default
	if (preflagEdit_)
		preFlag = preflagEdit_->getLineEdit()->text().toDouble();

	if (postflagEdit_)
		postFlag = postflagEdit_->getLineEdit()->text().toDouble();

	pair.first = preFlag;
	pair.second = postFlag;

	return pair;
}
QPair<int, int> ViewSelectionFrame::getNeuron(QString dataSetName)
{
	QPair<int, int> pair;
	pair.first = -1;
	pair.second = -1;

	int n = dataSetName.indexOf(" - ");
	int channel = -1;
	int unit = -1;
	if (n > -1)
	{
		QString leftStr = dataSetName.left(n); //Channel 12
		if (leftStr.indexOf("Channel") < 0)
			return pair;

		QString rightStr = dataSetName.mid(n + 3); //Unit 1

		n = leftStr.indexOf(" ");
		if (n > -1)
		{
			QString chanStr = leftStr.mid(n + 1); //12
			channel = chanStr.toInt();

			n = rightStr.indexOf(" ");
			if (n > -1)
			{
				QString unitStr = rightStr.mid(n + 1); //1
				unit = unitStr.toInt();
			}
		}
	}
	pair.first = channel;
	pair.second = unit;

	return pair;
}
int ViewSelectionFrame::getEvent(QString dataSetName)
{	
	int eventID = -1;
	int n = dataSetName.indexOf(" - ");	//5499 - ChoseLeft
	if (n > -1)
	{
		QString leftStr = dataSetName.left(n); //5499
		if (leftStr.indexOf("Channel") < 0)
			eventID = leftStr.toInt();
	}
	return eventID;	
}
QList<QList<QPair<int, int>>> ViewSelectionFrame::generateScrComb(QList<QPair<QString, QString>> scriptables)
{
	QPair<QString, QString> scrPair;
	QList<QList<QPair<QString, QString>>> pairsList;

	foreach(scrPair, scriptables)
	{
		QList<QPair<QString, QString>> pairs;
		QString scrRange = scrPair.second; // 1;2;3;4
		QStringList scrVals = scrRange.split(';'); //[1,2,3,4]

		for (int index = 0; index < scrVals.size(); index++)
		{
			QPair<QString, QString> genPair;
			genPair.first = scrPair.first;
			genPair.second = scrVals[index];
			if (scrVals[index].contains("-")) //range
			{
				QStringList scrSubVals = scrVals[index].split('-'); //[2-4]  -> [2,4]
				int min = scrSubVals.at(0).toInt(); //2
				int s = scrSubVals.size() - 1;
				int max = (scrSubVals.at(s)).toInt() + 1; //4

				int val = min;
				while (val<max)
				{
					QPair<QString, QString> genSubPair;
					genSubPair.first = scrPair.first;
					genSubPair.second = QString::number(val); // 2 ,3, 4

					val++;

					pairs.append(genSubPair);
				}
			}
			else
				pairs.append(genPair);
		}

		pairsList.append(pairs); //[ [<5314,1>,<5314,2>,<5314,3>,<5314,4>] , [<2882,0>,<2882,-1>], [<5461,0>,<5461,1>] ... ]
	}

	int size = maxVals(pairsList);

	//parse all lists and add value @ at the end if too short
	QList<QPair<QString, QString>> List;
	QList<QList<QPair<QString, QString>>> pairsList2;
	foreach(List, pairsList)
	{
		QPair<QString, QString> genPair;
		QPair<QString, QString> firstElt = List[0];
		firstElt.second = "@";

		while (List.size() < size)
		{
			List.append(firstElt);
		}
		pairsList2.append(List);
	}

	QList<QList<QPair<int, int>>> tmpList;
	for (int index = 0; index <size; index++)
	{
		QList<QPair<int, int>> scrTmpList;
		QList<QPair<QString, QString>> scrList;
		foreach(scrList, pairsList2)
		{
			if (scrList[index].second != "@")
			{
				QPair<int, int> genPair;
				genPair.first = scrList[index].first.toInt();
				genPair.second = scrList[index].second.toInt();

				scrTmpList.append(genPair); //[<5499-5314,1>, <5499-2882, 3>]
			}
		}

		tmpList.append(scrTmpList);
	}
	return tmpList; //[<5499-5314,1>, <5499-2882, 0>]
}
int ViewSelectionFrame::maxVals(QList<QList<QPair<QString, QString>>> pairsList)
{
	// all lines nust have the same number of values so we can retrieve the matrix columns: each "column" leads to a combination/plot)
	// if not, add @ to the "short" lines
	// in the ui: value @ can be inserted if a condition is not needed, eg. [2;1;4;-1;4;6]
	int size = -1;

	QList<QPair<QString, QString>> scrList;
	foreach(scrList, pairsList)
	{
		if (size < scrList.size())
			size = scrList.size();
	}
	return size;
}
void ViewSelectionFrame::DeleteCustomAnalysis()
{
	if (customAnalysis_)
	{
		//We are deleting this analysis
		currentTaskConfig_->notifyAnalysisSelection(customAnalysis_->getName(), false);

		QUuid nextAnalysisId = customAnalysis_->getAssociateId();
		if (designRoot_ && designRoot_->getExperiment())
		{
			designRoot_->getExperiment().staticCast<DataStore>()->ClearAssociateDescendants(nextAnalysisId);

			QList<QUuid> uuidList = designRoot_->getExperiment()->getDesignConfig()->getActiveAnalysisIds();
			uuidList.removeAll(nextAnalysisId);
			designRoot_->getExperiment()->getDesignConfig()->setActiveAnalysisIds(uuidList);

			Q_ASSERT(customAnalysisIndex_ < designRoot_->getNumAnalyses());
			if (!designRoot_->cullAnalysis(customAnalysisIndex_))
				Q_ASSERT(false);

			customAnalysis_.clear();
		}
	}
}
QSharedPointer<Picto::Analysis> ViewSelectionFrame::CreateCustomAnalysis()
{
	if (designRoot_ && designRoot_->getExperiment())
	{
		//Delete existing custom analysis, if any
		if (customAnalysis_)	
			DeleteCustomAnalysis();

		//Create a new analysis to put the exported analysis in.
		customAnalysis_ = designRoot_->importAnalysis("<Analysis/>").staticCast<Analysis>();
		customAnalysisIndex_ = designRoot_->getNumAnalyses() - 1; //keep track of the Index of the custom Analysis added so we can delete it later
		
		QString newName = QUuid::createUuid().toString();
		customAnalysis_->setName(newName);
		QUuid newAnalysisId = customAnalysis_->getAssociateId();

		bool linkResult = customAnalysis_->LinkToAsset(designRoot_->getExperiment(), QString());
		IGNORED_PARAMETER(linkResult);
		Q_ASSERT(linkResult);
		//Create UI Data for the new Analysis and attach it
		AssociateRootHost* assocRootHost = dynamic_cast<AssociateRootHost*>(customAnalysis_.data());
		Q_ASSERT(assocRootHost);
		QUuid hostId = assocRootHost->getHostId();
		QSharedPointer<Asset> newUIData = customAnalysis_->getParentAsset().staticCast<DataStore>()->createChildAsset("UIData", QString(), QString());
		QString feedback;
		newUIData.staticCast<AssociateRoot>()->LinkToAsset(customAnalysis_, feedback);
		Q_ASSERT(customAnalysis_);

		QList<QUuid> uuidList = designRoot_->getExperiment()->getDesignConfig()->getActiveAnalysisIds();
		uuidList.append(newAnalysisId);
		designRoot_->getExperiment()->getDesignConfig()->setActiveAnalysisIds(uuidList);
	}
	return customAnalysis_;
}
void ViewSelectionFrame::createCustomWidget(QString dataSetName, QList<int> alignIDList, QList<QPair<int, int>> scComb, QString scriptCombIndex)
{
	//Name needs to be different based on plottype, as in the TaskConfig the cached handlers are stored based on the asset name
	//Otherwise, if 2 plots are given the same name (but their type is different), the wrong handler is sometime retrieved
	//Beware! this name is used in RasterPlot::SubmitScan() and in ::AddContainer()
	int align = alignIDList.first();
	QString alignID = QString::number(align);
	QString plotName = dataSetName + "-" + alignID + "-" + scriptCombIndex + "-" + selectedType_;

	if (customAnalysis_)
	{
		//The Plots are created from the Analysis asset
		currentTaskConfig_->notifyAnalysisSelection(customAnalysis_->getName(), true);
		QSharedPointer<DataStore> parentDataStore = customAnalysis_.staticCast<DataStore>();
		if (parentDataStore)
		{
			QSharedPointer<Asset> newAsset = parentDataStore->createChildAsset("DataViewElement", selectedType_, QString());
			if (newAsset)
			{
				newAsset.objectCast<DataViewElement>()->setManualTitle(plotName);
				//this is important for the taskConfig to request/receive the handler
				newAsset.objectCast<UIEnabled>()->setName(plotName);

				//newAsset.staticCast<AssociateRoot>()->LinkToAsset(customAnalysis_, QString());
				parentDataStore->setTaskConfig(currentTaskConfig_);
				newAsset->setTaskConfig(currentTaskConfig_);

				AssociateElement* assocElem = dynamic_cast<AssociateElement*>(newAsset.data());
				if (assocElem)
				{
					if (assocElem->attachToLinkedAsset(false) == AssociateElement::FAIL)
					{
						newAsset->setDeleted();
						return;
					}

					assocElem->getLinkedAsset()->setTaskConfig(currentTaskConfig_);
					assocElem->postLinkUpdate();

					//make sure this is after postLinkUpdate()
					newAsset.objectCast<DataViewElement>()->setManualTitle(plotName);
					newAsset.objectCast<UIEnabled>()->setName(plotName);

					NeuralDataListener* neuralDataListener = dynamic_cast<NeuralDataListener*>(newAsset.data());
					if (neuralDataListener)
					{
						QPair<int, int> neuron = getNeuron(dataSetName);
						if (neuron.first > 0)
							neuralDataListener->setNeuron(neuron.first, neuron.second);
						else
						{
							int eventID = getEvent(dataSetName);
							if (eventID > 0)
								neuralDataListener->setEvent(eventID);
						}
						neuralDataListener->setAlignData(alignIDList);
						neuralDataListener->setCondition(scComb);
						QPair<int, int> flags = getFlags();
						neuralDataListener->setPreFlagWindow(flags.first);
						neuralDataListener->setPostFlagWindow(flags.second);
					}

					newAsset->getDesignConfig()->fixDuplicatedAssetIds();
				}
			}
		}
	}
	updateSizeAndPositionOptions();
}
void ViewSelectionFrame::widgetDragged(QWidget *pWidget)
{
	if (pWidget && widgetIndexMap_.key((DataViewWidget*)pWidget))
	{
		plotSelection_->setCurrentIndex(plotSelection_->findData(widgetIndexMap_.key((DataViewWidget*)pWidget)));
	}

	updateSizeAndPositionOptions();
}
void ViewSelectionFrame::widgetDropped(QPoint offset)
{
	//The current widget
	DataViewWidget *pCurrentView = widgetIndexMap_[plotSelection_->currentData().toInt()];
	if (!pCurrentView)
		return;

	DataViewSize::ViewSize originalSize = pCurrentView->getCurrentSize();
	while (!isWidgetPosValid(pCurrentView, offset.x(), offset.y()) && pCurrentView->getCurrentSize() > DataViewSize::VIEW_SIZE_1x1)
	{
		pCurrentView->setCurrentSize((DataViewSize::ViewSize)(pCurrentView->getCurrentSize() - 1));
	}

	if (isWidgetPosValid(pCurrentView, offset.x(), offset.y()))
	{
		pCurrentView->setPosition(offset.x(), offset.y());

		pCurrentView->setDisplayed(true);
		emit(widgetRemoved(pCurrentView));
		emit(widgetAdded(pCurrentView, offset.x(), offset.y(), pCurrentView->getCurrentSize()));
		pCurrentView->show();
	}
	else
	{
		//keep original position
		int x, y;
		pCurrentView->getPosition(x, y);

		pCurrentView->setCurrentSize(originalSize);

		pCurrentView->setDisplayed(true);
		emit(widgetRemoved(pCurrentView));
		emit(widgetAdded(pCurrentView, x, y, originalSize));
	}

	pCurrentView->show();
	updateSizeAndPositionOptions();
}

void ViewSelectionFrame::savePlotSettings()
{
	plotSettings_.clear();
	//Parse List of Neurons
	QString horList;
	QString neurList;
	QString alList;
	QString scList;
	QString valList;
	QString markList;
	for (int row = 0; row < levelsSelectionNeur_->count(); ++row)
	{
		QListWidgetItem *pWidgetItem = levelsSelectionNeur_->item(row);
		if (pWidgetItem && pWidgetItem->checkState() == Qt::Checked)
		{
			QString dataSetName = pWidgetItem->text();

			neurList.append(dataSetName + "&");
		}
	}
	for (int row = 0; row < levelsSelectionHor_->count(); ++row)
	{
		QListWidgetItem *pWidgetItem = levelsSelectionHor_->item(row);
		if (pWidgetItem && pWidgetItem->checkState() == Qt::Checked)
		{
			QString dataSetName = pWidgetItem->text();

			horList.append(dataSetName + "&");
		}
	}

	//Parse align elements
	for (int ind = 0; ind < levelsSelectionAl_->count(); ind++)
	{
		QListWidgetItem *pAlWidgetItem = levelsSelectionAl_->item(ind);
		if (pAlWidgetItem && pAlWidgetItem->checkState() == Qt::Checked)
		{
			//for each align event						
			QString alignStr = "";
			QString alignIDStr = pAlWidgetItem->text(); //2049 - DisplayDots
			alList.append(alignIDStr + "&");
		}
	}

	//Parse Values List (Alignment/Scriptables/Values)
	for (int i = 0; i < levelsSelectionVal_->count(); ++i)
	{
		QListWidgetItem* pValWidgetItem = levelsSelectionVal_->item(i);
		if (pValWidgetItem)
		{
			lblEdit* pLineEditWidgetItem = qobject_cast<lblEdit*>(levelsSelectionVal_->itemWidget(pValWidgetItem));
			if (pLineEditWidgetItem)
			{
				QString scriptable = pLineEditWidgetItem->getLabel(); // <alignment;condition>
				scList.append(scriptable + "&"); // <4778;4879>
				int n = scriptable.indexOf(";");
				if (n > -1)
				{
					QLineEdit* lineEdit = pLineEditWidgetItem->getLineEdit();
					QString pVals = lineEdit->text();
					if (pVals != "")
						valList.append(scriptable + "//" + pVals + "&"); // <4778;4879//1;2-5;@;3> values can contain a range "-" and "@" no value					
				}
			}
		}
	}
	for (int row = 0; row < levelsSelectionMark_->count(); ++row)
	{
		QListWidgetItem *pWidgetItem = levelsSelectionMark_->item(row);
		if (pWidgetItem && pWidgetItem->checkState() == Qt::Checked)
		{
			QString dataSetName = pWidgetItem->text();

			markList.append(dataSetName + "&");
		}
	}
	
	plotSettings_.append(currentTask_);
	plotSettings_.append(neurList);
	plotSettings_.append(horList);
	plotSettings_.append(alList);
	plotSettings_.append(scList);
	plotSettings_.append(valList);
	plotSettings_.append(markList);
}
void ViewSelectionFrame::retrievePlotSettings()
{
	if (plotSettings_.size() > -1)
	{
		QString taskName = plotSettings_.at(0);
		//check that settigns was saved for the current task
		if (currentTask_ != taskName)
			return;
	}
	
	resetPlotsList(2);
	NoCallBack_ = false;
	if (plotSettings_.size() > 0)
	{		
		QStringList neurList = plotSettings_.at(1).split("&");
		foreach(QString strNeur, neurList)
		{
			if (strNeur != "")
			{
				QList<QListWidgetItem*> list = levelsSelectionNeur_->findItems(strNeur, Qt::MatchExactly);
				if (list.size() > 0)
					list[0]->setCheckState(Qt::Checked);
			}
		}
	}
	if (plotSettings_.size() > 1)
	{
		QStringList horList = plotSettings_.at(2).split("&");
		foreach(QString strHor, horList)
		{
			if (strHor != "")
			{
				QList<QListWidgetItem*> list = levelsSelectionHor_->findItems(strHor, Qt::MatchExactly);
				if (list.size() > 0)
					list[0]->setCheckState(Qt::Checked);
			}
		}
	}
	if (plotSettings_.size() > 2)
	{
		QStringList alList = plotSettings_.at(3).split("&");
		foreach(QString strAl, alList)
		{
			if (strAl != "")
			{
				QList<QListWidgetItem*> list = levelsSelectionAl_->findItems(strAl, Qt::MatchExactly);
				if (list.size() > 0)
				{
					list[0]->setCheckState(Qt::Checked);
					levelsSelectionAl_->setEnabled(true);
				}
			}
		}
	}
	if (plotSettings_.size() > 3)
	{
		QStringList scList = plotSettings_.at(4).split("&");
		foreach(QString strSc, scList)
		{
			if (strSc != "")
			{
				QList<QListWidgetItem*> list = levelsSelectionSc_->findItems(strSc, Qt::MatchStartsWith);
				if (list.size() > 0)
				{
					list[0]->setCheckState(Qt::Checked);
					levelsSelectionSc_->setEnabled(true);
				}
			}
		}
	}
	if (plotSettings_.size() > 4)
	{
		//by now, val list should be populated
		for (int i = 0; i < levelsSelectionVal_->count(); ++i)
		{
			levelsSelectionVal_->setEnabled(true);
			QListWidgetItem* pValWidgetItem = levelsSelectionVal_->item(i);
			if (pValWidgetItem)
			{
				lblEdit* pLineEditWidgetItem = qobject_cast<lblEdit*>(levelsSelectionVal_->itemWidget(pValWidgetItem));
				if (pLineEditWidgetItem)
				{
					QString scriptable = pLineEditWidgetItem->getLabel(); // <alignment;condition>
					QStringList valList = plotSettings_.at(5).split("&");
					foreach(QString strVal, valList) //<5499;3862//1;2-5;3>
					{
						if (strVal != "")
						{
							int n = strVal.indexOf("//");
							if (n > -1)
							{
								QString label = strVal.left(n);
								if (label == scriptable)
								{
									QString values = strVal.mid(n + 2);
									QLineEdit* lineEdit = pLineEditWidgetItem->getLineEdit();
									lineEdit->setText(values);
								}
							}
						}
					}
				}
			}
		}
	}
	if (plotSettings_.size() > 5)
	{
		QStringList markList = plotSettings_.at(6).split("&");
		foreach(QString strMark, markList)
		{
			if (strMark != "")
			{
				QList<QListWidgetItem*> list = levelsSelectionMark_->findItems(strMark, Qt::MatchExactly);
				if (list.size() > 0)
					list[0]->setCheckState(Qt::Checked);
			}
		}
	}
}
/*****************************************************
*
*			PERSISTENT SETTINGS
*
*****************************************************/
/*! \brief Stores settings from the Workstation to disk.
*
*	\details Settings for the Workstation app are stored on disk.  The QSettings object does this for us in a
*	platform independent manner.  This uses the registry in Windows, XML preference files in OSX, and ini files in Unix.
*
*	Currently, the only settings are the recently opened files.
*/

void ViewSelectionFrame::writeSettings()
{
	savePlotSettings();

	QSettings settings("Block Designs", Picto::Names->workstationAppName);
	settings.setValue("plotSettings", plotSettings_);
}

/*! \brief Reads settings for the Workstation from disk.
*
*	\details Settings for the Workstation app are stored on disk.  The QSettings object does this for us in a
*	platform independent manner.  This uses the registry in Windows, XML preference files in OSX, and ini files in Unix.
*
*	Currently, the only settings are the recently opened files. The list also contains information about the recent file
*  corresponding viewer.
*/
void ViewSelectionFrame::readSettings()
{
	QSettings settings("Block Designs", Picto::Names->workstationAppName);
	plotSettings_ = settings.value("plotSettings").value<QStringList>();

	retrievePlotSettings();
}
lblEdit::lblEdit(QString strLabel, QString strContent){
	label_ = strLabel;

	QLabel *lblName = new QLabel(strLabel);
	lineEdit_ = new QLineEdit(strContent);

	QHBoxLayout *layout = new QHBoxLayout;
	layout->addWidget(lblName);
	layout->addWidget(lineEdit_);
	setLayout(layout);
}
QString lblEdit::getLabel(){ return label_; }
QLineEdit* lblEdit::getLineEdit(){ return lineEdit_; }




