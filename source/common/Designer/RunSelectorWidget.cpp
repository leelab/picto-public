#include <QCheckBox>
#include <QVBoxLayout>
#include <QAbstractButton>
#include "RunSelectorWidget.h"
#include "../../common/memleakdetect.h"
using namespace Picto;

//! [0]
RunSelectorWidget::RunSelectorWidget(QWidget *parent) :
	QWidget(parent)
{
	scrollArea_ = new QScrollArea();
	
	selectAll_ = new QPushButton("Select All");
	selectSaved_ = new QPushButton("Select Saved");
	clearSelection_ = new QPushButton("Clear Selection");
	connect(selectAll_,SIGNAL(released()),this,SLOT(selectAll()));
	connect(selectSaved_,SIGNAL(released()),this,SLOT(selectSaved()));
	connect(clearSelection_,SIGNAL(released()),this,SLOT(clearSelection()));
	QHBoxLayout* selectButtonLayout = new QHBoxLayout();
	selectAll_->setVisible(false);
	selectSaved_->setVisible(false);
	clearSelection_->setVisible(false);
	selectButtonLayout->addWidget(selectAll_);
	selectButtonLayout->addWidget(selectSaved_);
	selectButtonLayout->addWidget(clearSelection_);

	QVBoxLayout* widgetLayout = new QVBoxLayout();
	setLayout(widgetLayout);
	widgetLayout->addLayout(selectButtonLayout);
	widgetLayout->addWidget(scrollArea_);

	mainWidget_ = new QWidget();
	buttonGroup_ = new QButtonGroup(this);
	buttonGroup_->setExclusive(false);
	connect(buttonGroup_,SIGNAL(buttonClicked(int)),this,SLOT(buttonClicked(int)));
	clear();
}

void RunSelectorWidget::addRun(bool saved,QString filePath,QString runName,int index,QString notes)
{
	//Add new buttons to layout
	if(filePath.isEmpty())
		return;
	if(notes.isEmpty())
		notes = "No saved notes";
	int lastBackslash = filePath.lastIndexOf("/");
	int lastDot = filePath.lastIndexOf(".");
	if(lastDot < 0)
		lastDot = filePath.size();
	int numChars = lastDot-(lastBackslash+1);
	QString fileName = filePath.mid(lastBackslash+1,numChars);
	if(!currGroupBox_ || currGroupBox_->title() != fileName)
	{
		currGroupBox_ = new QGroupBox(fileName);
		currGroupBox_->setLayout(new QVBoxLayout());
		qobject_cast<QVBoxLayout*>(mainWidget_->layout())->addWidget(currGroupBox_,1,Qt::AlignTop);
		if(numSessions_ > 0)
			qobject_cast<QVBoxLayout*>(mainWidget_->layout())->setStretch(numSessions_-1,0);
		numSessions_++;
	}
	QCheckBox* taskCheckbox(new QCheckBox(runName));
	taskCheckbox->setToolTip(notes);
	taskCheckbox->setIcon(saved?QIcon(":/icons/filesave.png"):QIcon("://icons/delete.png"));
	buttonIdRunLookup_[numRuns_] = QSharedPointer<RunInfo>(new RunInfo(filePath,index,saved,taskCheckbox,notes));
	fileRunLookup_[filePath][index] = buttonIdRunLookup_[numRuns_];
	buttonGroup_->addButton(taskCheckbox,numRuns_);
	qobject_cast<QVBoxLayout*>(currGroupBox_->layout())->addWidget(taskCheckbox,0,Qt::AlignTop);
	numRuns_++;
	selectAll_->setVisible(true);
	selectSaved_->setVisible(true);
	clearSelection_->setVisible(true);
	scrollArea_->setWidget(mainWidget_);
}

void RunSelectorWidget::clear()
{
	//Remove all old buttons from layout.  We do this in a roundabout but simple way.  Moving a widget's
	//layout to another widget and then destroying that widget destroys the layout and all the children that
	//were attached to it.
	if(mainWidget_->layout())
		QWidget().setLayout(mainWidget_->layout());
	mainWidget_->setLayout(new QVBoxLayout());
	qobject_cast<QVBoxLayout*>(mainWidget_->layout())->setSizeConstraint(QLayout::SetMinAndMaxSize);
	selectAll_->setVisible(false);
	selectSaved_->setVisible(false);
	clearSelection_->setVisible(false);
	QList<QAbstractButton*> managedButtons = buttonGroup_->buttons();
	foreach(QAbstractButton* button,managedButtons)
	{
		buttonGroup_->removeButton(button);
	}
	buttonIdRunLookup_.clear();
	fileRunLookup_.clear();
	numRuns_ = 0;
	numSessions_ = 0;
	currGroupBox_ = NULL;
	scrollArea_->setWidget(mainWidget_);
}

QList<int> RunSelectorWidget::getSelectedRuns(QString filePath)
{
	if(!fileRunLookup_.contains(filePath))
		return QList<int>();
	QList<int> returnList;
	QSharedPointer<RunInfo> runInfo;
	foreach(QSharedPointer<RunInfo> runInfo,fileRunLookup_[filePath].values())
	{
		if(runInfo->button_->isChecked())
		{
			returnList.append(runInfo->index_);
		}
	}
	return returnList;
}

QStringList RunSelectorWidget::getSelectedFilePaths()
{
	QStringList returnList;
	QString nextPath;
	foreach(QAbstractButton* button,buttonGroup_->buttons())
	{
		if(button->isChecked())
		{
			nextPath = buttonIdRunLookup_[buttonGroup_->id(button)]->fileName_;
			if(returnList.size() && (nextPath == returnList.last()))
				continue;
			returnList.append(buttonIdRunLookup_[buttonGroup_->id(button)]->fileName_);
		}
	}
	return returnList;
}

int RunSelectorWidget::getNumSelectedRuns()
{
	int numRuns = 0;
	foreach(QAbstractButton* button,buttonGroup_->buttons())
	{
		if(button->isChecked())
			numRuns++;
	}
	return numRuns;

}

void RunSelectorWidget::setRunStatus(QString fileName,int runIndex,RunStatus status,QString message)
{
	if(!fileRunLookup_.contains(fileName) || !fileRunLookup_[fileName].contains(runIndex))
		return;
	fileRunLookup_[fileName][runIndex]->runStatus_ = status;
	if(!message.isEmpty())
		fileRunLookup_[fileName][runIndex]->button_->setToolTip(message);
	switch(status)
	{
	case IDLE:
		setRunColor(fileName,runIndex,QColor(255,100,100));
		break;
	case INPROGRESS:
		setRunColor(fileName,runIndex,QColor(100,100,255));
		break;
	case COMPLETE:
		setRunColor(fileName,runIndex,QColor(100,255,100));
		break;
	case ERROROCCURED:
		setRunColor(fileName,runIndex,Qt::red);
		break;
	}
}

int RunSelectorWidget::getRunStatus(QString fileName,int runIndex)
{
	if(!fileRunLookup_.contains(fileName) || !fileRunLookup_[fileName].contains(runIndex))
		return IDLE;
	return fileRunLookup_[fileName][runIndex]->runStatus_;
}

void RunSelectorWidget::setRunInProgress(QString fileName,int runIndex)
{
	setRunColor(fileName,runIndex,Qt::green);
}

void RunSelectorWidget::setRunComplete(QString fileName,int runIndex)
{
	setRunColor(fileName,runIndex,Qt::blue);
}

void RunSelectorWidget::setRunError(QString fileName,int runIndex)
{
	setRunColor(fileName,runIndex,Qt::red);
}

void RunSelectorWidget::resetAllRunStatus()
{
	foreach(QSharedPointer<RunInfo> runInfo,buttonIdRunLookup_.values())
	{
		runInfo->button_->setStyleSheet("");
		runInfo->runStatus_ = RunStatus::IDLE;
		runInfo->button_->setToolTip(runInfo->notes_);
	}
}

void RunSelectorWidget::setRunColor(QString fileName,int runIndex,QColor color)
{
	if(!fileRunLookup_.contains(fileName))
		return;
	if(!fileRunLookup_[fileName].contains(runIndex))
		return;

	QWidget* button = fileRunLookup_[fileName][runIndex]->button_;
	if(!color.isValid())
		button->setStyleSheet("");
	else
		button->setStyleSheet(QString("background-color:%1;").arg(color.name()));
	button->show();
}

void RunSelectorWidget::buttonClicked(int buttonIndex)
{
	Q_ASSERT(buttonIdRunLookup_.contains(buttonIndex));
	resetAllRunStatus();
	emit runSelectionChanged();
}

void RunSelectorWidget::selectAll()
{
	bool changed = false;
	foreach(QSharedPointer<RunInfo> runInfo,buttonIdRunLookup_.values())
	{
		if(!runInfo->button_->isChecked())
		{
			changed = true;
			runInfo->button_->setChecked(true);
		}
	}
	//buttonClicked is only triggered when a user physically presses a button, so we
	//need to emit runSelectionChanged here a well.
	if(changed)
	{
		resetAllRunStatus();
		emit runSelectionChanged();
	}
}

void RunSelectorWidget::selectSaved()
{
	bool needsChange = false;
	foreach(QSharedPointer<RunInfo> runInfo,buttonIdRunLookup_.values())
	{
		if(	(!runInfo->saved_ && runInfo->button_->isChecked())
			|| (runInfo->saved_ && !runInfo->button_->isChecked())
			)
		{
			needsChange = true;
			break;
		}
	}
	if(!needsChange)
		return;
	clearSelection();
	foreach(QSharedPointer<RunInfo> runInfo,buttonIdRunLookup_.values())
	{
		if(runInfo->saved_)
			runInfo->button_->setChecked(true);
	}
	resetAllRunStatus();
	//buttonClicked is only triggered when a user physically presses a button, so we
	//need to emit runSelectionChanged here a well.
	emit runSelectionChanged();
}

void RunSelectorWidget::clearSelection()
{
	bool changed = false;
	foreach(QSharedPointer<RunInfo> runInfo,buttonIdRunLookup_.values())
	{
		if(runInfo->button_->isChecked())
		{
			runInfo->button_->setChecked(false);
			changed = true;
		}
	}
	//buttonClicked is only triggered when a user physically presses a button, so we
	//need to emit runSelectionChanged here a well.
	if(changed)
	{
		resetAllRunStatus();
		emit runSelectionChanged();
	}
}