#include <QtGui>
#include "PropertyBrowser.h"
#include "../../common/storage/datastore.h"
#include "../../common/memleakdetect.h"
using namespace Picto;

//! [0]
PropertyBrowser::PropertyBrowser(QSharedPointer<EditorState> editorState,QWidget *parent) :
	editorState_(editorState),
	QtButtonPropertyBrowser(parent),
	propertyFactory_(new PropertyEditorFactory())
{
	connect(editorState_.data(), SIGNAL(selectedAssetChanged(QSharedPointer<Asset>)),
        this, SLOT(assetSelected(QSharedPointer<Asset>)));
	//connect(propertyFactory_.data(), SIGNAL(propertyChanged(QtProperty*,bool)),
 //       this, SLOT(propertyFocusChanged(QtProperty*,bool)));
	connect(propertyFactory_.data(), SIGNAL(propertyEdited(QSharedPointer<Property>)),
        this, SLOT(propertyEdited(QSharedPointer<Property>)));
}

void PropertyBrowser::assetSelected(QSharedPointer<Asset> asset)
{
	clear();
	propertyFactory_->clear();
	if(asset.isNull())
		return;
					
	QSharedPointer<DataStore> dataStore(asset.dynamicCast<DataStore>());
	if(dataStore.isNull())
		return;
	//if(!manager_.isNull())
	//{
	//	unsetFactoryForManager(manager_.data());
	//}
	QSharedPointer<QtVariantPropertyManager> manager = dataStore->getPropertyContainer()->getPropertyManager();
	setFactoryForManager(manager.data(), propertyFactory_.data());
	QHash<QString, QVector<QSharedPointer<Property>>> properties = dataStore->getPropertyContainer()->getProperties();
	QStringList orderedProps = dataStore->getOrderedPropertyList();
	foreach(QString propTag,orderedProps)
	{
		QVector<QSharedPointer<Property>> propVec = properties.value(propTag);
		foreach(QSharedPointer<Property> prop,propVec)
		{
			propertyFactory_->setNextProperty(prop);
			addProperty(prop->getVariantProperty());
		}
	}
	setMinimumWidth(childrenRect().width());
}

void PropertyBrowser::propertyEdited(QSharedPointer<Property> prop)
{
	editorState_->setLastActionUndoable();

	//Set the property value to its corresponding InitProperty if its scriptable
	QSharedPointer<Asset> currAsset = editorState_->getSelectedAsset();
	Q_ASSERT(currAsset);
	if(!currAsset->inherits("Picto::Scriptable"))
		return;
	QSharedPointer<Scriptable> currScriptable = currAsset.staticCast<Scriptable>();
	QSharedPointer<PropertyContainer> currInitPropContainer = currScriptable->getInitPropertyContainer();
	QSharedPointer<Property> currInitProp = currInitPropContainer->getProperty(prop->identifier());
	Q_ASSERT_X(currInitProp
		,"PropertyBrowser::propertyEdited"
		,"There is no InitProperty corresponding to the edited " + prop->identifier().toAscii() + " property");
	currInitProp->setValue(prop->value());
}