#include <QtGui>
#include "PropertyBrowser.h"
#include "../../common/storage/datastore.h"
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
	QMap<QString, QVector<QSharedPointer<Property>>> properties = dataStore->getPropertyContainer()->getProperties();
	for(QMap<QString, QVector<QSharedPointer<Property>>>::iterator typeIter = properties.begin();typeIter !=properties.end();typeIter ++)
	{
		for(QVector<QSharedPointer<Property>>::iterator propIter = typeIter.value().begin(); propIter != typeIter.value().end(); propIter++)
		{
			propertyFactory_->setNextProperty(*propIter);
			addProperty((*propIter)->getVariantProperty().data());
		}
	}
	setMinimumWidth(childrenRect().width());
}

void PropertyBrowser::propertyEdited(QSharedPointer<Property>)
{
	editorState_->setLastActionUndoable();
}