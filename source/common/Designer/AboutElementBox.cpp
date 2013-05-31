#include <QtWidgets>
#include "AboutElementBox.h"
#include "../../common/storage/datastore.h"
#include "../storage/SearchRequest.h"
#include "../../common/memleakdetect.h"
using namespace Picto;

//! [0]
AboutElementBox::AboutElementBox(QSharedPointer<EditorState> editorState,QWidget *parent) :
	SearchableTextEdit(parent),
	editorState_(editorState),
	assetDescriber_(new AssetDescriber())
{
	setLineWrapMode(WidgetWidth);
	setTabStopWidth(16);
	setReadOnly(true);
	connect(editorState_.data(), SIGNAL(selectedAssetChanged(QSharedPointer<Asset>)),
        this, SLOT(assetSelected(QSharedPointer<Asset>)));

	//Set up search
	connect(editorState_.data(),SIGNAL(searchRequested(SearchRequest)),this,SLOT(searchRequested(SearchRequest)));
	connect(editorState_.data(),SIGNAL(insertionItemChanged(QString,QString)),this,SLOT(elementOfInterestChanged(QString,QString)));
}

void AboutElementBox::assetSelected(QSharedPointer<Asset> asset)
{
	if(asset.isNull())
		return;
					
	QString className = asset->metaObject()->className();
	className.replace("Picto::","");
	elementOfInterestChanged(className,asset->friendlyTypeName());
}

void AboutElementBox::elementOfInterestChanged(QString className,QString friendlyName)
{
	if(className.isEmpty() || friendlyName.isEmpty())
		return;
	QString description;
	QSharedPointer<AssetDescription> desc = assetDescriber_->getAssetDescription(className);
	//Q_ASSERT(desc);
	if(desc)
	{
		description.append(QString("<h3 style=\"color:red\">%1</h3>").arg(friendlyName));
		QString sectionDescrip;
		QString currDescrip;
		sectionDescrip.append(QString("<dd><span style=\"color:black;\">%2</span></dd>").arg(desc->getOverview()));
		if(!sectionDescrip.isEmpty())
		{
			description.append("<h4 style=\"color:black\">Overview</h4>");
			description.append(sectionDescrip);
		}
		sectionDescrip.clear();
		foreach(QString propName,desc->getProperties())
		{
			currDescrip = desc->getPropertyDescription(propName);
			if(!currDescrip.isEmpty())
				sectionDescrip.append(QString("<dd><span style=\"color:blue;\">%1</span> - <span style=\"color:black;\">%2</span></dd>").arg(propName).arg(currDescrip));
		}
		if(!sectionDescrip.isEmpty())
		{
			description.append("<h4 style=\"color:black\">Properties</h4>");
			description.append(sectionDescrip);
		}
		sectionDescrip.clear();
		foreach(QString scripPropName,desc->getScriptProperties())
		{
			currDescrip = desc->getScriptPropertyDescription(scripPropName);
			if(!currDescrip.isEmpty())
				sectionDescrip.append(QString("<dd><span style=\"color:blue;\">%1</span> - <span style=\"color:black;\">%2</span></dd>").arg(scripPropName).arg(currDescrip));
		}
		if(!sectionDescrip.isEmpty())
		{
			description.append("<h4 style=\"color:black\">Script Properties</h4>");
			description.append(sectionDescrip);
		}
		sectionDescrip.clear();
		foreach(QString scriptFuncName,desc->getScriptFunctions())
		{
			currDescrip = desc->getScriptFunctionDescription(scriptFuncName);
			if(!currDescrip.isEmpty())
				sectionDescrip.append(QString("<dd><span style=\"color:blue;\">%1</span> - <span style=\"color:black;\">%2</span></dd>").arg(scriptFuncName).arg(currDescrip));
		}
		if(!sectionDescrip.isEmpty())
		{
			description.append("<h4 style=\"color:black\">Script Functions</h4>");
			description.append(sectionDescrip);
		}
		sectionDescrip.clear();
	}
	else
	{
		description.append(QString("<h3 style=\"color:red\">Element is not yet documented</h3>"));
	}
	setText(description);
	
	//Search for whatever search is currently active when the AboutElementBox contents change
	foreach(SearchRequest searchRequest,editorState_->getSearchRequests())
	{
		searchRequested(searchRequest);
	}
}

void AboutElementBox::searchRequested(SearchRequest searchRequest)
{
	if(searchRequest.type != SearchRequest::STRING)
		return;
	search(searchRequest,QColor(180,0,0,100));
}