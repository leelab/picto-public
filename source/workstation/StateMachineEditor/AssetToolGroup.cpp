#include <QtGui>
#include "AssetToolGroup.h"
#include "../../common/storage/DataStore.h"
using namespace Picto;


AssetToolGroup::AssetToolGroup(QSharedPointer<Asset> asset, QWidget *parent) :
	ToolGroup(parent)
{
	setAsset(asset);
}

void AssetToolGroup::setAsset(QSharedPointer<Asset> asset)
{
	clearButtons();
	asset_ = asset;
	QSharedPointer<DataStore> dataStore = asset.dynamicCast<DataStore>();
	if(!dataStore.isNull())
	{
		QStringList childTags = dataStore->getValidChildTags();
		foreach(QString childTag,childTags)
		{
			AddButton(childTag,QIcon());
		}
	}
	AddButton(tr("Text"),QIcon(QPixmap(":/icons/textpointer.png").scaled(30,30)));

}