#include <QLabel>
#include <QMessageBox>
#include "TaskRunViewer.h"
#include "../common/globals.h"
#include "../../common/memleakdetect.h"
using namespace Picto;

/*! \brief Constructs a new TaskRunViewer widget.
 */
TaskRunViewer::TaskRunViewer(QWidget *parent) :
	QWidget(parent)
{
	editingEnabled_ = false;

	mainLayout_ = new QVBoxLayout;
	availableRuns_ = new QComboBox();
	availableRuns_->setEditable(false);
	connect(availableRuns_,SIGNAL(currentIndexChanged(int)),this,SLOT(availableRunsIndexChanged(int)));
	runName_ = new QLineEdit();
	runNotes_ = new QTextEdit();
	runSaved_ = new QToolButton();
	runSaved_->setCheckable(true);
	connect(runSaved_,SIGNAL(toggled(bool)),this,SLOT(saveToggled(bool)));
	updateSaveButtonIcon();


	actButton_ = new QPushButton("Modify",this);
	connect(actButton_,SIGNAL(released()),this,SLOT(actTriggered()));
	actButton_->hide();
	cancelButton_ = new QPushButton("Cancel",this);
	connect(cancelButton_,SIGNAL(released()),this,SLOT(cancelTriggered()));

	QHBoxLayout* nameLayout = new QHBoxLayout;
	nameLayout->addWidget(runSaved_);
	nameLayout->addWidget(availableRuns_);
	nameLayout->addWidget(runName_);

	buttonLayout_ = new QHBoxLayout;
	buttonLayout_->addWidget(cancelButton_);
	buttonLayout_->addWidget(actButton_);

	mainLayout_->addWidget(new QLabel("Run Name"));
	mainLayout_->addLayout(nameLayout);
	mainLayout_->addWidget(new QLabel("Run Notes"));
	mainLayout_->addWidget(runNotes_);
	mainLayout_->addLayout(buttonLayout_);
	setLayout(mainLayout_);
	clear();

	setStateToReadOnly();
}

TaskRunViewer::~TaskRunViewer()
{
}

/*! \brief Sets the current table of Task Runs to the input QMap of TaskRunDataUnit objects indexed by their DataId values.
 */
void TaskRunViewer::setTaskRunData(QMap<qulonglong,QSharedPointer<Picto::TaskRunDataUnit>> dataMap)
{
	currTaskRuns_ = dataMap;
	updateComboBox();
	updateSaveButtonIcon(); //This assures that the saved/unsaved icon reflects whether the selected item is active regardless of whether
							//the operator is editing or not.
}

/*! \brief Marks the latest Task Run in the list as currently running or not running according to the input isRunning boolean.
 */
void TaskRunViewer::markLatestAsRunning(bool isRunning)
{
	bool prevRunningValue = latestIsRunning_;
	latestIsRunning_ = isRunning;
	if(prevRunningValue != latestIsRunning_)
	{
		updateComboBox();
	}
}

/*! \brief Returns the saved/unsaved icon currently being used for the latest run.
 */
QIcon TaskRunViewer::getLatestRunIcon()
{
	QSharedPointer<Picto::TaskRunDataUnit> latestRun = getLatestRun();
	if(latestRun.isNull())
		return QIcon();
	if(latestRun->saved_)
		return latestIsRunning_?QIcon(":/icons/savedrunning.png"):QIcon("://icons/filesave.png");
	else
		return latestIsRunning_?QIcon(":/icons/unsavedrunning.png"):QIcon("://icons/delete.png");
}

/*! \brief Enables/disables editing of Task Run notes, names and saved values.
 *	\details When enabled, the notes and names fields stop being read only and the save run button
 *	appears.  The button at the bottom of the window that says "Modify" when editing is disabled, says
 *	"Apply" when editing is enabled.
 */
void TaskRunViewer::enableEditing(bool enable)
{
	editingEnabled_ = enable;
	if(editingEnabled_)
		actButton_->show();
	else
		actButton_->hide();
	if(!editingEnabled_ && editing_)
		setStateToReadOnly();
}

/*! \brief Clears all data out of the Task Viewer display.
*/
void TaskRunViewer::clear()
{
	currTaskRuns_.clear();
	availableRuns_->clear();
	runName_->clear();
	runNotes_->clear();
	latestIsRunning_ = false;
}

/*! \brief Returns a TaskRunDataUnit representing the run that is currently selected in the TaskRunViewer.
 */
QSharedPointer<TaskRunDataUnit> TaskRunViewer::getSelectedRun()
{
	if(!currTaskRuns_.contains(availableRuns_->itemData(availableRuns_->currentIndex()).toLongLong()))
		return QSharedPointer<TaskRunDataUnit>();
	return currTaskRuns_.value(availableRuns_->itemData(availableRuns_->currentIndex()).toLongLong());
}

/*! \brief Returns a TaskRunDataUnit representing the latest run.
 */
QSharedPointer<Picto::TaskRunDataUnit> TaskRunViewer::getLatestRun()
{
	if(!currTaskRuns_.size())
		return QSharedPointer<Picto::TaskRunDataUnit>();
	QMap<qulonglong,QSharedPointer<Picto::TaskRunDataUnit>>::iterator it = currTaskRuns_.end()-1;
	return it.value();
}

/*! \brief Returns true if the run that is selected in the TaskRunViewer is currently active (ie. running).
 */
bool TaskRunViewer::selectedRunIsActive()
{
	QSharedPointer<Picto::TaskRunDataUnit> currRun = getSelectedRun();
	QSharedPointer<Picto::TaskRunDataUnit> latestRun = getLatestRun();
	if(latestIsRunning_ && currRun && latestRun && (currRun->getDataID() == latestRun->getDataID()))
		return true;
	return false;
}

/*! \brief Updates the TaskRuns combobox to reflect the TaskRuns that have been added in setTaskRunData().
 */
void TaskRunViewer::updateComboBox()
{
	if(editing_)
		return;
	int newSize = currTaskRuns_.size();
	int oldSize = availableRuns_->count();
	qulonglong currSelected = availableRuns_->itemData(availableRuns_->currentIndex()).toLongLong();
	int currIndex = -1;
	//Rather than simply erasing the combo box and rebuilding it from scratch, we
	//overwrite existing values if they differ.  This keeps the selection bar
	//from flashing whenever the combobox is updated.
	foreach(QSharedPointer<TaskRunDataUnit> run,currTaskRuns_)
	{
		currIndex++;
		qlonglong currDataId = run->getDataID();
		QString currName = run->name_;
		bool currSaved = run->saved_;
		if(currIndex < availableRuns_->count())
		{
			qlonglong comboBoxDataId = availableRuns_->itemData(currIndex).toLongLong();
			QString comboBoxName = availableRuns_->itemText(currIndex);
			if((currDataId == comboBoxDataId) && (currName == comboBoxName))
			{
				availableRuns_->setItemIcon(currIndex,currSaved?QIcon(":/icons/filesave.png"):QIcon(":/icons/delete.png"));
				continue;
			}
			availableRuns_->removeItem(currIndex);
		}
		
		availableRuns_->insertItem(currIndex,run->name_,run->getDataID());
		availableRuns_->setItemIcon(currIndex,currSaved?QIcon(":/icons/filesave.png"):QIcon(":/icons/delete.png"));

		if(run->getDataID() == currSelected)
		{
			availableRuns_->setCurrentIndex(currIndex);
		}
	}
	while((currIndex >= 0) && (currIndex < availableRuns_->count()-1))
		availableRuns_->removeItem(currIndex);

	if(newSize > oldSize)
		availableRuns_->setCurrentIndex(availableRuns_->count()-1);

	//If the current run is active, use a special icon for that run
	if(availableRuns_->count() && latestIsRunning_)
	{	//Since list is built up in the order of the currTaskRuns_ map which is
		//ordered by runid, the last one in the list is always the latest.
		QSharedPointer<Picto::TaskRunDataUnit> latestRun = getLatestRun();
		bool latestIsSaved = getLatestRun()->saved_;
		availableRuns_->setItemIcon(availableRuns_->count()-1,latestIsSaved?QIcon(":/icons/savedrunning.png"):QIcon(":/icons/unsavedrunning.png"));
	}
	//This may have already happened if the selected item changed, but it also may not have... 
	//we'll do it once more for good luck (and to make sure changes from one workstation are
	//reflected right away on others).
	updateFieldsFromCurrentRun();
}

/*! \brief Updates the TaskRunViewer fields (name, notes, saved) according to the latest data about the currently selected run.
 */
void TaskRunViewer::updateFieldsFromCurrentRun()
{
	if(editing_)
		return;
	QSharedPointer<TaskRunDataUnit> currRun = getSelectedRun();
	if(!currRun)
		return;
	if(currRun->name_ != runName_->text())
		runName_->setText(currRun->name_);
	if(currRun->notes_ != runNotes_->toPlainText())
		runNotes_->setText(currRun->notes_);
	runSaved_->setChecked(currRun->saved_);
	updateSaveButtonIcon();  //Putting this up here means that when a Run stops the save icon will change to the inactive version of the icon.

}

/*! \brief Updates the stored data about the currently selected run according to the values that have been set in the TaskRunViewer fields, 
 *	then emits taskRunDataChanged() with the current run's DataID to tell the outside world that the data should also be updated on the 
 *	Picto Server.
 */
void TaskRunViewer::updateCurrentRunFromFields()
{
	if(!editingEnabled_)
		return;

	QSharedPointer<TaskRunDataUnit> currRun = getSelectedRun();
	if(!currRun)
		return;

	currRun->name_ = runName_->text();
	currRun->notes_ = runNotes_->toPlainText();
	currRun->saved_ = runSaved_->isChecked();
	//Since we are sharing the task run data unit pointers with this
	//widget's owner, we can just emit this event to inform the owner
	//of the change, and the new values will be available to it in
	//its own data structures.
	emit taskRunDataChanged(currRun->getDataID());
}

/*! \brief Updates the SaveButton icon according to the current Run's saved value.
 */
void TaskRunViewer::updateSaveButtonIcon()
{
	bool selectedRunActive = selectedRunIsActive();
	if(runSaved_->isChecked())
		runSaved_->setIcon(selectedRunActive?QIcon(":/icons/savedrunning.png"):QIcon("://icons/filesave.png"));
	else
		runSaved_->setIcon(selectedRunActive?QIcon(":/icons/unsavedrunning.png"):QIcon("://icons/delete.png"));

}

/*! \brief Sets the TaskRunViewer state to editing.  Notes and name will no longer be read only.  The save button will appear
 *	and the button at the bottom of the widget will show "Apply" instead of "Modify".
 */
void TaskRunViewer::setStateToEditing()
{
	if(!editingEnabled_)
		return;

	runNotes_->setReadOnly(false);
	runNotes_->setFrameShape(QFrame::StyledPanel);
	runSaved_->show();
	cancelButton_->show();
	runName_->show();
	availableRuns_->hide();
	actButton_->setText("Apply");
	editing_ = true;
}

/*! \brief Sets the TaskRunViewer state to read only.  Notes and name will be set as read only.  The save button will dissappear
 *	and the button at the bottom of the widget will show "Modify" instead of "Apply".
 */
void TaskRunViewer::setStateToReadOnly()
{
	runNotes_->setReadOnly(true);
	runNotes_->setFrameShape(QFrame::NoFrame);
	runSaved_->hide();
	cancelButton_->hide();
	runName_->hide();
	availableRuns_->show();
	actButton_->setText("Modify");
	editing_ = false;
	updateComboBox();
}

/*! \brief Called when the button at the bottom of the TaskRunViewer is pressed.  If the state is currently read only, editing is enabled (setStateToEditing()).
 *	If the state is currently edit mode, the new values are set to the selected run, and the state returns to read only *updateCurrentRunFromFields() and  
 *	setStateToReadOnly())
 */
void TaskRunViewer::actTriggered()
{
	if(!editingEnabled_)
		return;

	if(editing_)
	{
		//Due to a bug in our current version of qt (4.5.2) discussed in http://www.qtcentre.org/threads/28361-Possilble-Bug-QXmlStreamReader
		//we might consider limiting runNotes_ size so that it is significantly less than 8192 bytes.
		//This has caused xml parsing problems in very rare cases (but if we do it here, maybe we
		//have to do it everywhere)
		//if(runNotes_->toPlainText().size() > 2000)
		//{
		//	QMessageBox box;
		//	box.setText("To Many Notes                                      ");
		//	box.setDetailedText("The notes field is currently limited to 2000 characters.");
		//	box.setIconPixmap(QPixmap(":/icons/triangle.png"));
		//	box.exec();
		//	return;
		//}
		updateCurrentRunFromFields();
		setStateToReadOnly();
		return;
	}
	updateFieldsFromCurrentRun();
	setStateToEditing();
}

/*! \brief Called when the cancel button is triggered. Returns the TaskRunViewer state from edit to read only without updating the run values
 *	from the widget fields.
 *	\details Internally, this just callss setStateToReadOnly().
 */
void TaskRunViewer::cancelTriggered()
{
	setStateToReadOnly();
}

/*! \brief Called when the Task Runs dropdown index changes. Calls updateFieldsFromCurrentRun() to update the widget fields according to the selected run.
 */
void TaskRunViewer::availableRunsIndexChanged(int)
{
	updateFieldsFromCurrentRun();
}

/*! \brief Called when save button icon is toggled.  Updates the icon on the button to reflect the current saved/unsaved state.
 */
void TaskRunViewer::saveToggled(bool)
{
	updateSaveButtonIcon();
}