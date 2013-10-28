#include <QSlider>
#include <QDoubleSpinBox>
#include <QVBoxLayout>
#include <QLabel>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QTimer>
#include "OutputWidgetHolder.h"
#include "../../common/parameter/AnalysisOutput.h"
#include "../../common/memleakdetect.h"
#include "SaveOutputDialog.h"


OutputWidgetHolder::OutputWidgetHolder(QWidget *parent) :
QFrame(parent)
{
	resetLayout();
	containerWidget_ = NULL;
	updateTimer_ = new QTimer(this);
	connect(updateTimer_,SIGNAL(timeout()),this,SLOT(update()));
}

OutputWidgetHolder::~OutputWidgetHolder()
{
}

void OutputWidgetHolder::newRunStarted(QUuid runId)
{
	if(runId != latestRunId_)
	{
		//Reset the layout to its initial condition
		resetLayout();
		//Update latest run id
		latestRunId_ = runId;
		//Since (due to threading) widgets may not be available immediately after run start, call update every 500ms until they are.
		updateTimer_->start(500);
	}
}

void OutputWidgetHolder::setSaveParameters(QString saveToPath,bool separateSubDirs)
{
	saveToPath_ = saveToPath;
	separateSubDirs_ = separateSubDirs;
}

void OutputWidgetHolder::saveOutput()
{
	if(!containerWidget_ || saveToPath_.isEmpty())
		return;
	containerWidget_->saveOutputTo(saveToPath_,separateSubDirs_);
}

void OutputWidgetHolder::update()
{
	containerWidget_ = Picto::AnalysisOutput::getContainerWidget(latestRunId_);
	if(containerWidget_)
	{
		layout()->addWidget(containerWidget_);
		if(containerWidget_->isSaveable())
			saveButton_->setEnabled(true);
		else
			connect(containerWidget_,SIGNAL(detectedSaveable()),this,SLOT(enableSaveButton()));
		updateTimer_->stop();
	}
}

void OutputWidgetHolder::resetLayout()
{
	//Delete last container widget layout and children (by putting it on another widget and deleting it)
	containerWidget_ = NULL;
	if(layout())
		QWidget().setLayout(layout());
	//Rebuild the layout
	QHBoxLayout* topLayout(new QHBoxLayout());
	saveButton_ = new QPushButton(QIcon("://icons/filesave.png"),"");
	saveButton_->setToolTip("Save Analysis Output");
	saveButton_->setEnabled(false);
	connect(saveButton_,SIGNAL(released()),this,SLOT(saveOutputFromDialog()));
	topLayout->addWidget(saveButton_,0,Qt::AlignRight | Qt::AlignTop);
	QVBoxLayout* layout(new QVBoxLayout());
	layout->addLayout(topLayout);
	setLayout(layout);
}

void OutputWidgetHolder::saveOutputFromDialog()
{
	//Restore dialog values
	QString dirName = ".";
	bool useSeperateSubDirs = true;
	//if(configDb_.isOpen())
	//{
	//	QSqlQuery query(configDb_);
	//	query.exec(QString("SELECT key,value FROM workstationinfo WHERE key IN ('OutputPath','SeperateDirs')"));
	//	while(query.next())
	//	{
	//		if(query.value(0) == "OutputPath")
	//			dirName = query.value(1).toString();
	//		else if(query.value(0) == "SeperateDirs")
	//			useSeperateSubDirs = query.value(1).toBool();
	//	}
	//}
	SaveOutputDialog saveDialog(this,dirName,useSeperateSubDirs);
	saveDialog.showDialog();
	//savedOutputBoxState_ = saveDialog.getCurrentState();
	dirName = saveDialog.getSelectedDir();
	if(dirName.isEmpty())
		return;
	useSeperateSubDirs = saveDialog.useSeperateSubDirs();
	setSaveParameters(dirName,useSeperateSubDirs);
	saveOutput();
}

void OutputWidgetHolder::enableSaveButton()
{
	if(saveButton_)
		saveButton_->setEnabled(true);
}