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
	query.exec("SELECT value FROM workstationinfo WHERE key='LastQuery'");
	bool rc = query.next();
	if(!rc)
	{
		//inputBox_->setText("Enter SQL command here...");
		query.prepare("INSERT INTO workstationinfo (key,value) VALUES ('LastQuery',:lastQuery)");
		query.bindValue(":lastQuery",QString("Enter SQL command here..."));
		query.exec();
	}
	//else
	//	inputBox_->setText(query.value(0).toString());
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
	toolBar_->addWidget(currSessionLabel_);
	toolBar_->addSeparator();
	toolBar_->addWidget(progressBar_);
	toolBar_->addSeparator();
	toolBar_->addAction(saveOutputAction_);

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
	mainLayout->addWidget(mainTabWindow_);

	setLayout(mainLayout);
}

void AnalysisViewer::loadSession()
{
	QString filename = QFileDialog::getOpenFileName(this,
			tr("Load Session"),".","Sqlite files (*.sqlite)");
	if(filename.isEmpty())
		return;
	//Load Sqlite Database
	//Check if this session is already open
	if(session_.isValid() && (session_.databaseName() == filename))
		return;

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
	if(session_.isValid())
		session_.close();
	session_ = QSqlDatabase::database(newSession.connectionName());
	runSelector_->loadSession(session_);
	Q_ASSERT(session_.isValid() && session_.isOpen());
	updateUI();
}

void AnalysisViewer::saveOutput()
{
	QString dirName = QFileDialog::getExistingDirectory(this,
			tr("Output Data Directory"),".", QFileDialog::ShowDirsOnly);
	if(dirName.isEmpty())
		return;
	outputDisplay_->saveOutputToDirectory(dirName);
}

void AnalysisViewer::executeCommand()
{
	QTime timer;
	timer.start();
	analysisDefinition_ = QSharedPointer<AnalysisDefinition>(new AnalysisDefinition());
	connect(analysisDefinition_.data(),SIGNAL(percentRemaining(int)),this,SLOT(updateProgressBar(int)));
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
	this->setFocus();
	mainTabWindow_->setTabEnabled(1,false);
	executeAction_->setEnabled(false);
	loadSessionAction_->setEnabled(false);
	saveOutputAction_->setEnabled(false);
	runSelector_->setEnabled(false);
	analysisDef_->setEnabled(false);
	analysisDefinition_->loadSession(session_);
	outputDisplay_->clear();
	runsRemaining_ = 0;
	progressBar_->setRange(0,0);	//Starts progress bar busy indicator.
	runsRemaining_ = runSelector_->selectedRunCount();
	for(int i=0;i<runSelector_->selectedRunCount();i++)
	{
		analysisDefinition_->startNewRun(runSelector_->getSelectedRun(i));
		QString result = analysisDefinition_->run();
		analysisDefinition_->finish();
		int currTabId = outputDisplay_->addTopLevelTab(runSelector_->nameOfSelectedRun(i));
		QLinkedList<QPointer<AnalysisOutputWidget>> outputWidgets = analysisDefinition_->getOutputWidgets();
		foreach(QPointer<QWidget> widget, outputWidgets)
		{
			outputDisplay_->addSubTab(currTabId,widget->objectName(),widget);
		}
		runsRemaining_--;
	}
	progressBar_->setRange(0,100);	//Returns progress bar to normal range if it wasn't done in updateProgressBar.
	progressBar_->setValue(100);
	QCoreApplication::processEvents();	//Get rid of any multiple presses on the execute button before we reenable it.
	executeAction_->setEnabled(true);
	loadSessionAction_->setEnabled(true);
	mainTabWindow_->setTabEnabled(1,true);
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
	currSessionLabel_->setText(session_.connectionName());
	progressBar_->setRange(0,100);
	progressBar_->reset();
}

void AnalysisViewer::updateProgressBar(int percentRemaining)
{
	//The calling function sends in the percent remaining of the current run,
	//we need to update this value to reflect the portion of all runs completed.
	progressBar_->setRange(0,100);
	double totalRuns = runSelector_->selectedRunCount();
	double completed = totalRuns-runsRemaining_;
	double totalFraction = (completed+1.0-(double(percentRemaining)/100.0))/totalRuns;
	progressBar_->setValue(100.0*totalFraction);
}