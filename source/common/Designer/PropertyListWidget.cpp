#include <QtWidgets>
#include <QMetaObject>
#include "PropertyListWidget.h"
#include "../../common/storage/datastore.h"
#include "../../common/memleakdetect.h"
using namespace Picto;
;
//! [0]
PropertyListWidget::PropertyListWidget(bool trackInitVals, QSharedPointer<EditorState> editorState, QWidget *parent) :
	QWidget(parent),
	mainWidget_(NULL),
	propertyFactory_(editorState),
	trackInitVals_(trackInitVals),
	browser_(new QtButtonPropertyBrowser(this))
{
	connect(&propertyFactory_, SIGNAL(propertyEdited(QSharedPointer<Property>,QVariant)),
        this, SLOT(propertyWasEdited(QSharedPointer<Property>,QVariant)));
	setLayout(new QVBoxLayout());
	layout()->addWidget(browser_);
	layout()->setSizeConstraint(QLayout::SetMinAndMaxSize);//This makes sure resizing of internal widgets is handled correctly
	layout()->setContentsMargins(QMargins(0,0,0,0));

	//Set up the property browser
	browser_->setFactoryForManager(&propManager_, &propertyFactory_);
}

PropertyListWidget::~PropertyListWidget()
{
	browser_->unsetFactoryForManager(&propManager_);
	browser_->clear();
	propManager_.clear();
	propertyFactory_.clear();
}

void PropertyListWidget::addProperties(QString title, QVector<QSharedPointer<Property>> props)
{
	//Add each Property to the property browser one by one
	foreach(QSharedPointer<Property> prop,props)
	{
		QString propName = prop->getName();
		bool scriptProp = (propName == "EntryScript" || propName == "FrameScript" || propName == "ExitScript"
			|| propName == "AnalysisEntryScript" || propName == "AnalysisFrameScript" || propName == "AnalysisExitScript");
		if(scriptProp)
			addScriptProperty(prop,&propManager_,browser_);
		else
			addProperty(prop,&propManager_,browser_);
	}

	//Show the changes
	show();
}

void PropertyListWidget::addProperty(QSharedPointer<Property> prop,QtVariantPropertyManager* manager,QtAbstractPropertyBrowser* browser)
{
	int propertyType = prop->type();
	if(propertyType == PropertyContainer::enumTypeId())
		propertyType = QtVariantPropertyManager::enumTypeId();
	QString propName = prop->getName();
	QtVariantProperty *item = NULL;
	item = manager->addProperty(propertyType,prop->getName());
	foreach(QString attr,prop->getAttributes())
	{
		item->setAttribute(attr,prop->attributeValue(attr));
	}
	item->setValue(trackInitVals_?prop->initValue():prop->value());
	propertyFactory_.setNextProperty(prop);
	browser->addProperty(item);
	if(trackInitVals_)
		connect(prop.data(),SIGNAL(initValueChanged(Property*,QVariant)),this,SLOT(propertyWasEditedExternally(Property*,QVariant)));
	else
		connect(prop.data(),SIGNAL(valueChanged(Property*,QVariant)),this,SLOT(propertyWasEditedExternally(Property*,QVariant)));
	propToQtPropHash_[prop.data()] = QPair<QtVariantProperty*,QSharedPointer<Property>>(item,prop);
}

void PropertyListWidget::addScriptProperty(QSharedPointer<Property> prop,QtVariantPropertyManager* manager,QtAbstractPropertyBrowser* browser)
{
	QtVariantProperty *groupItem = NULL;
	QtVariantProperty *item = NULL;
	groupItem = manager->addProperty( QtVariantPropertyManager::groupTypeId(), prop->getName());
	item = manager->addProperty(prop->type(),"");
	groupItem->insertSubProperty(item,0);
	item->setValue(trackInitVals_?prop->initValue():prop->value());
	propertyFactory_.setNextProperty(prop);
	QtBrowserItem* browserItem = browser->addProperty(groupItem);
	if(!item->value().toString().trimmed().isEmpty())
	{	//When first opening scripts in a property bar, if they have contents, expand them.
		Q_ASSERT(dynamic_cast<QtButtonPropertyBrowser*>(browser));
		dynamic_cast<QtButtonPropertyBrowser*>(browser)->setExpanded(browserItem,true);
	}
	if(trackInitVals_)
		connect(prop.data(),SIGNAL(initValueChanged(Property*,QVariant)),this,SLOT(propertyWasEditedExternally(Property*,QVariant)));
	else
		connect(prop.data(),SIGNAL(valueChanged(Property*,QVariant)),this,SLOT(propertyWasEditedExternally(Property*,QVariant)));
	propToQtPropHash_[prop.data()] = QPair<QtVariantProperty*,QSharedPointer<Property>>(item,prop);

}

void PropertyListWidget::propertyWasEdited(QSharedPointer<Property> prop,QVariant val)
{
	//We use QMetaObject::invokeMethod() below instead of calling the methods directly
	//so that we won't encounter any multithreaded write issues.  By using this method,
	//if the properties are in another thread, the function call will be scheduled to
	//run in that thread's event loop automatically.  If the property is in the current
	//thread, the function will be called immediately.  This is one of the few places
	//where the UI thread and an experiment that might possibly in a separate thread need
	//to communicate.
	if(trackInitVals_)
		QMetaObject::invokeMethod(prop.data(),"setInitValue",Q_ARG(QVariant,val));
	else
		QMetaObject::invokeMethod(prop.data(),"setValue",Q_ARG(QVariant,val));
	emit propertyEdited(prop,val);
}

void PropertyListWidget::propertyWasEditedExternally(Property* prop,QVariant val)
{
	Q_ASSERT(propToQtPropHash_.contains(prop));
	QtVariantProperty *qtProp = propToQtPropHash_.value(prop).first;
	Q_ASSERT(browser_);
	browser_->setCurrentItem(NULL);
	qtProp->setValue(val);
}