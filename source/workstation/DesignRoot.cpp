#include <QMessageBox>
#include <QTextCursor>
#include "DesignRoot.h"
#include "../common/storage/ObsoleteAsset.h"
#include "../common/memleakdetect.h"

DesignRoot::DesignRoot()
{
}
bool DesignRoot::resetDesignRoot(QString DesignRootText)
{
	designMap_.clear();
	//Deserialize Design from DesignRootText
	QString errors = "";
	QSharedPointer<QXmlStreamReader> xmlReader(new QXmlStreamReader(DesignRootText));
	QSharedPointer<PictoData> pictoData = QSharedPointer<PictoData>(PictoData::Create().staticCast<PictoData>());
	while(!xmlReader->isStartElement() && (xmlReader->name() != pictoData->assetType()) && !xmlReader->atEnd()) 
		xmlReader->readNext();
	if(xmlReader->atEnd())
	{
		errors = "Could not find " + pictoData->assetType() + " in DesignRoot Text.";
		QMessageBox box;
		box.setText("XML Parsing Error                                      ");
		box.setDetailedText(errors);
		box.setIconPixmap(QPixmap(":/icons/x.png"));
		box.exec();
		return false;
	}

	bool res = pictoData->fromXml(xmlReader,false);
	if(!res)
	{
		errors = "DesignRoot Text Error\n"+Serializable::getErrors();
	}
	if(!errors.isEmpty())
	{
		QMessageBox box;
		box.setText("XML Parsing Error                                      ");
		box.setDetailedText(errors);
		box.setIconPixmap(QPixmap(":/icons/x.png"));
		box.exec();
		return false;
	}
	if(ObsoleteAsset::encounteredObsoleteAsset())
	{
		QMessageBox box;
		box.setText("Obsolete Assets Removed                                     ");
		box.setDetailedText("File contents have been automatically upgraded to "
			"function with the latest version of Picto.  If you save this design, "
			"it will be incompatible with older versions of Picto.");
		box.setIconPixmap(QPixmap(":/icons/triangle.png"));
		box.exec();
		ObsoleteAsset::clearObsoleteAssetFlag();
		//Move to the upgraded version
		return resetDesignRoot(pictoData->toXml());
	}
	
	//Create Design Map
	QStringList pictoDataChildTags = pictoData->getDefinedChildTags();
	foreach(QString childTag,pictoDataChildTags)
	{
		QList<QSharedPointer<Asset>> children = pictoData->getGeneratedChildren(childTag);
		foreach(QSharedPointer<Asset> child,children)
		{
			if(!child->inherits("Picto::UIEnabled"))
				continue;
			QSharedPointer<Design> design(new Design());
			design->resetRoot(child.staticCast<UIEnabled>());
			designMap_[childTag].push_back(design);
		}
	}
	return true;
}
QStringList DesignRoot::getDesignIdentifiers()
{
	return designMap_.keys();
}
int DesignRoot::getDesignCount(QString identifier)
{
	if(!designMap_.contains(identifier))
		return 0;
	return designMap_.value(identifier).size();
}
QSharedPointer<Design> DesignRoot::getDesign(QString identifier,int index)
{
	if(!designMap_.contains(identifier))
		return QSharedPointer<Design>();
	QVector<QSharedPointer<Design>> designList = designMap_.value(identifier);
	if(!(designList.size() > index))
		return QSharedPointer<Design>();
	return designList[index];
}
void DesignRoot::refreshFromXml()
{
	foreach(QVector<QSharedPointer<Design>> designList,designMap_)
	{
		foreach(QSharedPointer<Design> design,designList)
		{
			design->refreshFromXml();
		}
	}
}
bool DesignRoot::isModified()
{
	foreach(QVector<QSharedPointer<Design>> designList,designMap_)
	{
		foreach(QSharedPointer<Design> design,designList)
		{
			if(design->isModified())
				return true;
		}
	}
	return false;
}
void DesignRoot::setUnmodified()
{
	foreach(QVector<QSharedPointer<Design>> designList,designMap_)
	{
		foreach(QSharedPointer<Design> design,designList)
		{
			design->setUnmodified();
		}
	}
}
QString DesignRoot::getDesignRootText()
{
	QString designRootText = "<PictoData>";
	foreach(QVector<QSharedPointer<Design>> designList,designMap_)
	{
		foreach(QSharedPointer<Design> design,designList)
		{
			designRootText.append(design->getDesignText());
		}
	}
	designRootText.append("</PictoData>");
	return designRootText;
}
bool DesignRoot::compiles()
{
	foreach(QVector<QSharedPointer<Design>> designList,designMap_)
	{
		foreach(QSharedPointer<Design> design,designList)
		{
			if(!design->compiles())
				return false;
		}
	}
	return true;
}