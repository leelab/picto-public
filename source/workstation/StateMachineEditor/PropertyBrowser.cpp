#include <QtGui>
#include "PropertyBrowser.h"
#include "../../common/storage/datastore.h"
using namespace Picto;

//! [0]
PropertyBrowser::PropertyBrowser(QSharedPointer<EditorState> editorState,QWidget *parent) :
	editorState_(editorState),
	QtButtonPropertyBrowser(parent),
	propertyFactory_(new PropertyEditorFactory(editorState)),
	dataChanged_(false),
	lastChangedItem(NULL)
{
	connect(editorState_.data(), SIGNAL(selectedAssetChanged(QSharedPointer<Asset>)),
        this, SLOT(assetSelected(QSharedPointer<Asset>)));
	connect(propertyFactory_.data(), SIGNAL(propertyChanged()),
        this, SLOT(propertyLostFocus()));

}

void PropertyBrowser::assetSelected(QSharedPointer<Asset> asset)
{
	clear();
	if(asset.isNull())
		return;
					
	QSharedPointer<DataStore> dataStore(asset.dynamicCast<DataStore>());
	if(dataStore.isNull())
		return;
	//if(!manager_.isNull())
	//{
	//	unsetFactoryForManager(manager_.data());
	//}
	manager_ = dataStore->getPropertyContainer()->getPropertyManager();
	setFactoryForManager(manager_.data(), propertyFactory_.data());
	QMap<QString, QVector<QSharedPointer<Property>>> properties = dataStore->getPropertyContainer()->getProperties();
	for(QMap<QString, QVector<QSharedPointer<Property>>>::iterator typeIter = properties.begin();typeIter !=properties.end();typeIter ++)
	{
		for(QVector<QSharedPointer<Property>>::iterator propIter = typeIter.value().begin(); propIter != typeIter.value().end(); propIter++)
		{
			addProperty((*propIter)->getVariantProperty().data());
		}
	}
	setMinimumWidth(childrenRect().width());
}

void PropertyBrowser::propertyLostFocus()
{
	checkForUndoableOp();
}

void PropertyBrowser::mousePressEvent(QMouseEvent *mouseEvent)
{
	QWidget* child = childAt(mouseEvent->pos());
	if(child)
	{
		//Get parent widget one level below me
		while(child->parentWidget() != this)
		{
			child = child->parentWidget();
		}
		//static_cast<QFrame*>(child)->setFrameStyle(QFrame::Box);
	}
	QWidget::mousePressEvent(mouseEvent);
}

void PropertyBrowser::itemChanged (QtBrowserItem* item)
{	
	lastChangedItem = item;
}
void PropertyBrowser::checkForUndoableOp()
{
	if(!lastChangedItem)
		return;
	lastChangedItem = NULL;
	editorState_->setLastActionUndoable();
}