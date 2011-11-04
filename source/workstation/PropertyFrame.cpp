#include <QtGui>
#include <QSqlQuery>
#include <QSqlError>
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

	pathMap_.clear();
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
		QString parentPath = runtimeDesc->getPath();
		QHash<QString, QVector<QSharedPointer<Property>>> properties;
		QList<QSharedPointer<Property>> runTimeProps;
		//If its a scritable, we only want to change the initialization properties.  If its something else
		//(currently only experiment uses this) we want to change the properties immediately.
		if(runtimeDesc->inherits("Picto::Scriptable"))
			properties = runtimeDesc.staticCast<Scriptable>()->getInitPropertyContainer()->getProperties();
		else
			properties = runtimeDesc->getPropertyContainer()->getProperties();
		QStringList orderedProps = runtimeDesc->getOrderedPropertyList();
		foreach(QString propTag,orderedProps)
		{
			QVector<QSharedPointer<Property>> propVec = properties.value(propTag);
			foreach(QSharedPointer<Property> prop,propVec)
			{
				if(prop->isRuntimeEnabled())
				{
					runTimeProps.append(prop);
					//Add prop to the pathMap so that it can be updated with saved values
					pathMap_[parentPath+"::"+prop->getName()] = prop;
				}
			}
		}
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
	setWidget(mainWidget_);
	int reqWidth = mainWidget_->sizeHint().width()+25;
	setFixedWidth((reqWidth<350)?reqWidth:350);
}

void PropertyFrame::updatePropertiesFromFile(QString filename)
{
	if(filename.isEmpty())
		return;
	//Load Sqlite Database
	int lastSlash = filename.lastIndexOf("/");
	Q_ASSERT(lastSlash >=0);
	propValsSession_ = QSqlDatabase::addDatabase("QSQLITE",filename.mid(lastSlash+1));
	propValsSession_.setDatabaseName(filename);
	propValsSession_.open();
	Q_ASSERT(propValsSession_.isOpen());

	//Query and Write Results
	QStringList missingProps;
	bool propFound = false;
	QSqlQuery query(propValsSession_);
	QHash<QString,QSharedPointer<Property>>::iterator it ;
	for(it = pathMap_.begin();it!=pathMap_.end();it++)
	{
		QString path = it.key();
		query.prepare("SELECT value FROM properties WHERE path=:path ORDER BY time DESC LIMIT 1");
		query.bindValue(":path",it.key());
		bool success = query.exec();
		Q_ASSERT_X(success,"PropertyFrame::updatePropertiesFromFile","Error: "+query.lastError().text().toAscii());
		if(query.next())
		{
			propFound = true;
			QString currValue = it.value()->toUserString();
			QString newValue = query.value(0).toString();
			it.value()->fromUserString(newValue);
			if(newValue != currValue)
				propertyEdited(it.value());
		}
		else
		{
			QStringList ancestors = it.key().split("::");
			QString propName = ancestors.takeLast();
			if(ancestors.size())
				propName.prepend("::").prepend(ancestors.takeLast());
			missingProps.append(propName);
		}

	}

	//Write Results to Properties
	if(!missingProps.isEmpty())
	{
		QString warningString("None of the properties were found in the loaded Session.");
		if(propFound)
		{
			missingProps.sort();
			QString warningString("No values found for the following properties:\n");
			foreach(QString path,missingProps)
			{
				warningString.append(QString("- %1\n").arg(path));
			}
		}
		QMessageBox msg;
		msg.setText(warningString);
		msg.exec();
	}
}

void PropertyFrame::propertyEdited(QSharedPointer<Property> prop)
{
	setWidget(mainWidget_);
	//Send it to director
	emit parameterMessageReady(prop);
}