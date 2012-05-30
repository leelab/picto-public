#include "AnalysisViewer.h"

#include "../common/globals.h"



#include <QToolBar>
#include <QAction>
#include <QMessageBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QStringList>
#include <QComboBox>
#include <QLabel>
#include <QTextEdit>
#include <QLineEdit>
#include <QSharedPointer>
#include <QMenu>
#include <QCloseEvent>
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlRecord>
#include <QFileDialog>
#include <QSqlField>
#include <QLinkedList>
#include <QProgressBar>
#include <QTabWidget>
#include "../../common/analysis/AnalysisOutputWidget.h"
#include "../../common/memleakdetect.h"
using namespace Picto;


AnalysisViewer::AnalysisViewer(QWidget *parent) :
	Viewer(parent)
{
	setupUi();


	QString dbName = "PictoWorkstation";
	dbName = dbName.toLower();
	configDb_ = QSqlDatabase::addDatabase("QSQLITE",dbName);
	configDb_.setDatabaseName(QCoreApplication::applicationDirPath() + "/" + dbName + ".config");
	configDb_.open();

	QSqlQuery query(configDb_);
	if(!configDb_.tables().contains("workstationinfo"))
	{
		query.exec("CREATE TABLE workstationinfo (key TEXT, value TEXT)");
	}
	query.exec("SELECT value FROM workstationinfo WHERE key='LastAnalysisDefinition'");
	bool rc = query.next();
	if(!rc)
	{
		//inputBox_->setText("Enter SQL command here...");
		query.prepare("INSERT INTO workstationinfo (key,value) VALUES ('LastAnalysisDefinition',:LastAnalysisDefinition)");
		query.bindValue(":LastAnalysisDefinition",QString(""));
		query.exec();
	}
	else
		analysisDef_->setText(query.value(0).toString());
}

AnalysisViewer::~AnalysisViewer()
{
}
//! Called just before displaying the viewer
void AnalysisViewer::init()
{
}

//!Called just before hiding the viewer

void AnalysisViewer::deinit()
{
}


bool AnalysisViewer::aboutToQuit()
{
	return true;
}

//! Sets up the user interface portions of the GUI
void AnalysisViewer::setupUi()
{
	//
	executeAction_ = new QAction(tr("&Start task"),this);
	executeAction_->setIcon(QIcon(":/icons/play.png"));
	connect(executeAction_,SIGNAL(triggered()),this, SLOT(executeCommand()));

	loadSessionAction_ = new QAction(tr("&Load Session"),this);
	connect(loadSessionAction_, SIGNAL(triggered()),this, SLOT(loadSession()));
	loadSessionAction_->setEnabled(true);

	currSessionLabel_ = new QLabel(tr("No Loaded Session"));

	progressBar_ = new QProgressBar();
	progressBar_->setRange(0,100);
	progressBar_->reset();

	progressBarTimer_ = new QTimer(this);
	progressBarTimer_->setInterval(100);
	progressBarTimer_->stop();
	connect(progressBarTimer_,SIGNAL(timeout()),this,SLOT(updateProgressBar()));


	saveOutputAction_ = new QAction(tr("&Save Output"),this);
	connect(saveOutputAction_, SIGNAL(triggered()),this, SLOT(saveOutput()));
	saveOutputAction_->setEnabled(false);
	
	outputDisplay_ = new AnalysisOutputDisplay();


	analysisDef_ = new QTextEdit();

	runSelector_ = new TaskRunSelector();

	//The toolbar
	toolBar_ = new QToolBar;

	toolBar_->addAction(executeAction_);
	toolBar_->addSeparator();
	toolBar_->addAction(loadSessionAction_);
	toolBar_->addSeparator();
	toolBar_->addAction(saveOutputAction_);
	toolBar_->addSeparator();
	toolBar_->addWidget(currSessionLabel_);

	QHBoxLayout *toolbarLayout = new QHBoxLayout;
	toolbarLayout->addWidget(toolBar_);
	toolbarLayout->addStretch();
	

	//------ Main layout -----------
	mainTabWindow_ = new QTabWidget();
	QVBoxLayout *runDefinitionLayout = new QVBoxLayout;
	runDefinitionLayout->addWidget(runSelector_);
	runDefinitionLayout->setStretch(1,3);
	runDefinitionLayout->addWidget(new QLabel("Enter Analysis Code:"));
	runDefinitionLayout->setStretch(2,1);
	runDefinitionLayout->addWidget(analysisDef_);
	runDefinitionLayout->setStretch(3,7);
	QWidget* runDefWidget = new QWidget();
	runDefWidget->setLayout(runDefinitionLayout);
	mainTabWindow_->addTab(runDefWidget,"Analysis Parameters");
	mainTabWindow_->addTab(outputDisplay_,"Analysis Output");
	mainTabWindow_->setTabEnabled(1,false);

	QVBoxLayout *mainLayout = new QVBoxLayout;
	mainLayout->addLayout(toolbarLayout);
	mainLayout->addWidget(progressBar_);
	mainLayout->addWidget(mainTabWindow_);

	setLayout(mainLayout);
}

void AnalysisViewer::loadSession()
{
	QStringList filenames = QFileDialog::getOpenFileNames(	this,
															tr("Select sessions"),
															".",
															"Picto Sessions(*.sqlite)");
	if(!filenames.size())
		return;
	filenames.sort();
	foreach(QSqlDatabase session,sessions_)
	{
		if(session.isValid())
			session.close();
	}
	sessions_.clear();
	foreach(QString filename,filenames)
	{
		//Load Sqlite Database
		filename = filename.replace("\\","/");
		Q_ASSERT(filename.lastIndexOf("/") >=0);
		Q_ASSERT(filename.lastIndexOf(".") >=0);
		QString connectionName = filename.mid(filename.lastIndexOf("/")+1);
		connectionName = connectionName.left(connectionName.lastIndexOf("."));

		QSqlDatabase newSession = QSqlDatabase::addDatabase("QSQLITE",connectionName);
		newSession.setDatabaseName(filename);
		newSession.open();
		if(!newSession.isOpen())
		{
			QMessageBox msg;
			msg.setText("Error: Could not open session file.");
			msg.exec();
			return;
		}
		mainTabWindow_->setTabEnabled(1,false);
		outputDisplay_->clear();
		sessions_.append(newSession);
		Q_ASSERT(sessions_.last().isValid() && sessions_.last().isOpen());
	}
	runSelector_->loadSessions(sessions_);
	
	updateUI();
}

void AnalysisViewer::saveOutput()
{
	QString dirName = QFileDialog::getExistingDirectory(this,
			tr("Output Data Directory"),".", QFileDialog::ShowDirsOnly);
	if(dirName.isEmpty())
		return;
	status_ = SAVING;
	progressBar_->setRange(0,0);	//Starts progress bar busy indicator.
	progressBarTimer_->start();
	executeAction_->setEnabled(false);
	loadSessionAction_->setEnabled(false);
	saveOutputAction_->setEnabled(false);
	outputDisplay_->saveOutputToDirectory(dirName);
	progressBarTimer_->stop();
	progressBar_->setRange(0,100);	//Returns progress bar to normal range if it wasn't done in updateProgressBar.
	progressBar_->setValue(100);
	executeAction_->setEnabled(true);
	loadSessionAction_->setEnabled(true);
	saveOutputAction_->setEnabled(true);
}

void AnalysisViewer::executeCommand()
{
	QTime timer;
	timer.start();
	analysisDefinition_ = QSharedPointer<AnalysisDefinition>(new AnalysisDefinition());
	if(!analysisDefinition_->fromXml(analysisDef_->toPlainText()))
	{
		QMessageBox box;
		box.setText("Syntax check failed                                       ");
		box.setDetailedText(Serializable::getErrors());
		box.setIconPixmap(QPixmap(":/icons/x.png"));
		box.exec();
		Serializable::clearErrors();
		return;
	}
	//Set the latest analysis definition in the workstation config file
	QSqlQuery query(configDb_);
	query.prepare("UPDATE workstationinfo SET value=:value WHERE key='LastAnalysisDefinition'");
	query.bindValue(":value",analysisDef_->toPlainText());
	query.exec();

	//Start Analysis
	status_ = ANALYZING;
	this->setFocus();
	//mainTabWindow_->setTabEnabled(1,false);
	executeAction_->setEnabled(false);
	loadSessionAction_->setEnabled(false);
	saveOutputAction_->setEnabled(false);
	runSelector_->setEnabled(false);
	analysisDef_->setEnabled(false);
	outputDisplay_->clear();
	runsRemaining_ = 0;
	progressBar_->setRange(0,0);	//Starts progress bar busy indicator.
	progressBarTimer_->start();
	runsRemaining_ = runSelector_->selectedRunCount();
	for(int i=0;i<runSelector_->selectedRunCount();i++)
	{
		analysisDefinition_->loadSession(runSelector_->getSessionForSelectedRun(i));
		analysisDefinition_->startNewRun(runSelector_->getSelectedRun(i));
		QString result = analysisDefinition_->run();
		analysisDefinition_->finish();
		int currTabId = outputDisplay_->addTopLevelTab(runSelector_->nameOfSelectedRun(i));
		QLinkedList<QPointer<AnalysisOutputWidget>> outputWidgets = analysisDefinition_->getOutputWidgets();
		foreach(QPointer<QWidget> widget, outputWidgets)
		{
			outputDisplay_->addSubTab(currTabId,widget->objectName(),widget);
			mainTabWindow_->setTabEnabled(1,true);
		}
		runsRemaining_--;
	}
	progressBarTimer_->stop();
	progressBar_->setRange(0,100);	//Returns progress bar to normal range if it wasn't done in updateProgressBar.
	progressBar_->setValue(100);
	QCoreApplication::processEvents();	//Get rid of any multiple presses on the execute button before we reenable it.
	executeAction_->setEnabled(true);
	loadSessionAction_->setEnabled(true);
	runSelector_->setEnabled(true);
	analysisDef_->setEnabled(true);
	if(outputDisplay_->supportsSaving())
	{
		saveOutputAction_->setEnabled(true);
	}
	qDebug("Total Analysis Time: " + QString::number(timer.elapsed()).toAscii());
}

void AnalysisViewer::updateUI()
{
	if(sessions_.length() == 0)
		currSessionLabel_->setText(tr("No Loaded Session"));
	else if(sessions_.length() == 1)
		currSessionLabel_->setText(sessions_[0].connectionName());
	else
		currSessionLabel_->setText(sessions_[0].connectionName()+" - " + sessions_.last().connectionName());
	progressBar_->setRange(0,100);
	progressBar_->reset();
}

void AnalysisViewer::updateProgressBar()
{
	progressBar_->setRange(0,100);
	double totalFraction = 0;
	switch(status_)
	{
	case ANALYZING:
		{
			if(!analysisDefinition_)
				return;
			int currRunPercentRemaining = analysisDefinition_->getPercentRemaining();
			//The analysisDefinition_ gives us the percent remaining of the current run,
			//we need to update this value to reflect the portion of all runs completed.
			double totalRuns = runSelector_->selectedRunCount();
			double completed = totalRuns-runsRemaining_;
			totalFraction = (completed+1.0-(double(currRunPercentRemaining)/100.0))/totalRuns;
			
		}
	break;
	case SAVING:
		{
			totalFraction = outputDisplay_->fractionOutputSaved();
		}
		break;
	}
	progressBar_->setValue(100.0*totalFraction);
}