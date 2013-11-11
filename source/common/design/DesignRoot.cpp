#include <QMessageBox>
#include <QTextCursor>
#include "DesignRoot.h"
#include "../statemachine/UIData.h"
#include "../parameter/AssociateRootHost.h"
#include "../parameter/AssociateRoot.h"
#include "../storage/OldVersionAsset.h"
#include "../memleakdetect.h"
using namespace Picto;

DesignRoot::DesignRoot()
{
	connect(&designRootText_,SIGNAL(undoAvailable(bool)),this,SIGNAL(undoAvailable(bool)));
	connect(&designRootText_,SIGNAL(redoAvailable(bool)),this,SIGNAL(redoAvailable(bool)));
}
bool DesignRoot::resetDesignRoot(QString DesignRootText)
{
	//Deserialize Design from DesignRootText
	QString errors = "";
	QSharedPointer<QXmlStreamReader> xmlReader(new QXmlStreamReader(DesignRootText));
	pictoData_ = QSharedPointer<PictoData>(PictoData::Create().staticCast<PictoData>());
	while(!xmlReader->isStartElement() && (xmlReader->name() != pictoData_->identifier()) && !xmlReader->atEnd()) 
		xmlReader->readNext();
	if(xmlReader->atEnd())
	{
		lastError_.details = "Could not find " + pictoData_->identifier() + " in DesignRoot Text.";
		lastError_.name = "XML Parsing Error                                      ";
		return false;
	}

	compiled_ = false;
	bool res = pictoData_->fromXml(xmlReader);
	if(!res)
	{
		errors = "DesignRoot Text Error\n"+Serializable::getErrors();
	}
	if(!errors.isEmpty())
	{
		lastError_.details = errors;
		lastError_.name = "XML Parsing Error                                      ";
		pictoData_.clear();
		return false;
	}
	connect(pictoData_.data(),SIGNAL(edited()),this,SLOT(designEdited()));
	setDesignName(pictoData_->getName());
	if(OldVersionAsset::encounteredOldVersionAsset() || Property::encounteredObsoleteSerialSyntax())
	{
		lastWarning_.name="Obsolete Syntax Encountered                                  ";
		lastWarning_.details="File contents have been automatically upgraded to "
			"function with the latest version of Picto.  If you save this design, "
			"it will be incompatible with older versions of Picto.";
		OldVersionAsset::clearOldVersionAssetFlag();
		Property::clearObsoleteSerialSyntax();
		//Move to the upgraded version
		return resetDesignRoot(pictoData_->toXml());
	}
	
	setUndoPoint();	//Has the effect of serializing the root object tree into designRootText_
	designRootText_.clearUndoRedoStacks();
	designRootText_.setModified(false);

	////Create Design Map
	//QStringList pictoDataChildTags = pictoData_->getDefinedChildTags();
	//foreach(QString childTag,pictoDataChildTags)
	//{
	//	QList<QSharedPointer<Asset>> children = pictoData_->getGeneratedChildren(childTag);
	//	foreach(QSharedPointer<Asset> child,children)
	//	{
	//		if(!child->inherits("Picto::UIEnabled"))
	//			continue;
	//		QSharedPointer<Design> design(new Design());
	//		design->resetRoot(child.staticCast<UIEnabled>());
	//		designMap_[childTag].push_back(design);
	//	}
	//}
	emit refreshedFromXml();
	return true;
}
//QStringList DesignRoot::getDesignIdentifiers()
//{
//	return designMap_.keys();
//}
//int DesignRoot::getDesignCount(QString identifier)
//{
//	if(!designMap_.contains(identifier))
//		return 0;
//	return designMap_.value(identifier).size();
//}
//QSharedPointer<Design> DesignRoot::getDesign(QString identifier,int index)
//{
//	if(!designMap_.contains(identifier))
//		return QSharedPointer<Design>();
//	QVector<QSharedPointer<Design>> designList = designMap_.value(identifier);
//	if(!(designList.size() > index))
//		return QSharedPointer<Design>();
//	return designList[index];
//}

QSharedPointer<Asset> DesignRoot::importAnalysis(QString analysisText)
{
	if(!pictoData_)
		return QSharedPointer<Asset>();
	QSharedPointer<Asset> newAsset = pictoData_->importChildAsset(analysisText,QString());
	return newAsset;
}

bool DesignRoot::removeAnalysis(int index)
{
	QSharedPointer<Asset> analysis = getAnalysis(index);
	if(!analysis)
		return false;
	analysis->setDeleted();
	return true;
}

QSharedPointer<Asset> DesignRoot::getExperiment()
{
	if(!pictoData_)
		return QSharedPointer<Asset>();
	return pictoData_->getExperiment();
}

int DesignRoot::getNumAnalyses()
{
	if(!pictoData_)
		return 0;
	return pictoData_->getGeneratedChildren("Analysis").size();
}

QSharedPointer<Asset> DesignRoot::getAnalysis(int index)
{
	if(index >= getNumAnalyses() || index < 0)
		return QSharedPointer<Asset>();
	return pictoData_->getGeneratedChildren("Analysis")[index];
}

void DesignRoot::setUndoPoint()
{
	if(!pictoData_)
		return;
	QString newDesignRootText = pictoData_->toXml();
	if(newDesignRootText != designRootText_.toPlainText())
	{
		try	//Sometimes with giant experiment designs this fails to allocate memory
		{
			QTextCursor cursor = QTextCursor(&designRootText_);
			cursor.beginEditBlock();
			cursor.select(QTextCursor::Document);
			cursor.insertText(newDesignRootText);
			cursor.endEditBlock();
		}
		catch(...)
		{
			designRootText_.setPlainText(newDesignRootText);
		}
	}
}

QSharedPointer<Asset> DesignRoot::getOpenAsset()
{
	QSharedPointer<Asset> experiment = getExperiment();
	if(!experiment)
		return QSharedPointer<Asset>();
	AssociateRootHost* expRootHost = dynamic_cast<AssociateRootHost*>(pictoData_->getExperiment().data());
	QSharedPointer<UIData> uiData = expRootHost->getAssociateRoot("UIData").staticCast<UIData>();
	return experiment->getDesignConfig()->getAsset(uiData->getOpenAsset());
}

void DesignRoot::setOpenAsset(QSharedPointer<Asset> asset)
{
	AssociateRootHost* expRootHost = dynamic_cast<AssociateRootHost*>(pictoData_->getExperiment().data());
	QSharedPointer<UIData> uiData = expRootHost->getAssociateRoot("UIData").staticCast<UIData>();
	uiData->setOpenAsset(asset->getAssetId());
}

bool DesignRoot::hasUndo()
{
	return designRootText_.isUndoAvailable();
}
bool DesignRoot::hasRedo()
{
	return designRootText_.isRedoAvailable();
}
void DesignRoot::undo()
{
	if(!hasUndo())
		return;
	designRootText_.undo();
	refreshFromXml();
}
void DesignRoot::redo()
{
	if(!hasRedo())
		return;
	designRootText_.redo();
	refreshFromXml();
}

void DesignRoot::refreshFromXml()
{
	QString text = designRootText_.toPlainText();
	//Deserialize Design from DesignRootText
	QString errors = "";
	QSharedPointer<QXmlStreamReader> xmlReader(new QXmlStreamReader(text));
	pictoData_ = QSharedPointer<PictoData>(PictoData::Create().staticCast<PictoData>());
	while(!xmlReader->isStartElement() && (xmlReader->name() != pictoData_->identifier()) && !xmlReader->atEnd()) 
		xmlReader->readNext();
	if(xmlReader->atEnd())
	{
		Q_ASSERT_X(false,"DesignRoot::refreshFromXml","Could not find " + pictoData_->identifier().toLatin1() + " in DesignRoot Text.");
	}
	bool res = pictoData_->fromXml(xmlReader);
	if(!res)
	{
		pictoData_.clear();
		Q_ASSERT_X(false,"DesignRoot::refreshFromXml","DesignRoot Text Error\n"+Serializable::getErrors().toLatin1());
	}
	connect(pictoData_.data(),SIGNAL(edited()),this,SLOT(designEdited()));
	setDesignName(pictoData_->getName());
	emit refreshedFromXml();
}

bool DesignRoot::isModified()
{
	return designRootText_.isModified();
}
void DesignRoot::setUnmodified()
{
	designRootText_.setModified(false);
}
QString DesignRoot::getDesignRootText()
{
	setUndoPoint();
	return designRootText_.toPlainText();
}
bool DesignRoot::compiles(QString* errors)
{
	(*errors) = "";
	if(compiled_)
		return true;
	Serializable::clearErrors();
	if(pictoData_ && pictoData_->validateTree())
	{
		compiled_ = true;
		return true;
	}
	(*errors) = Serializable::getErrors();
	return false;
}

void DesignRoot::enableRunMode(bool runMode)
{
	if(!pictoData_)
		return;
	pictoData_->enableRunModeForDescendants(runMode);
}
void DesignRoot::designEdited()
{
	compiled_ = false;
}