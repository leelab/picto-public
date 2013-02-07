#include <QtWidgets>
#include "PropertyBrowser.h"
#include "../../common/storage/datastore.h"
#include "../../common/memleakdetect.h"
using namespace Picto;

//! [0]
PropertyBrowser::PropertyBrowser(QSharedPointer<EditorState> editorState,QWidget *parent) :
	QWidget(parent),
	editorState_(editorState)
{
	connect(editorState_.data(), SIGNAL(selectedAssetChanged(QSharedPointer<Asset>)),
        this, SLOT(assetSelected(QSharedPointer<Asset>)));

	propGroupWidget_ = new PropertyGroupWidget();
	QVBoxLayout* myLayout = new QVBoxLayout();
	myLayout->addWidget(propGroupWidget_,Qt::AlignTop | Qt::AlignLeft);
	setLayout(myLayout);

	connect(propGroupWidget_, SIGNAL(propertyEdited(QSharedPointer<Property>,QVariant)),
        this, SLOT(propertyEdited(QSharedPointer<Property>,QVariant)));
}

void PropertyBrowser::assetSelected(QSharedPointer<Asset> asset)
{
	if(asset.isNull())
		return;
					
	QSharedPointer<DataStore> dataStore(asset.dynamicCast<DataStore>());
	if(dataStore.isNull())
		return;

	QHash<QString, QVector<QSharedPointer<Property>>> properties = dataStore->getPropertyContainer()->getProperties();
	QStringList orderedProps = dataStore->getOrderedPropertyList();
	QVector<QSharedPointer<Property>> propVec;
	foreach(QString propTag,orderedProps)
	{
		propVec << properties.value(propTag);
	}
	propGroupWidget_->clear();
	propGroupWidget_->addProperties(dataStore->getName(),propVec);
	setSizePolicy(QSizePolicy::Maximum,QSizePolicy::Maximum);
}

void PropertyBrowser::propertyEdited(QSharedPointer<Property> prop,QVariant val)
{
	editorState_->setLastActionUndoable();

	//Set the property value to its corresponding InitProperty if its scriptable
	QSharedPointer<Asset> currAsset = editorState_->getSelectedAsset();
	Q_ASSERT(currAsset);
	if(!currAsset->inherits("Picto::Scriptable"))
		return;
	QSharedPointer<Scriptable> currScriptable = currAsset.staticCast<Scriptable>();
	QSharedPointer<PropertyContainer> currInitPropContainer = currScriptable->getInitPropertyContainer();
	QSharedPointer<Property> currInitProp = currInitPropContainer->getProperty(prop->identifier());
	Q_ASSERT_X(currInitProp
		,"PropertyBrowser::propertyEdited"
		,"There is no InitProperty corresponding to the edited " + prop->identifier().toLatin1() + " property");
	currInitProp->setValue(prop->value());
}