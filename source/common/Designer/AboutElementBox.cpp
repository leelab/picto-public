#include <QtWidgets>
#include "AboutElementBox.h"
#include "../../common/storage/datastore.h"
#include "../storage/SearchRequest.h"
#include "../../common/memleakdetect.h"
using namespace Picto;

/*! \brief Constructs a new AboutElementBox with the input EditorState and parent widget.
 *	\details This constructor creates a new AssetDescriber that is used to find documentation
 *	on all input Assets.  It connects to editorState's EditorState::searchRequested() and
 *	EditorState::insertionItemChanged() to get messages about when the requested serach
 *	changes and when the user has selected a new insertion item from the Designer's Toolbox.
 */
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

/*! \brief A convenience function that extract's the input Asset object's className and friendlyName and then calls elementOfInterestChanged().*/
void AboutElementBox::assetSelected(QSharedPointer<Asset> asset)
{
	if(asset.isNull())
		return;
					
	QString className = asset->metaObject()->className();
	className.replace("Picto::","");
	elementOfInterestChanged(className,asset->friendlyTypeName());
}

/*! \brief Causes the AboutElementBox to start display information about the class with the input className and friendlyName.
 *	\details Information is read from the AssetDescriber object.  This function mainly just
 *	takes this data, formats it and prints it out to the underlying SearchableTextEdit.
 */
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

/*! \brief Called whenever the requested search changes to update highlighted text in the Text box.*/
void AboutElementBox::searchRequested(SearchRequest searchRequest)
{
	if(searchRequest.type != SearchRequest::STRING)
		return;
	search(searchRequest,QColor(180,0,0,100));
}