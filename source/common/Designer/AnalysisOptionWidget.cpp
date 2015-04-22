#include <QCheckBox>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QToolButton>
#include <QPair>
#include <QMessageBox>
#include <QLabel>
#include <QLineEdit>
#include <QGroupBox>
#include "AnalysisOptionWidget.h"
#include "../../common/memleakdetect.h"

namespace Picto
{

AnalysisOptionWidget::AnalysisOptionWidget(QSharedPointer<EditorState> editorState, QWidget *parent) :
	QWidget(parent),
	editorState_(editorState),
	updatingList_(false),
	ignoreSelectBoxEvents_(false),
	selectedTask_(nullptr)
{
	QGroupBox* groupBox = new QGroupBox("Edit Mode");

	//----- Task Selection -------------

	taskBox_ = new QComboBox();
	taskBox_->setSizeAdjustPolicy(QComboBox::AdjustToContents);
	taskBox_->setInsertPolicy(QComboBox::NoInsert);
	taskBox_->setEditable(false);
	connect(taskBox_, SIGNAL(currentIndexChanged(int)), this, SLOT(selectedTaskIndexChanged(int)));
	connect(taskBox_, SIGNAL(editTextChanged(const QString&)), this, SLOT(selectedTaskTextChanged(const QString&)));

	deleteTaskAction_ = new QAction(QIcon(":/icons/delete.png"), tr("Delete Task"), this);
	deleteTaskAction_->setStatusTip(tr("Delete Task"));
	deleteTaskAction_->setEnabled(false);
	connect(deleteTaskAction_, SIGNAL(triggered()), this, SLOT(deleteCurrentTask()));

	QToolButton* deleteTaskButton = new QToolButton();
	deleteTaskButton->setDefaultAction(deleteTaskAction_);
	deleteTaskButton->setAutoRaise(true);

	addTaskAction_ = new QAction(QIcon(":/icons/newanalysis.svg"), tr("Add Task"), this);
	addTaskAction_->setStatusTip(tr("Add Task"));
	addTaskAction_->setEnabled(true);
	connect(addTaskAction_, SIGNAL(triggered()), this, SLOT(addTask()));

	QToolButton* addTaskButton = new QToolButton();
	addTaskButton->setDefaultAction(addTaskAction_);
	addTaskButton->setAutoRaise(true);

	QHBoxLayout* taskLabel(new QHBoxLayout());
	taskLabel->addWidget(new QLabel("Task"));
	taskLabel->addWidget(addTaskButton);
	taskLabel->addWidget(deleteTaskButton);

	QHBoxLayout* taskSelect(new QHBoxLayout());
	taskSelect->addWidget(taskBox_);
	//connect(editorState.data(), SIGNAL(taskAdded()), this, SLOT(updateTaskList()));

	QVBoxLayout* taskSelectionLayout = new QVBoxLayout();
	taskSelectionLayout->addLayout(taskLabel);
	taskSelectionLayout->addLayout(taskSelect);

	//----- Analysis Selection -------------

	selectAnalysisBox_ = new QComboBox();
	selectAnalysisBox_->setSizeAdjustPolicy(QComboBox::AdjustToContents);
	selectAnalysisBox_->setInsertPolicy(QComboBox::NoInsert);
	selectAnalysisBox_->setEditable(false);
	connect(selectAnalysisBox_, SIGNAL(currentIndexChanged(int)), this, SLOT(selectedIndexChanged(int)));
	connect(selectAnalysisBox_, SIGNAL(editTextChanged(const QString&)), this, SLOT(selectedItemTextChanged(const QString&)));

	addAnalysisAction_ = new QAction(QIcon(":/icons/newanalysis.svg"), tr("Add Analysis"),this);
	addAnalysisAction_->setStatusTip(tr("Add Analysis"));
	addAnalysisAction_->setEnabled(true);
	connect(addAnalysisAction_,SIGNAL(triggered()),this,SLOT(addAnalysis()));

	QToolButton* addAnalysisButton = new QToolButton();
	addAnalysisButton->setDefaultAction(addAnalysisAction_);
	addAnalysisButton->setAutoRaise(true);

	deleteAnalysisAction_ = new QAction(QIcon(":/icons/delete.png"), tr("Delete Analysis"), this);
    deleteAnalysisAction_->setStatusTip(tr("Delete Analysis"));
	deleteAnalysisAction_->setEnabled(false);
	connect(deleteAnalysisAction_,SIGNAL(triggered()),this,SLOT(deleteSelectedAnalysis()));

	QToolButton* deleteAnalysisButton = new QToolButton();
	deleteAnalysisButton->setDefaultAction(deleteAnalysisAction_);
	deleteAnalysisButton->setAutoRaise(true);


	QHBoxLayout *analysisLabel = new QHBoxLayout();
	analysisLabel->addWidget(new QLabel("Analysis"));
	analysisLabel->addWidget(addAnalysisButton);
	analysisLabel->addWidget(deleteAnalysisButton);

	QHBoxLayout* analysisSelect(new QHBoxLayout());
	analysisSelect->addWidget(selectAnalysisBox_);

	QVBoxLayout* analysisSelectionLayout = new QVBoxLayout();
	analysisSelectionLayout->addLayout(analysisLabel);
	analysisSelectionLayout->addLayout(analysisSelect);

	//----- Option Widget Layout --------------

	QHBoxLayout* groupBoxLayout(new QHBoxLayout());
	groupBoxLayout->addLayout(taskSelectionLayout);
	groupBoxLayout->addSpacing(10);
	groupBoxLayout->addLayout(analysisSelectionLayout);

	groupBox->setLayout(groupBoxLayout);

	QHBoxLayout* widgetLayout(new QHBoxLayout());
	widgetLayout->addWidget(groupBox);

	setLayout(widgetLayout);
}

/*! \brief Used to pass the widget the current Designer's DesignRoot.
 */
void AnalysisOptionWidget::setDesignRoot(QSharedPointer<DesignRoot> designRoot)
{
	if(!isEnabled())
		return;
	designRoot_ = designRoot;
}

/*!	\brief Finds the Analysis with matching analysisId and sets it as the currently selected item in the QComboBox.
 */
void AnalysisOptionWidget::setSelectedAnalysis(QUuid analysisId)
{
	if(!isEnabled())
		return;
	if(analysisId == QUuid())
	{
		selectAnalysisBox_->setCurrentIndex(0);
	}
	else
	{
		for (int i = 1; i<selectAnalysisBox_->count(); i++)
		{
			QUuid currAnalysisId = selectAnalysisBox_->itemData(i).toUuid();
			if (selectAnalysisBox_->itemData(i).toUuid() == analysisId)
			{
				selectAnalysisBox_->setCurrentIndex(i);
				break;
			}
		}
	}
}

/*!	\brief Finds the Analysis with matching Asset and sets it as the currently selected item in the QComboBox.
*/
void AnalysisOptionWidget::setSelectedAnalysis(QSharedPointer<Analysis> analysis)
{
	if(!isEnabled())
		return;
	QUuid id;
	if(analysis)
		id = analysis->getAssociateId();
	setSelectedAnalysis(id);
}

/*! \brief Returns the currently selected Analysis asset. 
 */
QSharedPointer<Analysis> AnalysisOptionWidget::getSelectedAnalysis()
{
	return selectedAnalysis_;
}

/*! \brief Returns the analysisId of the currently selected Analysis asset.
 */
QUuid AnalysisOptionWidget::getSelectedAnalysisId()
{
	QSharedPointer<Analysis> selectedAnalysis = getSelectedAnalysis();
	if(selectedAnalysis)
		return selectedAnalysis->getAssociateId();
	return QUuid();
}

/*! \brief Clears and rebuilds the Analysis List.
 */
void AnalysisOptionWidget::updateAnalysisList()
{
	if(!isEnabled())
		return;
	if(updatingList_)
		return;
	updatingList_ = true;
	QUuid lastAnalysisId;
	if(selectedAnalysis_)
		lastAnalysisId = selectedAnalysis_->getAssociateId();
	ignoreSelectBoxEvents_ = true;
	int newSelectionIndex = 0;
	selectAnalysisBox_->clear();
	selectAnalysisBox_->addItem("None");
	QUuid currAnalysisId;
	for(int i=0;i<designRoot_->getNumAnalyses();i++)
	{
		currAnalysisId = designRoot_->getAnalysis(i).staticCast<Analysis>()->getAssociateId();
		selectAnalysisBox_->addItem(designRoot_->getAnalysis(i)->getName(), currAnalysisId);
		if(!lastAnalysisId.isNull() && currAnalysisId == lastAnalysisId)
			newSelectionIndex = i+1;
	}

	//We set the current index to -1 so that it will cause selectedIndexChanged to get triggered when set.
	selectAnalysisBox_->setCurrentIndex(-1);
	ignoreSelectBoxEvents_ = false;
	selectAnalysisBox_->setCurrentIndex(newSelectionIndex);
	updatingList_ = false;
}

/*! \brief Adds a new analysis to the designRoot, using default values.
 */
void AnalysisOptionWidget::addAnalysis()
{
	if(!isEnabled())
		return;
	QSharedPointer<Asset> newAnalysis;

	//Add a new Analysis Design to the designRoot
	newAnalysis = designRoot_->importAnalysis("<Analysis/>");
	//Create UI Data for the new Analysis and attach it
	AssociateRootHost* assocRootHost = dynamic_cast<AssociateRootHost*>(newAnalysis.data());
	Q_ASSERT(assocRootHost);
	QUuid hostId = assocRootHost->getHostId();
	QSharedPointer<Asset> newUIData = newAnalysis->getParentAsset().staticCast<DataStore>()->createChildAsset("UIData",QString(),QString());
	QString feedback;
	newUIData.staticCast<AssociateRoot>()->LinkToAsset(newAnalysis,feedback);
	Q_ASSERT(newAnalysis);

	//Update list to include new analysis
	updateAnalysisList();

	//Select new analysis
	setSelectedAnalysis(newAnalysis.staticCast<Analysis>()->getAssociateId());

	//Select name of new analysis for editing
	selectAnalysisBox_->lineEdit()->selectAll();
}

/*! \brief Connected to the QComboBox::currentIndexChanged signal, this is called whenever a new analysis is selected.
 */
void AnalysisOptionWidget::selectedIndexChanged(int selectedIndex)
{
	if(!isEnabled())
		return;
	if(ignoreSelectBoxEvents_)
		return;

	Q_ASSERT(designRoot_);
	Q_ASSERT(selectedIndex <= designRoot_->getNumAnalyses());

	QSharedPointer<Analysis> lastCurrAnalysis = editorState_->getCurrentAnalysis();

	//If no analysis was selected
	if(selectedIndex <= 0)
	{
		selectAnalysisBox_->setEditable(false);
		deleteAnalysisAction_->setEnabled(false);

		//Erase the current selected analysis pointer
		selectedAnalysis_.clear();
	}
	else
	{
		selectAnalysisBox_->setEditable(true);
		deleteAnalysisAction_->setEnabled(true);

		//Update the current selected analysis
		selectedAnalysis_ = designRoot_->getAnalysis(selectedIndex-1).staticCast<Analysis>();
	}
	if(selectedAnalysis_ != lastCurrAnalysis)
	{
		//Tell the editorState about the new current analysis
		editorState_->setCurrentAnalysis(selectedAnalysis_);
	}
}

/*!	\brief Connected to the QComboBox::editTextChanged signal, this is called whenever the name of an analysis is changed.
 */
void AnalysisOptionWidget::selectedItemTextChanged(const QString& selectedItemText)
{
	if(!isEnabled())
		return;
	if(ignoreSelectBoxEvents_)
		return;
	int currIndex = selectAnalysisBox_->currentIndex();
	if(currIndex <= 0)
		return;
	Q_ASSERT(currIndex >= 0 && currIndex <= designRoot_->getNumAnalyses());
	if(selectedItemText != designRoot_->getAnalysis(currIndex-1)->getName())
	{
		//Update the analysis' name
		designRoot_->getAnalysis(currIndex-1).staticCast<Analysis>()->setName(selectedItemText);

		//Update the list to reflect the new name
		updateAnalysisList();
	}
}

/*! \brief Connected to the deleteAnalysisAction_, this deletes the currently selected analysis.
 *	\sa deleteAnalysisAction_
 */
void AnalysisOptionWidget::deleteSelectedAnalysis()
{
	if(!isEnabled())
		return;
	Q_ASSERT(editorState_ && editorState_->getTopLevelAsset());
	QSharedPointer<Analysis> selectedAnalysis = getSelectedAnalysis();
	if(!selectedAnalysis)
		return;

	int ret = QMessageBox::warning(this, tr("Picto"),
                                QString("Are you sure you want to delete the %1 Analysis?").arg(selectedAnalysis->getName()),
                                QMessageBox::Yes | QMessageBox::Cancel,
                                QMessageBox::Cancel);
	if(ret != QMessageBox::Yes)
		return;

	//Detach analysis from experiment
	QUuid currAnalysisId = selectedAnalysis->getAssociateId();
	editorState_->getTopLevelAsset().staticCast<DataStore>()->ClearAssociateDescendants(currAnalysisId);
	
	//Remove analysis from design
	int analysisIndexToRemove = selectAnalysisBox_->currentIndex() - 1;
	Q_ASSERT(analysisIndexToRemove < designRoot_->getNumAnalyses());
	if(!designRoot_->removeAnalysis(analysisIndexToRemove))
		Q_ASSERT(false);

	//Reset experiment so that the removal of the analysis (setDeleted() is used) takes affect.
	//This will result in updateAnalysisList() getting called.
	editorState_->triggerExperimentReset();

	//Set current analysis to none.
	setSelectedAnalysis(QUuid());
}

/*!	\brief Updates the current list of tasks.
 */
void AnalysisOptionWidget::updateTaskList()
{
	if (!isEnabled())
		return;
	if (updatingList_)
		return;

	updatingList_ = true;
	ignoreSelectBoxEvents_ = true;
	int currentIndex = 0;

	QSharedPointer<Experiment> pExperiment = designRoot_->getExperiment().objectCast<Experiment>();
	if (pExperiment)
	{
		taskBox_->clear();

		QStringList tasks = pExperiment->getTaskNames();

		for (int i = 0; i < tasks.count(); i++)
		{
			const QString &taskName = tasks[i];
			taskBox_->addItem(taskName);

			if (selectedTask_ && selectedTask_->getName() == taskName)
			{
				currentIndex = i;
			}

		}
	}

	ignoreSelectBoxEvents_ = false;
	taskBox_->setCurrentIndex(-1);
	updatingList_ = false;
	taskBox_->setCurrentIndex(currentIndex);
}

/*! \brief Bound to AnalysisOptionWidget::addTaskAction_, it creates a new task in the current experiment
 */
void AnalysisOptionWidget::addTask()
{
	if (!isEnabled())
		return;
	QSharedPointer<Task> newTask = designRoot_->createTask().objectCast<Task>();
	newTask->rename("NewTask");


	//Select new Task
	setSelectedTask(newTask);

	updateTaskList();

	//Select name of new analysis for editing
	taskBox_->lineEdit()->selectAll();
}

/*! \brief Bound to AnalysisOptionWidget::deleteTaskAction_, it deletes the current task
 */
void AnalysisOptionWidget::deleteCurrentTask()
{
	if (!isEnabled())
		return;

	int ret = QMessageBox::warning(this, tr("Picto"),
		QString("Are you sure you want to delete the %1 Task?").arg(selectedTask_->getName()),
		QMessageBox::Yes | QMessageBox::Cancel,
		QMessageBox::Cancel);

	if (ret != QMessageBox::Yes)
		return;

	QSharedPointer<Task> tempTask = selectedTask_;

	if (taskBox_->count() <= 1)
	{
		addTask();
	}
	else
	{
		int iNewIndex = (taskBox_->currentIndex() == 0) ? 1 : 0;
		selectedTaskIndexChanged(iNewIndex);

	}

	tempTask->setDeleted();

	//Reset experiment so that the removal of the Task (setDeleted() is used) takes effect.
	editorState_->triggerExperimentReset();
}

/*!	\brief Sets the current task to passed in asset.
 */
void AnalysisOptionWidget::setSelectedTask(QSharedPointer<Task> task)
{
	if (!isEnabled())
		return;
	if (task == selectedTask_)
		return;
	selectedTask_ = task;

	if (!task.isNull() && !task->getStateMachine().isNull())
	{
		//Move designer to selected task
		editorState_->setWindowAsset(task->getStateMachine(), false);
	}
	else
	{
		editorState_->setWindowAsset(designRoot_->getExperiment(), false);
	}

}

//! Returns the current Task selected for editing
QSharedPointer<Task> AnalysisOptionWidget::getSelectedTask()
{
	return selectedTask_;
}

//! Connected to the QComboBox signal emitted whenever the index changes in the taskBox_
void AnalysisOptionWidget::selectedTaskIndexChanged(int selectedIndex)
{
	if (!isEnabled())
		return;
	if (ignoreSelectBoxEvents_)
		return;
	if (updatingList_)
		return;

	//Not a true update if the list is empty
	if (taskBox_->count() == 0)
	{
		return;
	}

	//If no analysis was selected
	if (selectedIndex < 0)
	{
		taskBox_->setEditable(false);
		deleteTaskAction_->setEnabled(false);

		//Erase the current selected analysis pointer
		selectedTask_.clear();
	}
	else
	{
		taskBox_->setEditable(true);
		deleteTaskAction_->setEnabled(true);

		QString name = taskBox_->itemText(selectedIndex);

		QSharedPointer<Task> newTask = designRoot_->getExperiment().objectCast<Experiment>()->getTaskByName(name);

		//Update the current selected task
		setSelectedTask(newTask);
	}
}

//! Connected to the QComboBox signal emitted whenever the name is edited in the taskBox_
void AnalysisOptionWidget::selectedTaskTextChanged(const QString& name)
{
	if (!isEnabled())
		return;
	if (ignoreSelectBoxEvents_)
		return;
	if (updatingList_)
		return;

	QSharedPointer<Experiment> pExperiment = designRoot_->getExperiment().objectCast<Experiment>();
	if (pExperiment && name != pExperiment->getTaskNames()[taskBox_->currentIndex()])
	{
		selectedTask_->rename(name);
		taskBox_->setItemText(taskBox_->currentIndex(), name);
	}

}

}; //namespace Picto
