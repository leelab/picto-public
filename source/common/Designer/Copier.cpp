#include <QMessageBox>
#include <QApplication>
#include <QClipboard>
#include <QWeakPointer>
#include "Copier.h"
#include "../parameter/AssociateElement.h"
#include "../storage/DesignConfig.h"
#include "../../common/memleakdetect.h"
Copier::Copier(QSharedPointer<EditorState> editorState)	:
editorState_(editorState)
{

}

Copier::~Copier()
{
}

void Copier::copy(QList<QSharedPointer<Asset>> assets,bool copyAnalysis)
{
	if(!assets.size())
		return;

	//Check to see if any of the selected items contain the desired elements.  If not, tell the user none were selected.
	SearchRequest searchRequest(SearchRequest::EXPERIMENT,SearchRequest::EXISTS);
	if(copyAnalysis)
		searchRequest = SearchRequest(SearchRequest::ACTIVE_ANALYSES,SearchRequest::EXISTS);

	bool hasDesiredElement = false;
	foreach(QSharedPointer<Asset> asset, assets)
	{
		QSharedPointer<DataStore> dataStore = asset.dynamicCast<DataStore>();
		if(dataStore)
		{
			if(dataStore->searchRecursivelyForQuery(searchRequest))
			{
				hasDesiredElement = true;
				break;
			}
		}
    }
	if(!hasDesiredElement)
	{
		if(copyAnalysis)
		{
			QMessageBox::warning(NULL,"No analysis elements found","The selected elements did not contain any analysis elements.");
			return;
		}
		QMessageBox::warning(NULL,"No experimental elements found","The selected elements did not contain any experimental elements.");
		return;
	}

	//Get the AssociateRootHost (Experiment or Analysis)
	AssociateRootHost* assocRootHost;
	if(copyAnalysis)
		assocRootHost = dynamic_cast<AssociateRootHost*>(editorState_->getCurrentAnalysis().data());
	else
		assocRootHost = dynamic_cast<AssociateRootHost*>(editorState_->getTopLevelAsset().data());
	if(!assocRootHost)
		return;

	//Create a list of assets that need to be copied. (For experimental, this is just the input list.  For analysis,
	//it is a list of analysis assets attached to the asset trees in the input list).
	QList<QSharedPointer<Asset>> assetsToCopy;
	if(copyAnalysis)
	{
		//Get the analysis Id.
		QUuid analysisId;
		analysisId = dynamic_cast<AssociateRoot*>(assocRootHost)->getAssociateId();

		//For each input asset, get all the Analses attached to the tree underneath it and add them to the
		//assetsToCopy list.
		foreach (QSharedPointer<Asset> asset, assets) 
		{
			if(asset->inherits("Picto::DataStore"))
			{
				assetsToCopy.append(asset.staticCast<DataStore>()->getAssociateDescendants(analysisId));
			}
		}
	}
	else
	{
		//If we're copying experimental assets, just put them in the assetsToCopy list.
		assetsToCopy = assets;
	}
	
	//Get the associateId of the User Interface elements attached to the current AssociateRootHost (Experiment or Analysis).
	QSharedPointer<AssociateRoot> assocRoot = assocRootHost->getAssociateRoot();
	if(!copyAnalysis && !assocRoot)
		return;

	//Loop through all assets to be copied, add each one's xml to the element text and add its UI XML to the uiText.
	QString elementText;
	QString uiText;
	QList<QSharedPointer<Asset>> uiDescendants;
	QString elementTag = copyAnalysis?"AnalysisCopy":"ElementCopy";
	QString uiTag = "UICopy";
	foreach (QSharedPointer<Asset> asset, assetsToCopy) 
	{
		//Add the element xml to elementText
		elementText.append("<"+elementTag+">"+asset->toXml()+"</"+elementTag+">");
		//If UIData is attached, add the UI elements to the UiText.
		if(assocRoot)
		{
			QUuid uiDataId = assocRoot->getAssociateId();
			if(asset->inherits("Picto::DataStore"))
			{
				//Get the UIDescendants
				uiDescendants = asset.staticCast<DataStore>()->getAssociateDescendants(uiDataId);
				//Go through UIDescednants adding their xml to the UIText
				foreach(QSharedPointer<Asset> associate,uiDescendants)
				{
					uiText.append("<"+uiTag+">"+associate->toXml()+"</"+uiTag+">");
				}
			}
		}
	}

	//Add identifying tags to the xml strings and put them in the clipboard.
	QClipboard *clipboard = QApplication::clipboard();
	QString copyText =	"<CopyData>"
						"<CopiedFrom>"
						+assets.first()->getParentAsset()->getPath()
						+"</CopiedFrom>"
						+elementText
						+uiText
						+"</CopyData>";
	clipboard->setText(	copyText );
}

void Copier::paste(QSharedPointer<Asset> pasteParent, QPoint pastePosition)
{
	if(!pasteParent)
		return;

	QClipboard *clipboard = QApplication::clipboard();
	QString copiedText = clipboard->text();
	//Separate copied text into its separate components.  There should be either an
	//ExperimentElements or an AnalysisElements section and up to one UIElements section
	//as well as a CopiedFrom path string.
	QSharedPointer<QXmlStreamReader> xmlStreamReader(new QXmlStreamReader(copiedText));
	bool error = false;
	QString copiedFrom;
	QStringList elems;
	QStringList uiElems;
	bool hadAnalysis = false;
	bool hadExperiment = false;
	bool hadCopiedFrom = false;
	bool hadUI = false;
	while(!xmlStreamReader->isEndDocument() && !(xmlStreamReader->isEndElement() && xmlStreamReader->name().toString() == "CopyData"))
	{
		QString errorStr = xmlStreamReader->errorString();
		if(xmlStreamReader->readNext() == QXmlStreamReader::Invalid)
		{
			error = true;
			break;
		}
		if(!xmlStreamReader->isStartElement())
			continue;
		QString tagName = xmlStreamReader->name().toString();
		if(tagName == "CopyData")
			continue;
		else if(tagName == "CopiedFrom")
		{
			hadCopiedFrom = true;
			copiedFrom = xmlStreamReader->text().toString();
		}
		else if(tagName == "AnalysisCopy")
		{
			hadAnalysis = true;
			if(hadExperiment)
			{
				error = true;
				break;
			}
			QString output;
			if(!getContentsUntilEndTag(xmlStreamReader,tagName,output))
			{
				error = true;
				break;
			}
			elems.append(output);
		}
		else if(tagName == "ElementCopy")
		{
			hadExperiment = true;
			if(hadAnalysis)
			{
				error = true;
				break;
			}
			QString output;
			if(!getContentsUntilEndTag(xmlStreamReader,tagName,output))
			{
				error = true;
				break;
			}
			elems.append(output);
		}
		else if(tagName == "UICopy")
		{
			hadUI = true;
			QString output;
			if(!getContentsUntilEndTag(xmlStreamReader,tagName,output))
			{
				error = true;
				break;
			}
			uiElems.append(output);
		}
		else
		{
			error = true;
			break;
		}
	
	}
	if(error)
	{
		QMessageBox::critical(NULL,"Paste Error","Could not paste.  Your design has not been changed.");
		return;
	}

	//Verify that necessary data was included in the copied string
	if(!(hadAnalysis || hadUI) || !hadCopiedFrom || !(hadAnalysis || hadExperiment) || !elems.size())
	{
		QMessageBox::warning(NULL,"Insufficient copied data","Could not paste due to insufficient copy data.");
		return;
	}

	if(hadAnalysis && hadExperiment)
	{
		QMessageBox::warning(NULL,"Could not copy data","Analysis and Experiment elements must be copied seperately.");
		return;
	}

	//Create a temporary design config and set it to the current window asset.
	//We use a temporary design config so that newly imported assets will not have their asset ids
	//change and UIElements can reattach to these assets based on their
	//asset ids instead of using path names (which could lead to problems if there was another
	//asset with the same path).
	QSharedPointer<DesignConfig> origDesignConfig = editorState_->getTopLevelAsset()->getDesignConfig();
	QSharedPointer<DesignConfig> tempDesignConfig(new DesignConfig());

	///////////////////////////////
	//Import Experimental Elements
	QList<QSharedPointer<Asset>> newExperimentElems;
	QList<QSharedPointer<Asset>> newAnalysisElems;
	QString errorOutput;
	if(hadExperiment)
	{
		pasteParent->setDesignConfig(tempDesignConfig);
		foreach(QString elem,elems)
		{
			newExperimentElems.append(pasteParent.staticCast<DataStore>()->importChildAsset(elem,errorOutput));
			if(errorOutput.size())
			{
				QMessageBox::critical(NULL,"Copy Error","Copy failed.  " + errorOutput.toLatin1());
				editorState_->requestUndo();
				return;
			}
		}
	}
	else if(hadAnalysis)
	{
		//Get the active analysis
		QSharedPointer<Asset> activeAnalysis = editorState_->getCurrentAnalysis();
		Q_ASSERT(activeAnalysis->inherits("Picto::DataStore"));
		if(!activeAnalysis)
		{
			editorState_->requestUndo();
			return;
		}
		activeAnalysis->setDesignConfig(tempDesignConfig);
		foreach(QString elem,elems)
		{
			newAnalysisElems.append(activeAnalysis.staticCast<DataStore>()->importChildAsset(elem,errorOutput));
			if(errorOutput.size())
			{
				QMessageBox::critical(NULL,"Copy Error","Copy failed.  " + errorOutput.toLatin1());
				editorState_->requestUndo();
				return;
			}
		}
	}

	////////////////////
	//Import UI elements
	//Get UIRoot
	AssociateRootHost* assocRootHost;
	if(hadExperiment)
		assocRootHost = dynamic_cast<AssociateRootHost*>(editorState_->getTopLevelAsset().data());
	else
		assocRootHost = dynamic_cast<AssociateRootHost*>(editorState_->getCurrentAnalysis().data());
	Q_ASSERT(assocRootHost);
	QSharedPointer<AssociateRoot> uiRoot = assocRootHost->getAssociateRoot();
	Q_ASSERT(uiRoot);
	DataStore* uiRootDataStore = dynamic_cast<DataStore*>(uiRoot.data());
	Q_ASSERT(uiRootDataStore);
	uiRootDataStore->setDesignConfig(tempDesignConfig);
	QList<QSharedPointer<Asset>> newUIElems;
	foreach(QString uiElem,uiElems)
	{
		newUIElems.append(uiRootDataStore->importChildAsset(uiElem,errorOutput));
		if(errorOutput.size())
		{
			QMessageBox::critical(NULL,"Copy Error","Copy failed.  " + errorOutput.toLatin1());
			editorState_->requestUndo();
			return;
		}
	}

	//Now that everything has been added.  Initialize asset id lookup in the Temporary Design Config.
	tempDesignConfig->disallowIdDuplication();
	tempDesignConfig->fixDuplicatedAssetIds();

	//Attach new UIElems to their link elements.
	AssociateElement* assocElem;
	QSharedPointer<Asset> linkAsset;
	foreach(QSharedPointer<Asset> uiElem,newUIElems)
	{
		assocElem = dynamic_cast<AssociateElement*>(uiElem.data());
		Q_ASSERT(assocElem);
		linkAsset = tempDesignConfig->getAsset(assocElem->getParentId());
		if(linkAsset.isNull() || !linkAsset->inherits("Picto::DataStore"))
		{
			QMessageBox::critical(NULL,"Copy Error","The copy data was invalid.  Please try again.");
			editorState_->requestUndo();
			return;
		}
		linkAsset.staticCast<DataStore>()->AddAssociateChild(assocElem->getAssociateId(),uiElem->identifier(),uiElem);
	}

	//Set DesignConfig back to original in all elements including those parent assets that we had to change.
	foreach(QWeakPointer<Asset> weakAsset,tempDesignConfig->getAssets())
	{
		Q_ASSERT(!weakAsset.isNull());
		weakAsset.toStrongRef()->setDesignConfig(origDesignConfig);
	}
	tempDesignConfig.clear();
	origDesignConfig->fixDuplicatedAssetIds();

	//Update Analysis Elements paths
	QString pasteParentPath = pasteParent->getPath();
	//Remove task from pasteParentPath
	QStringList pasteParentPathComponents = pasteParentPath.split("::",QString::KeepEmptyParts);
	pasteParentPathComponents.pop_front();
	pasteParentPath = pasteParentPathComponents.join("::");
	foreach(QSharedPointer<Asset> asset,newAnalysisElems)
	{
		assocElem = dynamic_cast<AssociateElement*>(asset.data());
		Q_ASSERT(assocElem);
		assocElem->updateLinkPath(copiedFrom,pasteParentPath);
	}

	//Attempt to link new analysis elements to the new design.
	QList<QSharedPointer<Asset>> failedLinks;
	foreach(QSharedPointer<Asset> asset,newAnalysisElems)
	{
		assocElem = dynamic_cast<AssociateElement*>(asset.data());
		Q_ASSERT(assocElem);
		if(assocElem->attachToLinkedAsset(false) == AssociateElement::FAIL)
		{
			failedLinks.append(asset);
			asset->setDeleted();
		}
	}

	//Go through pasted elements and reposition them according to the input pastePosition
	//Create a list of elements in the top level that should be moved
	QList<QSharedPointer<UIEnabled>> elemsToMove;
	foreach(QSharedPointer<Asset> elem,newExperimentElems)
	{
		if(!elem->inherits("Picto::UIEnabled"))
			continue;
		elemsToMove.append(elem.staticCast<UIEnabled>());
		if(elemsToMove.last()->getPos().isNull())
			elemsToMove.pop_back();	//If the pos is empty, we should ignore it.
	}
	foreach(QSharedPointer<Asset> elem,newAnalysisElems)
	{
		if(!elem->inherits("Picto::UIEnabled"))
			continue;
		assocElem = dynamic_cast<AssociateElement*>(elem.data());
		//Only move the Analysis Elements that are linked to the paste parent level.
		//This was not a problem with Experimental Elements because our list only
		//included those top level assets.
		if(assocElem->getLinkedAsset() != pasteParent)
			continue;
		elemsToMove.append(elem.staticCast<UIEnabled>());
		if(elemsToMove.last()->getPos().isNull() || elemsToMove.last())
			elemsToMove.pop_back();	//If the pos is empty, we should ignore it.  Maybe its an AnalysisScriptable.
	}
	//Get the top left position of a bounding box surrounding the moving elements
	if(elemsToMove.size())
	{
		QPoint topLeftPoint(1000000,10000000);
		QPoint currElemPos;
		foreach(QSharedPointer<UIEnabled> elem,elemsToMove)
		{
			currElemPos = elem->getPos();
			if(currElemPos.x() < topLeftPoint.x())
				topLeftPoint.setX(currElemPos.x());
			if(currElemPos.y() < topLeftPoint.y())
				topLeftPoint.setY(currElemPos.y());
		}

		//Get the offset from the bouding box top left point to the paste position
		QPoint offset = pastePosition-topLeftPoint;
		//Offset all input elements' positions by the offset
		foreach(QSharedPointer<UIEnabled> elem,elemsToMove)
		{
			elem->setPos(elem->getPos()+offset);
		}
	}

	//Inform the user which links failed.
	if(failedLinks.size())
	{
		QString failedLinkString = "The following Analysis elements were skipped during the copy operation since appropriate parent elements could not be found for them:\n";
		foreach(QSharedPointer<Asset> asset,failedLinks)
		{
			failedLinkString.append(dynamic_cast<AssociateElement*>(asset.data())->getParentPath()+"\n");
		}
		QMessageBox::warning(NULL,"Skipped Analyses",failedLinkString);
	}
	editorState_->setLastActionUndoable();
	editorState_->triggerExperimentReset();
}

bool Copier::getContentsUntilEndTag(QSharedPointer<QXmlStreamReader> xmlReader, QString endTag, QString& output)
{
	output.clear();
	QSharedPointer<QXmlStreamWriter> xmlWriter;
	Q_ASSERT(xmlReader->name().toString() == endTag);

	xmlWriter = QSharedPointer<QXmlStreamWriter>(new QXmlStreamWriter(&output));
	if(xmlReader->readNext() == QXmlStreamReader::Invalid)
	{
		return false;
	}
	while(!(xmlReader->isEndElement() && (xmlReader->name().toString() == endTag)) && !xmlReader->isEndDocument())
	{
		xmlWriter->writeCurrentToken(*xmlReader);
		if(xmlReader->readNext() == QXmlStreamReader::Invalid)
		{
			return false;
		}
	}
	if(xmlReader->isEndDocument())
		return false;
	return true;
}