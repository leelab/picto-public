#include <QtWidgets>
#include <QSqlQuery>
#include <QSqlError>
#include "PropertyFrame.h"
#include "../common/storage/PropertyDataUnit.h"
#include "../common/storage/datastore.h"
#include "../parameter/Parameter.h"
#include "../common/memleakdetect.h"
using namespace Picto;


PropertyFrame::PropertyFrame(QWidget *parent) :
	QScrollArea(parent),
	mainWidget_(NULL)
{
	setWidgetResizable(true);
}

/*! \brief Creates widgets for all RuntimeEditable Property object descendants of the input dataStore and lays them out, one after another, 
 *	grouped according to their parent Design Elements.
 *	\details The function goes through the input dataStore and searches for RuntimeEditable Property descendants.  When tney are found, they are grouped
 *	by parent Design Element and added to a PropertyGroupWidget, which handles creating of the appropriate Property Widgets and laying them out in groups 
 *	titled according to the DesignElement name.
 */
void PropertyFrame::setTopLevelDataStore(QSharedPointer<DataStore> dataStore)
{
	if(mainWidget_)
	{
		delete mainWidget_;
	}
	mainWidget_ = new QWidget();
	QVBoxLayout* layout = new QVBoxLayout();

	pathMap_.clear();
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

	PropertyGroupWidget* propGroupWidget = new PropertyGroupWidget(true);
	connect(propGroupWidget, SIGNAL(propertyEdited(QSharedPointer<Property>,QVariant)),
		 this, SLOT(propertyEdited(QSharedPointer<Property>,QVariant)));

	foreach(QSharedPointer<DataStore> runtimeDesc,runtimeDescendants)
	{
		QString parentPath = runtimeDesc->getPath();
		QHash<QString, QVector<QSharedPointer<Property>>> properties;
		QVector<QSharedPointer<Property>> runTimeProps;
		properties = runtimeDesc->getPropertyContainer()->getProperties();
		QStringList orderedProps = runtimeDesc->getOrderedPropertyList();
		foreach(QString propTag,orderedProps)
		{
			QVector<QSharedPointer<Property>> propVec = properties.value(propTag);
			foreach(QSharedPointer<Property> prop,propVec)
			{
				if(prop->isRuntimeEnabled() && prop->isVisible())
				{
					runTimeProps.append(prop);
					//Add prop to the pathMap so that it can be updated with saved values
					pathMap_[parentPath+"::"+prop->getName()] = prop;
				}
			}
		}
		if(!runTimeProps.size())
			continue;

		propGroupWidget->addProperties(runtimeDesc->getName(),runTimeProps);
	}
	layout->addWidget(propGroupWidget);
	layout->setAlignment(Qt::AlignTop);
	layout->setSizeConstraint(QLayout::SetMinAndMaxSize);
	mainWidget_->setLayout(layout);
	setWidget(mainWidget_);
	int reqWidth = mainWidget_->sizeHint().width()+25;
	setFixedWidth((reqWidth<350)?reqWidth:350);
}

/*! \brief Loads a session at the input filename path and sets all of the final init property values from that
 *	session to their corresponding RuntimeEditable Properties that are displayed in this PropertyFrame.
 *	\details If any Properties end up having their initValues changed, the parameterMessageReady() signal is emitted.
 *	If any Properties can't be found in the Session file at the input path, a pop-up is displayed listing the Properties
 *	that could not be found.
 *	\note We created this a long time ago and it didn't see much use.  It is probably not working and outdated.  In particular
 *	it should be working with the Sessions "initproperties" table, but that didn't exist when this function was created.  It would
 *	be worthwhile to go through this and fix it.
 */
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
			QString currValue = it.value()->initValToUserString();
			QString newValue = unit.value_;
			it.value()->initValFromUserString(newValue);
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

/*! \brief Called whenever a Property is edited whose widget appears in this PropertyFrame.  Verifies that the new Property value is legal, 
 *	then emits a parameterMessageReady() signal if it is.
 */
void PropertyFrame::propertyEdited(QSharedPointer<Property> prop,QVariant val)
{
	setWidget(mainWidget_);
	QSharedPointer<Asset> propParent = prop->getParentAsset();
	QSharedPointer<Parameter> paramParent = propParent.dynamicCast<Parameter>();
	if(paramParent)
	{
		if(!paramParent->valuesAreValid())
		{
			paramParent->fixValues();
			return;
		}
	}
	
	//Send it to director
	emit parameterMessageReady(prop);
}