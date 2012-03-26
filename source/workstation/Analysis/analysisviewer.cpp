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
	
	outputDisplay_ = new AnalysisOutputDisplay();


	analysisDef_ = new QTextEdit();


	//The toolbar
	toolBar_ = new QToolBar;

	toolBar_->addAction(executeAction_);
	toolBar_->addSeparator();
	toolBar_->addAction(loadSessionAction_);
	toolBar_->addSeparator();
	toolBar_->addWidget(currSessionLabel_);
	toolBar_->addSeparator();
	toolBar_->addWidget(progressBar_);

	QHBoxLayout *toolbarLayout = new QHBoxLayout;
	toolbarLayout->addWidget(toolBar_);
	toolbarLayout->addStretch();
	

	//------ Main layout -----------
	QVBoxLayout *mainLayout = new QVBoxLayout;
	mainLayout->addLayout(toolbarLayout);
	
	
	mainLayout->addWidget(analysisDef_);
	mainLayout->setStretch(1,1);
	mainLayout->addWidget(outputDisplay_);
	mainLayout->setStretch(2,6);

	setLayout(mainLayout);
}

void AnalysisViewer::loadSession()
{
	QString filename = QFileDialog::getOpenFileName(this,
			tr("Load Session"),".","Sqlite files (*.sqlite)");
	if(filename.isEmpty())
		return;
	//Load Sqlite Database
	int lastSlash = filename.lastIndexOf("/");
	Q_ASSERT(lastSlash >=0);
	//Check if this session is already open
	if(session_.isValid() && (session_.databaseName() == filename))
		return;

	QSqlDatabase newSession = QSqlDatabase::addDatabase("QSQLITE",filename.mid(lastSlash+1));
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
	Q_ASSERT(session_.isValid() && session_.isOpen());
	updateUI();
}

void AnalysisViewer::executeCommand()
{
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
	executeAction_->setEnabled(false);
	loadSessionAction_->setEnabled(false);
	analysisDefinition_->loadSession(session_);
	progressBar_->setRange(0,0);	//Starts progress bar busy indicator.
	QString result = analysisDefinition_->runTo(-1);
	analysisDefinition_->finish();
	progressBar_->setRange(0,100);	//Returns progress bar to normal range if it wasn't done in updateProgressBar.
	progressBar_->setValue(100);
	
	outputDisplay_->clear();
	QLinkedList<QPointer<QWidget>> outputWidgets = analysisDefinition_->getOutputWidgets();
	foreach(QPointer<QWidget> widget, outputWidgets)
	{
		outputDisplay_->addOutputTab(widget->objectName(),widget);
	}
	QCoreApplication::processEvents();	//Get rid of any multiple presses on the execute button before we reenable it.
	executeAction_->setEnabled(true);
	loadSessionAction_->setEnabled(true);
}

void AnalysisViewer::updateUI()
{
	currSessionLabel_->setText(session_.connectionName());
	progressBar_->setRange(0,100);
	progressBar_->reset();
}

void AnalysisViewer::updateProgressBar(int percentRemaining)
{
	progressBar_->setRange(0,100);
	progressBar_->setValue(100-percentRemaining);
}