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

	printAssetId_ = 0;
	printAssetIsProperty_ = false;
	whenAssetId_ = 0;
	whenAssetIsProperty_ = false;
	timeSinceAssetId_ = 0;
	timeSinceIsProperty_ = false;
	timeSinceSourceTransitions_ = "";
	timeSinceDestTransitions_ = "";


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
	//inputBox_ = new QTextEdit("");
	outputBox_ = new QTextEdit("SQL results show up here...");


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

	// New Analysis System
	QHBoxLayout *queryBuilderLayout = new QHBoxLayout;
	printPath_ = new QLineEdit();
	connect(printPath_,SIGNAL(editingFinished()),this,SLOT(updateUI()));
	whenPath_ = new QLineEdit();
	connect(whenPath_,SIGNAL(editingFinished()),this,SLOT(updateUI()));
	eventType_ = new QComboBox();
	shouldStampTime_ = new QComboBox();
	shouldStampTime_->addItem("Start at first time",0);
	shouldStampTime_->addItem("Append time since",1);
	shouldStampTime_->setCurrentIndex(1);
	timeSinceLabel_ = new QLabel(" element/property at path: ");
	timeSince_ = new QLineEdit();
	connect(timeSince_,SIGNAL(editingFinished()),this,SLOT(updateUI()));
	timeSinceEventType_ = new QComboBox();
	queryBuilderLayout->addWidget(new QLabel("When element/property at path: "));
	queryBuilderLayout->addWidget(whenPath_);
	queryBuilderLayout->addWidget(eventType_);
	queryBuilderLayout->addWidget(new QLabel(", print value of property at path: "));
	queryBuilderLayout->addWidget(printPath_);
	queryBuilderLayout->addWidget(new QLabel(".  "));
	queryBuilderLayout->addWidget(shouldStampTime_);
	queryBuilderLayout->addWidget(timeSinceLabel_);
	queryBuilderLayout->addWidget(timeSince_);
	queryBuilderLayout->addWidget(timeSinceEventType_);
	
	//------ Main layout -----------
	QVBoxLayout *mainLayout = new QVBoxLayout;
	mainLayout->addLayout(toolbarLayout);
	mainLayout->addLayout(queryBuilderLayout);
	mainLayout->setStretch(1,1);
	mainLayout->addWidget(outputBox_);
	mainLayout->setStretch(2,4);

	setLayout(mainLayout);
}

int AnalysisViewer::getElementId(QString path)
{
	if(!session_.isOpen())
		return 0;
	QSqlQuery query(session_);
	query.setForwardOnly(true);
	query.prepare("SELECT assetid FROM elementlookup WHERE path=:path");
	query.bindValue(":path",path);
	bool success = query.exec();

	if(!success || !query.next())
	{
		return 0;
	}
	else
	{
		return query.value(0).toInt();		
	}
}
int AnalysisViewer::getPropertyId(int parentId, QString name)
{
	if(!session_.isOpen())
		return 0;
	QSqlQuery query(session_);
	query.setForwardOnly(true);
	query.prepare("SELECT assetid FROM propertylookup WHERE parent=:parentId AND name=:name");
	query.bindValue(":parentId",parentId);
	query.bindValue(":name",name);
	bool success = query.exec();

	if(!success || !query.next())
	{
		return 0;
	}
	else
	{
		return query.value(0).toInt();		
	}
}

void AnalysisViewer::resetQueryParameterData()
{
	printAssetId_ = 0;
	printAssetIsProperty_ = false;
	lastPrintAssetPath_ = "";
	whenAssetId_ = 0;
	lastWhenAssetPath_ = "";
	whenSourceTransitions_.clear();
	whenDestTransitions_.clear();
	whenAssetIsProperty_ = false;
	timeSinceAssetId_ = 0;
	lastTimeSinceAssetPath_ = "";
	timeSinceIsProperty_ = false;
	timeSinceSourceTransitions_.clear();
	timeSinceDestTransitions_.clear();
}

void AnalysisViewer::getQueryParameters()
{
	//Get PrintAsset Params
	QString fullPath = printPath_->text();
	QStringList splitPath;
	QString name;
	QString parentPath;
	int parentId;
	if(lastPrintAssetPath_ != fullPath)
	{
		lastPrintAssetPath_ = fullPath;
		printAssetIsProperty_ = false;
		printAssetId_ = 0;
		splitPath = fullPath.split("::");
		name = splitPath.takeLast();
		parentPath = splitPath.join("::");
		parentId = getElementId(parentPath);
		if(parentId > 0)
			printAssetId_ = getPropertyId(parentId,name);
		if(printAssetId_ <= 0)
			printAssetId_ = getElementId(fullPath);
		else
			printAssetIsProperty_ = true;
	}


	//Get WhenAsset Params
	fullPath = whenPath_->text();
	if(lastWhenAssetPath_ != fullPath)
	{
		lastWhenAssetPath_ = fullPath;
		whenAssetIsProperty_ = false;
		splitPath = fullPath.split("::");
		name = splitPath.takeLast();
		parentPath = splitPath.join("::");
		parentId = getElementId(parentPath);
		whenAssetId_ = 0;
		if(parentId > 0)
			whenAssetId_ = getPropertyId(parentId,name);
		if(whenAssetId_ <= 0)
			whenAssetId_ = getElementId(fullPath);
		else
			whenAssetIsProperty_ = true;
		if(!whenAssetIsProperty_)
		{
			whenSourceTransitions_ = getTransitionIds(parentId,name,true);
			whenDestTransitions_ = getTransitionIds(parentId,name,false);
		}
	}


	//Get TimeSinceAsset Params
	fullPath = timeSince_->text();
	if(lastTimeSinceAssetPath_ != fullPath)
	{
		lastTimeSinceAssetPath_ = fullPath;
		timeSinceIsProperty_ = false;
		splitPath = fullPath.split("::");
		name = splitPath.takeLast();
		parentPath = splitPath.join("::");
		parentId = getElementId(parentPath);
		timeSinceAssetId_ = 0;
		if(parentId > 0)
			timeSinceAssetId_ = getPropertyId(timeSinceAssetId_,name);
		if(timeSinceAssetId_ <= 0)
			timeSinceAssetId_ = getElementId(fullPath);
		else
			timeSinceIsProperty_ = true;
		if(!timeSinceIsProperty_)
		{
			timeSinceSourceTransitions_ = getTransitionIds(parentId,name,true);
			timeSinceDestTransitions_ = getTransitionIds(parentId,name,false);
		}
	}

}

QString AnalysisViewer::getTransitionIds(int parentId, QString name, bool asSource)
{
	QStringList transIds;
	if(!session_.isOpen())
		return "";
	QSqlQuery query(session_);
	query.setForwardOnly(true);
	if(asSource)
		query.prepare("SELECT assetid FROM transitionlookup WHERE parent=:parentId AND source=:name");
	else
		query.prepare("SELECT assetid FROM transitionlookup WHERE parent=:parentId AND destination=:name");

	query.bindValue(":parentId",parentId);
	query.bindValue(":name",name);
	bool success = query.exec();

	while(success && query.next())
	{
		transIds.append(query.value(0).toString());		
	}
	return transIds.join(",");
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
	resetQueryParameterData();
	updateUI();
}

void AnalysisViewer::executeCommand()
{
	progressBar_->setRange(0,0);
	if(!session_.isOpen())
	{
		outputBox_->setText("Session isn't open");
		progressBar_->setRange(0,100);
		return;
	}
	getQueryParameters();

	QSqlQuery query(session_);
	query.setForwardOnly(true);















double inArray[]={1.0,2.0,3.0,4.0,5.0,9.0,8.0,7.0,6.0};
QByteArray byteArray = QByteArray::fromRawData(reinterpret_cast<const char*>(inArray),sizeof(inArray));
QByteArray outputArray;
query.prepare("UPDATE lfp SET data=:data");
query.bindValue(":data",byteArray);
bool done = query.exec();
if(!done)
{
	outputBox_->setText("Update Query Failed: " + query.lastError().text().toAscii());
	progressBar_->setRange(0,100);
	return;
}
query.prepare("SELECT data FROM lfp LIMIT 1");
done = query.exec();
if(!done || !query.next())
{
	outputBox_->setText("Select Query Failed: " + query.lastError().text().toAscii());
	progressBar_->setRange(0,100);
	return;
}
outputArray = query.value(0).toByteArray();
const double* outArray = reinterpret_cast<const double*>(outputArray.constData());
for(int i=0;i<(sizeof(inArray)/sizeof(double));i++)
{
	outputBox_->append(QString("InArray Val: %1 ").arg(QString::number(inArray[i])));
	if(inArray[i] == outArray[i])
		outputBox_->append("matches ");
	else
		outputBox_->append("doesn't match ");
	outputBox_->append(QString("OutArray Val: %1\n").arg(QString::number(outArray[i])));
}
progressBar_->setRange(0,100);
return;















	//Get Prop Vals
	QLinkedList<PropData> propVals;
	query.prepare("SELECT p.value, p.dataid FROM properties p WHERE p.assetid=:assetid ORDER BY p.dataid");
	query.bindValue(":assetid",printAssetId_);
	bool success = query.exec();
	if(!success || !query.next())
	{
		outputBox_->setText("Query Failed: " + query.lastError().text().toAscii());
		progressBar_->setRange(0,100);
		return;
	}
	do{
		propVals.append(PropData(query.value(0).toString(),query.value(1).toLongLong()));
	} while(query.next());

	//Get PrintTime Values
	QLinkedList<TimeData> printTimeData;
	if(whenAssetIsProperty_)
	{
		query.prepare("SELECT p.dataid, f.time FROM properties p, frames f WHERE p.assetid=:assetid AND p.frameid=f.dataid ORDER BY p.dataid");
		query.bindValue(":assetid",whenAssetId_);
		bool success = query.exec();
		if(!success)
		{
			outputBox_->setText("Query Failed: " + query.lastError().text().toAscii());
			progressBar_->setRange(0,100);
			return;
		}
		while(query.next()){
			printTimeData.append(TimeData(query.value(0).toLongLong(),query.value(1).toDouble()));
		}
	}
	else
	{
		if(eventType_->itemData(eventType_->currentIndex()).toInt() == STARTS)
			query.prepare(QString("SELECT t.dataid, f.time FROM transitions t, frames f WHERE t.transid IN (%1) AND t.frameid=f.dataid ORDER BY t.dataid").arg(whenDestTransitions_));
		else
			query.prepare(QString("SELECT t.dataid, f.time FROM transitions t, frames f WHERE t.transid IN (%1) AND t.frameid=f.dataid ORDER BY t.dataid").arg(whenSourceTransitions_));
		bool success = query.exec();
		if(!success)
		{
			outputBox_->setText("Query Failed: " + query.lastError().text().toAscii());
			progressBar_->setRange(0,100);
			return;
		}
		while(query.next())
		{
			printTimeData.append(TimeData(query.value(0).toLongLong(),query.value(1).toDouble()));
		}
	}

	//Get TimeSince times
	QLinkedList<TimeData> timeSinceDataVals;

	if(timeSinceIsProperty_)
	{
		query.prepare("SELECT p.dataid, f.time FROM properties p, frames f WHERE p.assetid=:assetid AND p.frameid=f.dataid ORDER BY p.dataid");
		query.bindValue(":assetid",whenAssetId_);
		bool success = query.exec();
		if(!success)
		{
			outputBox_->setText("Query Failed: " + query.lastError().text().toAscii());
			progressBar_->setRange(0,100);
			return;
		}
		while(query.next())
		{
			timeSinceDataVals.append(TimeData(query.value(0).toLongLong(),query.value(1).toDouble()));
		}
	}
	else
	{
		if(timeSinceEventType_->itemData(timeSinceEventType_->currentIndex()).toInt() == STARTS)
			query.prepare(QString("SELECT t.dataid, f.time FROM transitions t, frames f WHERE t.transid IN (%1) AND t.frameid=f.dataid ORDER BY t.dataid").arg(timeSinceDestTransitions_));
		else
			query.prepare(QString("SELECT t.dataid, f.time FROM transitions t, frames f WHERE t.transid IN (%1) AND t.frameid=f.dataid ORDER BY t.dataid").arg(timeSinceSourceTransitions_));
		bool success = query.exec();
		if(!success)
		{
			outputBox_->setText("Query Failed: " + query.lastError().text().toAscii());
			progressBar_->setRange(0,100);
			return;
		}
		while(query.next())
		{
			timeSinceDataVals.append(TimeData(query.value(0).toLongLong(),query.value(1).toDouble()));
		}
	}

	if(timeSinceDataVals.isEmpty())
	{
		//There's no time since entry.  Just use first frame in session
		query.prepare("SELECT f.dataid, f.time FROM frames f ORDER BY f.dataid ASC LIMIT 1");;
		bool success = query.exec();
		if(!success || !query.next())
		{
			outputBox_->setText("Query Failed: " + query.lastError().text().toAscii());
			progressBar_->setRange(0,100);
			return;
		}
		timeSinceDataVals.append(TimeData(query.value(0).toLongLong(),query.value(1).toDouble()));
	}
	//Now that we know how much data we're dealing with, we can setup the progress bar
	progressBar_->setRange(0,propVals.count()+1);
	QString output;
	//We now have three lists: propVals,printTimeData,timeSinceDataVals.
	//Loop through timeSinceDataVals marking each new start time.
	Q_ASSERT(!timeSinceDataVals.empty());
	TimeData lastTimeSinceData = timeSinceDataVals.takeFirst();
	TimeData nextTimeSinceData(printTimeData.last().dataId+1,printTimeData.last().frameTime+1);
	TimeData nextPrintTimeData(0,0);
	QString currPropVal="";
	//Move to first timeSince time
	while(!printTimeData.empty() && (printTimeData.first().dataId < lastTimeSinceData.dataId))
	{
		printTimeData.takeFirst();
	}

	do{

		nextTimeSinceData = timeSinceDataVals.takeFirst();
		while(!printTimeData.empty() && (printTimeData.first().dataId < nextTimeSinceData.dataId))
		{
			nextPrintTimeData = printTimeData.takeFirst();
			while(!propVals.empty() && (propVals.first().dataId <= nextPrintTimeData.dataId))
			{
				currPropVal = propVals.takeFirst().value;
				progressBar_->setValue(progressBar_->value()+1);
			}
			output.append(currPropVal);
			if(shouldStampTime_->currentIndex())
			{
				output.append(",").append(QString::number( (1000*(nextPrintTimeData.frameTime-lastTimeSinceData.frameTime))+.5,'f',0 ));
			}
			output.append("\n");
		}
		lastTimeSinceData = nextTimeSinceData;
	}while(!timeSinceDataVals.empty());
	outputBox_->setText(output);
	progressBar_->setValue(progressBar_->maximum());


	//QString queryText = inputBox_->toPlainText();
	//QSqlQuery configQuery(configDb_);
	//configQuery.prepare("UPDATE workstationinfo SET value=:lastQuery WHERE key='LastQuery'");
	//configQuery.bindValue(":lastQuery",queryText);
	//bool success = configQuery.exec();
	//Q_ASSERT(success);
	//
	//if(!session_.isOpen())
	//	return;
	//QSqlQuery query(session_);
	//query.setForwardOnly(true);
	//query.prepare(queryText);
	//success = query.exec();

	//QString text;
	//if(!success)
	//{
	//	text = "SQL Error: " + query.lastError().text().toAscii();
	//}
	//else
	//{
	//	int colsPerRec=-1;
	//	int approxRowLength = 0;
	//	int numRecs = query.size();
	//	int arraySize = 20;
	//	int currRow = 0;
	//	QTime timer;
	//	timer.restart();
	//	while(query.next())
	//	{
	//		qDebug("A"+QString::number(timer.elapsed()).toAscii());timer.restart();
	//		if(colsPerRec < 0)
	//		{
	//			//Get information about the query output
	//			colsPerRec = query.record().count();
	//			//approxRowLength = 0;
	//			//numRecs = query.size();
	//			//for(int i=0;i<colsPerRec;i++)
	//			//{
	//			//	approxRowLength += query.record().field(i).length()+1;
	//			//}
	//			//arraySize = numRecs*approxRowLength;
	//			//text.resize(arraySize);
	//		}
	//		qDebug("B"+QString::number(timer.elapsed()).toAscii());timer.restart();
	//		for(int i=0;i<colsPerRec;i++)
	//		{
	//			//if(text.length()+20>=arraySize)
	//			//{	//Expand the array
	//			//	arraySize *= 2;
	//			//	text.reserve(arraySize);
	//			//}
	//			text.append(query.value(i).toString());
	//			if(i<colsPerRec-1)
	//			{
	//				text.append(",");
	//			}
	//		}
	//		text.append("\n");
	//		//currRow++;
	//		qDebug("C"+QString::number(timer.elapsed()).toAscii());timer.restart();
	//	}
	//	//text.append("\0");
	//	query.clear();
	//}
	//outputBox_->setText(text);
}

void AnalysisViewer::updateUI()
{
	currSessionLabel_->setText(session_.connectionName());
	progressBar_->setRange(0,100);
	progressBar_->reset();

	getQueryParameters();
	//Update eventType combo box based on whenAsset type
	if(whenAssetId_ > 0)
	{
		if(whenAssetIsProperty_ && (eventType_->count() != 1))
		{
			eventType_->clear();
			eventType_->addItem("changes",CHANGES);
		}
		else if(!whenAssetIsProperty_ && (eventType_->count() != 2))
		{
			eventType_->clear();
			eventType_->addItem("starts",STARTS);
			eventType_->addItem("ends",ENDS);
		}
		eventType_->setEnabled(true);
	}
	else
	{
		eventType_->setEnabled(false);
		eventType_->clear();
	}

	//Update timeSinceEventType_ combo box based on timeSinceAsset type
	if(timeSinceAssetId_ > 0)
	{
		if(timeSinceIsProperty_ && (timeSinceEventType_->count() != 1))
		{
			timeSinceEventType_->clear();
			timeSinceEventType_->addItem("changed",CHANGES);
		}
		else if(!timeSinceIsProperty_ && (timeSinceEventType_->count() != 2))
		{
			timeSinceEventType_->clear();
			timeSinceEventType_->addItem("started",STARTS);
			timeSinceEventType_->addItem("ended",ENDS);
		}
		timeSinceEventType_->setEnabled(true);
	}
	else
	{
		timeSinceEventType_->setEnabled(false);
		timeSinceEventType_->clear();
	}
}