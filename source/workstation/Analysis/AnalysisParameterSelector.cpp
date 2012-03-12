#include <QHBoxLayout>
#include <QSqlQuery>

#include "AnalysisParameterSelector.h"
#include "../common/globals.h"
#include "../../common/memleakdetect.h"
using namespace Picto;


AnalysisParameterSelector::AnalysisParameterSelector(QWidget *parent) :
	QWidget(parent)
{
	setupUi();

	printAssetId_ = 0;
	printAssetIsProperty_ = false;
	whenAssetId_ = 0;
	whenAssetIsProperty_ = false;
	timeSinceAssetId_ = 0;
	timeSinceIsProperty_ = false;
	timeSinceSourceTransitions_ = "";
	timeSinceDestTransitions_ = "";
}

AnalysisParameterSelector::~AnalysisParameterSelector()
{
}

//! Sets up the user interface portions of the GUI
void AnalysisParameterSelector::setupUi()
{
	// New Analysis System
	QHBoxLayout *mainLayout = new QHBoxLayout;
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
	mainLayout->addWidget(new QLabel("<b>When</b> element/property at path: "));
	mainLayout->addWidget(whenPath_);
	mainLayout->addWidget(eventType_);
	mainLayout->addWidget(new QLabel(", <b>print</b> value of property at path: "));
	mainLayout->addWidget(printPath_);
	mainLayout->addWidget(new QLabel(".  "));
	mainLayout->addWidget(shouldStampTime_);
	mainLayout->addWidget(timeSinceLabel_);
	mainLayout->addWidget(timeSince_);
	mainLayout->addWidget(timeSinceEventType_);
	
	setLayout(mainLayout);
}

int AnalysisParameterSelector::getElementId(QString path)
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
int AnalysisParameterSelector::getPropertyId(int parentId, QString name)
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

void AnalysisParameterSelector::resetQueryParameterData()
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

void AnalysisParameterSelector::getQueryParameters()
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

QString AnalysisParameterSelector::getTransitionIds(int parentId, QString name, bool asSource)
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

void AnalysisParameterSelector::updateUI()
{
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