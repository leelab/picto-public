#include <QtWidgets>
#include <QMetaObject>
#include "PropertyGroupWidget.h"
#include "../../common/storage/datastore.h"
#include "../../common/memleakdetect.h"
using namespace Picto;
;
//! [0]
PropertyGroupWidget::PropertyGroupWidget(bool trackInitVals, QSharedPointer<EditorState> editorState, QWidget *parent) :
	QWidget(parent),
	mainWidget_(NULL),
	propertyFactory_(new PropertyEditorFactory(editorState)),
	trackInitVals_(trackInitVals)
{
	connect(propertyFactory_.data(), SIGNAL(propertyEdited(QSharedPointer<Property>,QVariant)),
        this, SLOT(propertyWasEdited(QSharedPointer<Property>,QVariant)));
	setLayout(new QVBoxLayout());
	layout()->setContentsMargins(QMargins(0,0,0,0));
}

PropertyGroupWidget::~PropertyGroupWidget()
{
	clear();
}

void PropertyGroupWidget::addProperties(QString title, QVector<QSharedPointer<Property>> props)
{
	//If the main widget wasn't yet created, create it and set it up to recieve sub widgets
	if(!mainWidget_)
	{
		mainWidget_ = new QWidget();
		layout_ = new QVBoxLayout();
		mainWidget_->setLayout(layout_);
		layout()->addWidget(mainWidget_);
	}
	//Create a PropertyBrowser for these properties and set it up
	QtButtonPropertyBrowser* browser = new QtButtonPropertyBrowser();
	QtVariantPropertyManager* propManager(new QtVariantPropertyManager(mainWidget_));
	browser->setFactoryForManager(propManager, propertyFactory_.data());
	//Add each Property to the property browser one by one
	foreach(QSharedPointer<Property> prop,props)
	{
		QString propName = prop->getName();
		bool scriptProp = (propName == "EntryScript" || propName == "FrameScript" || propName == "ExitScript"
			|| propName == "AnalysisEntryScript" || propName == "AnalysisEntryScript" || propName == "AnalysisEntryScript");
		if(scriptProp)
			addScriptProperty(prop,propManager,browser);
		else
			addProperty(prop,propManager,browser);
	}

	//Add the newly created browser to a layout
	QVBoxLayout* browserLayout = new QVBoxLayout();
	browserLayout->addWidget(browser);
	browserLayout->setSizeConstraint(QLayout::SetMinAndMaxSize);//This makes sure resizing of internal widgets is handled correctly
	
	//Create a box to contain the browser, titled with the input title
	QGroupBox* browserBox = new QGroupBox(title);
	browserBox->setLayout(browserLayout);
	browserBox->setFlat(true);	//This causes only the top line of the box to be shown (saving space)
	
	//Add the browser box to the end of the mainWidget layout
	layout_->addWidget(browserBox);
	mainWidget_->setLayout(layout_);
	mainWidget_->show();
}

void PropertyGroupWidget::clear()
{
	//Disconnect all tracked properties' signals from this object
	foreach(Property* prop,propToQtPropHash_.keys())
	{
		prop->disconnect(this);
	}
	propToQtPropHash_.clear();
	propertyFactory_->clear();
	if(mainWidget_)
	{
		mainWidget_->setParent(NULL);
		delete mainWidget_;
		mainWidget_ = NULL;
	}

}

void PropertyGroupWidget::addProperty(QSharedPointer<Property> prop,QtVariantPropertyManager* manager,QtButtonPropertyBrowser* browser)
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
	propertyFactory_->setNextProperty(prop);
	browser->addProperty(item);
	if(trackInitVals_)
		connect(prop.data(),SIGNAL(initValueChanged(Property*,QVariant)),this,SLOT(propertyWasEditedExternally(Property*,QVariant)));
	else
		connect(prop.data(),SIGNAL(valueChanged(Property*,QVariant)),this,SLOT(propertyWasEditedExternally(Property*,QVariant)));
	propToQtPropHash_[prop.data()] = item;
}

void PropertyGroupWidget::addScriptProperty(QSharedPointer<Property> prop,QtVariantPropertyManager* manager,QtButtonPropertyBrowser* browser)
{
	QtVariantProperty *groupItem = NULL;
	QtVariantProperty *item = NULL;
	groupItem = manager->addProperty( QtVariantPropertyManager::groupTypeId(), prop->getName());
	item = manager->addProperty(prop->type(),"");
	groupItem->insertSubProperty(item,0);
	item->setValue(trackInitVals_?prop->initValue():prop->value());
	propertyFactory_->setNextProperty(prop);
	QtBrowserItem* browserItem = browser->addProperty(groupItem);
	if(!item->value().toString().trimmed().isEmpty())
	{	//When first opening scripts in a property bar, if they have contents, expand them.
		browser->setExpanded(browserItem,true);
	}
	if(trackInitVals_)
		connect(prop.data(),SIGNAL(initValueChanged(Property*,QVariant)),this,SLOT(propertyWasEditedExternally(Property*,QVariant)));
	else
		connect(prop.data(),SIGNAL(valueChanged(Property*,QVariant)),this,SLOT(propertyWasEditedExternally(Property*,QVariant)));
	propToQtPropHash_[prop.data()] = item;

}

void PropertyGroupWidget::propertyWasEdited(QSharedPointer<Property> prop,QVariant val)
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

void PropertyGroupWidget::propertyWasEditedExternally(Property* prop,QVariant val)
{
	Q_ASSERT(propToQtPropHash_.contains(prop));
	QtVariantProperty *qtProp = propToQtPropHash_.value(prop);
	qtProp->setValue(val);
}