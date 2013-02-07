#include <QtWidgets>
#include "PropertyGroupWidget.h"
#include "../../common/storage/datastore.h"
#include "../../common/memleakdetect.h"
using namespace Picto;

//! [0]
PropertyGroupWidget::PropertyGroupWidget(QWidget *parent) :
	QWidget(parent),
	mainWidget_(NULL),
	propertyFactory_(new PropertyEditorFactory())
{
	connect(propertyFactory_.data(), SIGNAL(propertyEdited(QSharedPointer<Property>,QVariant)),
        this, SLOT(propertyWasEdited(QSharedPointer<Property>,QVariant)));
	setLayout(new QVBoxLayout());
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
		int propertyType = prop->type();
		if(propertyType == PropertyContainer::enumTypeId())
			propertyType = QtVariantPropertyManager::enumTypeId();
		QtVariantProperty *item = propManager->addProperty(propertyType,
														prop->getName());
		foreach(QString attr,prop->getAttributes())
		{
			item->setAttribute(attr,prop->attributeValue(attr));
		}
		item->setValue(prop->value());
		propertyFactory_->setNextProperty(prop);
		browser->addProperty(item);
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
	propertyFactory_->clear();
	if(mainWidget_)
	{
		mainWidget_->setParent(NULL);
		delete mainWidget_;
		mainWidget_ = NULL;
	}

}

void PropertyGroupWidget::propertyWasEdited(QSharedPointer<Property> prop,QVariant val)
{
	prop->setValue(val);
	emit propertyEdited(prop,val);
}