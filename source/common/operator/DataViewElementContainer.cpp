#include <QMap>
#include <QPair>
#include <QDebug>
#include <QScriptValue>
#include <QScriptValueIterator>
#include <QAction>
#include <QTextEdit>
#include <QCoreApplication>

#include "DataViewElementContainer.h"
#include "../parameter/AssociateElement.h"
#include "../operator/OperatorPlot.h"

#include "../memleakdetect.h"

using namespace Picto;

/*! \brief Creates a new DataViewElementContainer and adds AssetFactory objects to it for creating all of the various
 *	types of DataViewElements.
 */
DataViewElementContainer::DataViewElementContainer()
	: dataViewElementFactory_(new AssetFactory(0, -1))
{
	AddDefinableObjectFactory("DataViewElement", dataViewElementFactory_);
	dataViewElementFactory_->addAssetType(OperatorPlot::type,
		QSharedPointer<AssetFactory>(new AssetFactory(0, -1, AssetFactory::NewAssetFnPtr(OperatorPlot::Create))));
}

void DataViewElementContainer::postDeserialize()
{
	StateMachineElement::postDeserialize();
	QStringList childTags = getDefinedChildTags();
	foreach(QString childTag,childTags)
	{
		QList<QSharedPointer<Asset>> tagChildren = getGeneratedChildren(childTag);
		foreach(QSharedPointer<Asset> tagChild,tagChildren)
		{
			addChildToElementLists(tagChild);
		}
	}
	connect(this,SIGNAL(childAddedAfterDeserialize(QSharedPointer<Asset>)),this,SLOT(addChildToElementLists(QSharedPointer<Asset>)));
}

bool DataViewElementContainer::validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	if(!StateMachineElement::validateObject(xmlStreamReader))
		return false;
	return true;
}
