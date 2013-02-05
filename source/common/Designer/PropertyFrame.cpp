#include <QtWidgets>
#include <QSqlQuery>
#include <QSqlError>
#include "PropertyFrame.h"
#include "../common/storage/PropertyDataUnit.h"
#include "../common/storage/datastore.h"
#include "../common/memleakdetect.h"
using namespace Picto;

//! [0]
PropertyFrame::PropertyFrame(QWidget *parent) :
	QScrollArea(parent),
	propertyFactory_(new PropertyEditorFactory()),
	mainWidget_(NULL)
{
	connect(propertyFactory_.data(), SIGNAL(propertyEdited(QSharedPointer<Property>,QVariant)),
        this, SLOT(propertyEdited(QSharedPointer<Property>,QVariant)));
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

	//Find experiment
	QSharedPointer<Asset> experiment;
	experiment = dataStore;
	while(!experiment->inherits("Picto::Experiment"))
	{
		experiment = experiment->getParentAsset();
		Q_ASSERT(experiment);
	}
	QList<QSharedPointer<DataStore>> runtimeDescendants = dataStore->getRuntimeEditableDescendants();
	if(experiment.staticCast<Picto::Experiment>()->isUIEnabled())
		runtimeDescendants.push_front(experiment.staticCast<DataStore>());
	
	
	
	
	//------------------DECOUPLE CODE----------------------------------------------------------------------
	//Build an internal property manager and use it instead of the one in the propertycontainer itself
	//as step one for property manager<->property decoupling
	propManagers_.clear();
	//------------------DECOUPLE CODE----------------------------------------------------------------------




	foreach(QSharedPointer<DataStore> runtimeDesc,runtimeDescendants)
	{
		QString parentPath = runtimeDesc->getPath();
		QHash<QString, QVector<QSharedPointer<Property>>> properties;
		QList<QSharedPointer<Property>> runTimeProps;
		properties = runtimeDesc->getUIPropertyContainer()->getProperties();
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












		//------------------DECOUPLE CODE----------------------------------------------------------------------
		//Build an internal property manager and use it instead of the one in the propertycontainer itself
		//as step one for property manager<->property decoupling
		QSharedPointer<QtVariantPropertyManager> manager = QSharedPointer<QtVariantPropertyManager>(new QtVariantPropertyManager());
		propManagers_.append(manager);
		QtVariantProperty *groupItem = manager->addProperty(QtVariantPropertyManager::groupTypeId(),
																runtimeDesc->getName());
		browser->setFactoryForManager(manager.data(), propertyFactory_.data());
		foreach(QSharedPointer<Property> runTimeProp, runTimeProps) 
		{
			QtVariantProperty *item = manager->addProperty(runTimeProp->type(),
															  runTimeProp->getName());
			item->setValue(runTimeProp->value());
			propertyFactory_->setNextProperty(runTimeProp);
			browser->addProperty(item);
		}
		assetLayout->addWidget(browser);
		layout->addWidget(assetBox);
		//------------------DECOUPLE CODE----------------------------------------------------------------------


















		//manager = runtimeDesc->getUIPropertyContainer()->getPropertyManager();
		//browser->setFactoryForManager(manager.data(), propertyFactory_.data());
		//foreach(QSharedPointer<Property> runTimeProp, runTimeProps) 
		//{
		//	propertyFactory_->setNextProperty(runTimeProp);
		//	browser->addProperty(runTimeProp->getVariantProperty());
		//}
		//assetLayout->addWidget(browser);
		//layout->addWidget(assetBox);
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
	if(!propValsSession_.isOpen())
	{
		QMessageBox msg;
		msg.setText("Error: Could not open session.");
		msg.exec();
		return;
	}

	//Query and Write Results
	QStringList missingProps;
	bool propFound = false;
	QSqlQuery query(propValsSession_);
	QHash<QString,QSharedPointer<Property>>::iterator it ;
	for(it = pathMap_.begin();it!=pathMap_.end();it++)
	{
		int lastSep = it.key().lastIndexOf("::");
		QString path = it.key().left(lastSep);
		QString name = "";
		if(lastSep >=0)
			name = it.key().mid(lastSep+2);
		query.prepare("SELECT c.data FROM currentstate c, propertylookup p, elementlookup e WHERE e.path=:path AND e.assetid = p.parent AND p.name = :name AND c.variableid = p.assetid ORDER BY c.dataid DESC LIMIT 1");
		query.bindValue(":path",path);
		query.bindValue(":name",name);
		bool success = query.exec();
		Q_ASSERT_X(success,"PropertyFrame::updatePropertiesFromFile","Error: "+query.lastError().text().toLatin1());
		if(query.next())
		{
			propFound = true;
			PropertyDataUnit unit;
			unit.fromXml(query.value(0).toString());
			QString currValue = it.value()->toUserString();
			QString newValue = unit.value_;
			it.value()->fromUserString(newValue);
			if(newValue != currValue)
				emit parameterMessageReady(it.value());
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
			warningString = "No values found in the loaded session for the following properties:\n";
			foreach(QString path,missingProps)
			{
				warningString.append(QString("- %1\n").arg(path));
			}
		}
		QMessageBox msg;
		msg.setText(warningString);
		msg.setIcon(QMessageBox::Warning);
		msg.exec();
	}
	else
	{
		QMessageBox msg;
		msg.setText("All property values for this task were updated from the input session.");
		msg.setIcon(QMessageBox::Information);
		msg.exec();
	}
}

void PropertyFrame::propertyEdited(QSharedPointer<Property> prop,QVariant val)
{
	prop->setValue(val);
	setWidget(mainWidget_);
	//Send it to director
	emit parameterMessageReady(prop);
}