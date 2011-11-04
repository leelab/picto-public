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
#include <QSharedPointer>
#include <QMenu>
#include <QCloseEvent>
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlRecord>
#include <QFileDialog>
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
		inputBox_->setText("Enter SQL command here...");
		query.prepare("INSERT INTO workstationinfo (key,value) VALUES ('LastQuery',:lastQuery)");
		query.bindValue(":lastQuery",QString("Enter SQL command here..."));
		query.exec();
	}
	else
		inputBox_->setText(query.value(0).toString());

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
	inputBox_ = new QTextEdit("");
	outputBox_ = new QTextEdit("SQL results show up here...");


	//The toolbar
	toolBar_ = new QToolBar;

	toolBar_->addAction(executeAction_);
	toolBar_->addSeparator();
	toolBar_->addAction(loadSessionAction_);
	toolBar_->addSeparator();
	toolBar_->addWidget(currSessionLabel_);

	QHBoxLayout *toolbarLayout = new QHBoxLayout;
	toolbarLayout->addWidget(toolBar_);
	toolbarLayout->addStretch();
	
	//------ Main layout -----------
	QVBoxLayout *mainLayout = new QVBoxLayout;
	mainLayout->addLayout(toolbarLayout);
	mainLayout->addWidget(inputBox_);
	mainLayout->setStretch(1,1);
	mainLayout->addWidget(outputBox_);
	mainLayout->setStretch(2,4);
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
	session_ = QSqlDatabase::addDatabase("QSQLITE",filename.mid(lastSlash+1));
	session_.setDatabaseName(filename);
	session_.open();
	if(!session_.isOpen())
	{
		QMessageBox msg;
		msg.setText("Error: Could not open session file.");
		msg.exec();
		return;
	}
	currSessionLabel_->setText(filename.mid(lastSlash+1));
}

void AnalysisViewer::executeCommand()
{
	QString queryText = inputBox_->toPlainText();
	QSqlQuery configQuery(configDb_);
	configQuery.prepare("UPDATE workstationinfo SET value=:lastQuery WHERE key='LastQuery'");
	configQuery.bindValue(":lastQuery",queryText);
	bool success = configQuery.exec();
	Q_ASSERT(success);
	
	if(!session_.isOpen())
		return;
	QSqlQuery query(session_);
	query.prepare(queryText);
	success = query.exec();

	QString output;
	if(!success)
	{
		output = "SQL Error: " + query.lastError().text().toAscii();
	}
	else
	{
		while(query.next())
		{
			QSqlRecord currRec = query.record();
			for(int i=0;i<currRec.count();i++)
			{
				output.append(currRec.value(i).toString());
				if(i<currRec.count()-1)
					output.append(",");
			}
			output.append("\n");
		}
	}
	outputBox_->setText(output);
}