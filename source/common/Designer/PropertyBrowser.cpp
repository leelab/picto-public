#include <QtWidgets>
#include "PropertyBrowser.h"
#include "../../common/storage/datastore.h"
#include "../../common/memleakdetect.h"
using namespace Picto;

//! [0]
PropertyBrowser::PropertyBrowser(QSharedPointer<EditorState> editorState,QWidget *parent) :
	QWidget(parent),
	editorState_(editorState)
{
	connect(editorState_.data(), SIGNAL(selectedAssetChanged(QSharedPointer<Asset>)),
        this, SLOT(assetSelected(QSharedPointer<Asset>)));
	connect(editorState_.data(), SIGNAL(startBarSelected(QSharedPointer<Asset>)),
        this, SLOT(startBarSelected(QSharedPointer<Asset>)));

	propGroupWidget_ = new PropertyGroupWidget(false,editorState);
	QVBoxLayout* myLayout = new QVBoxLayout();
	myLayout->addWidget(propGroupWidget_,Qt::AlignTop | Qt::AlignLeft);
	setLayout(myLayout);
	myLayout->setContentsMargins(QMargins(0,0,0,0));

	//Set up script name order
	orderedScriptNames_.append("AnalysisEntryScript");
	orderedScriptNames_.append("EntryScript");
	orderedScriptNames_.append("AnalysisFrameScript");
	orderedScriptNames_.append("FrameScript");
	orderedScriptNames_.append("ExitScript");
	orderedScriptNames_.append("AnalysisExitScript");
	//Generate lookup table of script names
	foreach(QString name,orderedScriptNames_)
		scriptNamesLookup_[name] = true;
	connect(propGroupWidget_, SIGNAL(propertyEdited(QSharedPointer<Property>,QVariant)),
        this, SLOT(propertyEdited(QSharedPointer<Property>,QVariant)));
}

void PropertyBrowser::assetSelected(QSharedPointer<Asset> asset)
{
	if(asset.isNull())
		return;
					
	QSharedPointer<DataStore> dataStore(asset.dynamicCast<DataStore>());
	if(dataStore.isNull())
		return;

	QHash<QString, QVector<QSharedPointer<Property>>> properties = dataStore->getPropertyContainer()->getProperties();
	QStringList orderedProps = dataStore->getOrderedPropertyList();
	QVector<QSharedPointer<Property>> propVec;
	foreach(QString propTag,orderedProps)
	{
		foreach(QSharedPointer<Property> prop,properties.value(propTag))
		{
			if(prop->isVisible() && !scriptNamesLookup_.contains(propTag))
				propVec << prop;
		}
	}
	propGroupWidget_->clear();
	propGroupWidget_->addProperties(dataStore->getName(),propVec);
	setSizePolicy(QSizePolicy::Maximum,QSizePolicy::Maximum);
}

void PropertyBrowser::propertyEdited(QSharedPointer<Property> prop,QVariant val)
{
	editorState_->setLastActionUndoable();
}

void PropertyBrowser::startBarSelected(QSharedPointer<Asset> asset)
{
	//Only show scripts
	if(asset.isNull())
		return;
					
	QSharedPointer<DataStore> dataStore(asset.dynamicCast<DataStore>());
	if(dataStore.isNull())
		return;

	QHash<QString, QVector<QSharedPointer<Property>>> properties = dataStore->getPropertyContainer()->getProperties();
	QVector<QSharedPointer<Property>> propVec;
	foreach(QString propTag,orderedScriptNames_)
	{
		foreach(QSharedPointer<Property> prop,properties.value(propTag))
		{
			if(prop->isVisible())
				propVec << prop;
		}
	}
	propGroupWidget_->clear();
	propGroupWidget_->addProperties(dataStore->getName(),propVec);
	setSizePolicy(QSizePolicy::Maximum,QSizePolicy::Maximum);
}