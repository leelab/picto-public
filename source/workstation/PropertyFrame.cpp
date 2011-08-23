#include <QtGui>
#include "PropertyFrame.h"
#include "../common/storage/datastore.h"
using namespace Picto;

//! [0]
PropertyFrame::PropertyFrame(QWidget *parent) :
	QScrollArea(parent),
	propertyFactory_(new PropertyEditorFactory()),
	mainWidget_(NULL)
{
	connect(propertyFactory_.data(), SIGNAL(propertyEdited(QSharedPointer<Picto::Property>)),
        this, SLOT(propertyEdited(QSharedPointer<Picto::Property>)));
}

void PropertyFrame::setTopLevelDataStore(QSharedPointer<DataStore> dataStore)
{
	if(mainWidget_)
	{
		delete mainWidget_;
	}
	mainWidget_ = new QWidget();
	QVBoxLayout* layout = new QVBoxLayout();
	mainWidget_->setLayout(layout);

	propertyFactory_->clear();
	if(dataStore.isNull())
		return;
	QSharedPointer<QtVariantPropertyManager> manager;
	
	QList<QSharedPointer<DataStore>> runtimeDescendants = dataStore->getRuntimeEditableDescendants();
	foreach(QSharedPointer<DataStore> runtimeDesc,runtimeDescendants)
	{
		QList<QSharedPointer<Property>> runTimeProps = runtimeDesc.staticCast<Scriptable>()->getInitPropertyContainer()->getRuntimeProperties();
		if(!runTimeProps.size())
			continue;
		QGroupBox* assetBox = new QGroupBox(runtimeDesc->getName());
		QVBoxLayout* assetLayout = new QVBoxLayout(assetBox);
		QtButtonPropertyBrowser* browser = new QtButtonPropertyBrowser();
		manager = runtimeDesc.staticCast<Scriptable>()->getInitPropertyContainer()->getPropertyManager();
		browser->setFactoryForManager(manager.data(), propertyFactory_.data());
		foreach(QSharedPointer<Property> runTimeProp, runTimeProps) 
		{
			propertyFactory_->setNextProperty(runTimeProp);
			browser->addProperty(runTimeProp->getVariantProperty().data());
		}
		assetLayout->addWidget(browser);
		layout->addWidget(assetBox);
	}
	layout->setAlignment(Qt::AlignTop);
	layout->addStretch(1);
	//mainWidget_->setFixedWidth(250);
	setWidget(mainWidget_);
	setWidgetResizable(true);
	int reqWidth = mainWidget_->sizeHint().width()+25;
	setFixedWidth((reqWidth<350)?reqWidth:350);
}

void PropertyFrame::propertyEdited(QSharedPointer<Picto::Property>)
{
	setWidget(mainWidget_);
	//Send it to director
}