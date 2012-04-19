#include <QLabel>
#include <QMessageBox>
#include "TaskRunViewer.h"
#include "../common/globals.h"
#include "../../common/memleakdetect.h"
using namespace Picto;


TaskRunViewer::TaskRunViewer(QWidget *parent) :
	QWidget(parent)
{
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

void TaskRunViewer::setTaskRunData(QMap<qulonglong,QSharedPointer<Picto::TaskRunDataUnit>> dataMap)
{
	currTaskRuns_ = dataMap;
	updateComboBox();
	updateSaveButtonIcon(); //This assures that the saved/unsaved icon reflects whether the selected item is active regardless of whether
							//the operator is editing or not.
}

void TaskRunViewer::markLatestAsRunning(bool isRunning)
{
	bool prevRunningValue = latestIsRunning_;
	latestIsRunning_ = isRunning;
	if(prevRunningValue != latestIsRunning_)
	{
		updateComboBox();
	}
}

void TaskRunViewer::clear()
{
	currTaskRuns_.clear();
	availableRuns_->clear();
	runName_->clear();
	runNotes_->clear();
	latestIsRunning_ = false;
}

QSharedPointer<TaskRunDataUnit> TaskRunViewer::getSelectedRun()
{
	if(!currTaskRuns_.contains(availableRuns_->itemData(availableRuns_->currentIndex()).toLongLong()))
		return QSharedPointer<TaskRunDataUnit>();
	return currTaskRuns_.value(availableRuns_->itemData(availableRuns_->currentIndex()).toLongLong());
}

QSharedPointer<Picto::TaskRunDataUnit> TaskRunViewer::getLatestRun()
{
	if(!currTaskRuns_.size())
		return QSharedPointer<Picto::TaskRunDataUnit>();
	QMap<qulonglong,QSharedPointer<Picto::TaskRunDataUnit>>::iterator it = currTaskRuns_.end()-1;
	return it.value();
}

bool TaskRunViewer::selectedRunIsActive()
{
	QSharedPointer<Picto::TaskRunDataUnit> currRun = getSelectedRun();
	QSharedPointer<Picto::TaskRunDataUnit> latestRun = getLatestRun();
	if(latestIsRunning_ && currRun && latestRun && (currRun->getDataID() == latestRun->getDataID()))
		return true;
	return false;
}

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

void TaskRunViewer::updateCurrentRunFromFields()
{
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

void TaskRunViewer::updateSaveButtonIcon()
{
	bool selectedRunActive = selectedRunIsActive();
	if(runSaved_->isChecked())
		runSaved_->setIcon(selectedRunActive?QIcon(":/icons/savedrunning.png"):QIcon("://icons/filesave.png"));
	else
		runSaved_->setIcon(selectedRunActive?QIcon(":/icons/unsavedrunning.png"):QIcon("://icons/delete.png"));

}

void TaskRunViewer::setStateToEditing()
{
	runNotes_->setReadOnly(false);
	runNotes_->setFrameShape(QFrame::StyledPanel);
	runSaved_->show();
	cancelButton_->show();
	runName_->show();
	availableRuns_->hide();
	actButton_->setText("Apply");
	editing_ = true;
}
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

void TaskRunViewer::actTriggered()
{
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

void TaskRunViewer::cancelTriggered()
{
	setStateToReadOnly();
}

void TaskRunViewer::availableRunsIndexChanged(int)
{
	updateFieldsFromCurrentRun();
}

void TaskRunViewer::saveToggled(bool)
{
	updateSaveButtonIcon();
}