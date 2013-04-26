#include <QMessageBox>
#include <QTextCursor>
#include "Design.h"
#include "../statemachine/UIData.h"
#include "../parameter/AssociateRootHost.h"
//#include "../common/analysis/AnalysisContainer.h"
#include "../common/memleakdetect.h"
using namespace Picto;

Design::Design()
{
}
//bool Design::resetRoot(QSharedPointer<UIEnabled> root)
//{
//	root_ = root;
//	return true;
//}
//QSharedPointer<Asset> Design::getRootAsset()
//{
//	return root_.staticCast<Asset>();
//}
//QSharedPointer<Asset> Design::getOpenAsset()
//{
//	//Get UIData
//	AssociateRootHost* assocRootHost = dynamic_cast<AssociateRootHost*>(root_.data());
//	QSharedPointer<UIData> uiData = assocRootHost->getAssociateRoot().staticCast<UIData>();
//	//Set open asset id to UIData
//	return findAssetWithID(root_,uiData->getOpenAsset());
//}
//void Design::setOpenAsset(QSharedPointer<Asset> asset)
//{
//	//Get UIData
//	AssociateRootHost* assocRootHost = dynamic_cast<AssociateRootHost*>(root_.data());
//	QSharedPointer<UIData> uiData = assocRootHost->getAssociateRoot().staticCast<UIData>();
//	//Set open asset id to UIData
//	uiData->setOpenAsset(asset->getAssetId());
//}
//void Design::setUndoPoint()
//{
//	if(!root_)
//		return;
//
//	QString newDesignText = root_->toXml();
//	if(newDesignText != designText_.toPlainText())
//	{
//		QTextCursor cursor = QTextCursor(&designText_);
//		cursor.beginEditBlock();
//		cursor.select(QTextCursor::Document);
//		cursor.removeSelectedText();
//		cursor.insertText(newDesignText);
//		cursor.endEditBlock();
//	}
//}
//bool Design::hasUndo()
//{
//	return designText_.isUndoAvailable();
//}
//bool Design::hasRedo()
//{
//	return designText_.isRedoAvailable();
//}
//void Design::undo()
//{
//	if(!hasUndo())
//		return;
//	designText_.undo();
//	refreshFromXml();
//}
//void Design::redo()
//{
//	if(!hasRedo())
//		return;
//	designText_.redo();
//	refreshFromXml();
//}
//void Design::refreshFromXml()
//{
//	QString res = updateAssetsFromText(designText_.toPlainText());
//	Q_ASSERT(res.isEmpty());
//
//}
//bool Design::isModified()
//{
//	return designText_.isModified();
//}
//void Design::setUnmodified()
//{
//	designText_.setModified(false);
//}
//QString Design::getDesignText()
//{
//	setUndoPoint();
//	return designText_.toPlainText();
//}

//bool Design::compiles(QString* errors)
//{
//	(*errors) = "";
//	if(compiled_)
//		return true;
//	Serializable::clearErrors();
//	if(root_ && root_->validateTree())
//	{
//		compiled_ = true;
//		return true;
//	}
//	(*errors) = Serializable::getErrors();
//	return false;
//}

QString Design::updateAssetsFromText(QString designText)
{
	Q_ASSERT(root_);
	QSharedPointer<QXmlStreamReader> xmlReader(new QXmlStreamReader(designText));
	QSharedPointer<DataStore> newRoot = createRoot(root_->assetType());
	while(!xmlReader->isStartElement() && (xmlReader->name() != newRoot->assetType())) 
		xmlReader->readNext();
	if(xmlReader->atEnd())
	{
		return "Could not find " + newRoot->assetType() + " in Design Text.";
	}

	bool res = newRoot->fromXml(xmlReader,false);
	if(!res)
	{
		return "Design Text Error\n"+Serializable::getErrors();
	}
	root_ = newRoot.staticCast<UIEnabled>();
	//setOpenAsset(findAssetWithID(root_,root_->getOpenDescendant()));
	return "";
}
QSharedPointer<Asset> Design::findAssetWithID(QSharedPointer<Asset> root, int id)
{
	Q_ASSERT(root);
	QSharedPointer<Asset> result;
	QSharedPointer<UIEnabled> uienabled = root.dynamicCast<UIEnabled>();
	if	(	!id || 
			(!uienabled.isNull() && (uienabled->getAssetId() == id))
		)
	{	//This is the asset with that id, return it.
		return root;
	}
	QSharedPointer<DataStore> rootDS = root.dynamicCast<DataStore>();
	if(!rootDS)
		return QSharedPointer<Asset>();
	//Recursively call the function on all of this assets children
	QStringList childTypes = rootDS->getDefinedChildTags();
	foreach(QString childType, childTypes)
	{
		QList<QSharedPointer<Asset>> assets = rootDS->getGeneratedChildren(childType);
		foreach(QSharedPointer<Asset> childAsset,assets)
		{
			result = findAssetWithID(childAsset,id);
			if(!result.isNull())
				return result;
		}
	}
	//Either the asset didn't have children or none had this id, return null pointer.
	return QSharedPointer<Asset>();
}
QSharedPointer<DataStore> Design::createRoot(QString identifier)
{
	if(identifier == "Experiment")
		return Experiment::Create().staticCast<DataStore>();
	//if(identifier == "AnalysisContainer")
	//	return AnalysisContainer::Create().staticCast<DataStore>();
	Q_ASSERT_X(false,"Design::createRoot","A Design type was created with an unsupported root node.");
	return QSharedPointer<DataStore>();
}
//void Design::designEdited()
//{
//	compiled_ = false;
//}