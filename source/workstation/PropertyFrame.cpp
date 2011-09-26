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
	connect(propertyFactory_.data(), SIGNAL(propertyEdited(QSharedPointer<Property>)),
        this, SLOT(propertyEdited(QSharedPointer<Property>)));
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
	//Find experiment
	QSharedPointer<Asset> experiment;
	experiment = dataStore;
	while(!experiment->inherits("Picto::Experiment"))
	{
		experiment = experiment->getParentAsset();
		Q_ASSERT(experiment);
	}
	QList<QSharedPointer<DataStore>> runtimeDescendants = dataStore->getRuntimeEditableDescendants();
	runtimeDescendants.push_front(experiment.staticCast<DataStore>());
	foreach(QSharedPointer<DataStore> runtimeDesc,runtimeDescendants)
	{
		QList<QSharedPointer<Property>> runTimeProps;
		//If its a scritable, we only want to change the initialization properties.  If its something else
		//(currently only experiment uses this) we want to change the properties immediately.
		if(runtimeDesc->inherits("Picto::Scriptable"))
			runTimeProps = runtimeDesc.staticCast<Scriptable>()->getInitPropertyContainer()->getRuntimeProperties();
		else
			runTimeProps = runtimeDesc->getPropertyContainer()->getRuntimeProperties();

		if(!runTimeProps.size())
			continue;
		QGroupBox* assetBox = new QGroupBox(runtimeDesc->getName());
		QVBoxLayout* assetLayout = new QVBoxLayout(assetBox);
		QtButtonPropertyBrowser* browser = new QtButtonPropertyBrowser();
		if(runtimeDesc->inherits("Picto::Scriptable"))
			manager = runtimeDesc.staticCast<Scriptable>()->getInitPropertyContainer()->getPropertyManager();
		else
			manager = runtimeDesc->getPropertyContainer()->getPropertyManager();
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

void PropertyFrame::propertyEdited(QSharedPointer<Property> prop)
{
	setWidget(mainWidget_);
	//Send it to director
	emit parameterMessageReady(prop);
}