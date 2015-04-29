#include <QMessageBox>
#include <QApplication>
#include <QWeakPointer>
#include "AssetExportImport.h"
#include "../parameter/AssociateElement.h"
#include "../storage/DesignConfig.h"
#include "../../common/memleakdetect.h"
AssetExportImport::AssetExportImport()
{
}

AssetExportImport::~AssetExportImport()
{
}

/*! \brief Serializes the input assets from the input associateRootHost object's tree to a text string and returns that
 *	string.
 *	\details The returned string is meant to be used with AssetExportImport::importFromText().  If the returned string is
 *	empty, there was a problem and more information is available from getLatestMessage().  The input associateRootHost is
 *	the Experiment object if the input objects are experimental assets and the Analysis object if the input objects are
 *	AnalysisElements.  Experimental and Analysis elements cannot be exported at the same time.  The serialized text
 *	includes UIData so that imported elements will appear with the same positions as the exported elements with the
 *	exception of top level elements which will have the same relative positions but will be imported at the point where
 *	the mouse clicks for the import.  Exported string looks something like this:
 *	\code
 	<CopyData>
 		<CopiedFrom>[DesignPath]</CopyFrom>
 		<ElementCopy>
 			[Serialized Element 1 XML as in Design File]
 		</ElementCopy>
 		<ElementCopy>
 			[Serialized Element 2 XML as in Design File]
 		</ElementCopy>
 			...
 		<UICopy>
 			[Serialized UIElement for Element1 XML as in Design File]
 		</UICopy>
 		<UICopy>
 			[Serialized UIElement for Element2 XML as in Design File]	
 		</UICopy>
 			...
 	</CopyData>
 	\endcode
 */
QString AssetExportImport::exportToText(QList<QSharedPointer<Asset>> assets,AssociateRootHost* associateRootHost)
{
	latestMessage_.clear();
	if(!assets.size())
	{
		latestMessage_ = "No elements were selected.";
		return "";
	}
	if(!associateRootHost)
	{
		Q_ASSERT(false);
		latestMessage_ = "Hmm... that didn't work...";
		return "";
	}

	bool copyAnalysis = (dynamic_cast<Analysis*>(associateRootHost) != NULL);
	bool exportAnalysis = false;
	//If we're copying analysis elements, either all assets in list must be analysis elements or there must be only one
	//asset in the list (the experimental element from which analyses should be exported).
	if(copyAnalysis)
	{
		int numAnalysisElems = 0;
		int numExpElems = 0;
		foreach(QSharedPointer<Asset> asset, assets)
		{
			//If this asset is not an analysis element
			if(!dynamic_cast<AssociateElement*>(asset.data()))
				numExpElems++;
			else
				numAnalysisElems++;
		}
		if((numExpElems == 0) && (numAnalysisElems == 0))
		{
			latestMessage_ = "Please select elements to copy.";
			return "";
		}
		else if(numExpElems > 0 && numAnalysisElems > 0)
		{
			latestMessage_ = "Please select either Analysis elements or Experiment elements, but not both.";
			return "";
		}
		else if(numExpElems > 1)
		{
			//If there is more than one experiment asset in the list, the copy is invalid
			latestMessage_ = "Analysis can only be exported from a single Experiment element at a time.";
			return "";
		}
		else if(numExpElems == 1)
		{
			exportAnalysis = true;
		}
	}
	else
	{	//If we're not copying analysis elements, make sure that none are included in the selection
		foreach(QSharedPointer<Asset> asset, assets)
		{
			//If this asset is an analysis element, there is a problem
			if(dynamic_cast<AssociateElement*>(asset.data()))
			{
				latestMessage_ = "Analysis and Experimental elements must be copied separately.";
				return "";
			}
		}
	}

	////Check to see if any of the selected items contain the desired elements.  If not, tell the user none were selected.
	//SearchRequest searchRequest(SearchRequest::EXPERIMENT,SearchRequest::EXISTS);
	//if(copyAnalysis)
	//	searchRequest = SearchRequest(SearchRequest::ACTIVE_ANALYSES,SearchRequest::EXISTS);

	//foreach(QSharedPointer<Asset> asset, assets)
	//{
	//	QSharedPointer<DataStore> dataStore = asset.dynamicCast<DataStore>();
	//	if(dataStore)
	//	{
	//		if(!dataStore->searchRecursivelyForQuery(searchRequest))
	//		{
	//			latestMessage_ = "Analysis and Experimental elements must be copied separately.";
	//			return "";
	//		}
	//	}
	//   }

	//Create a list of assets that need to be copied. (For experimental, and analysis elements this is just the input list.  For analysis from an experimental container,
	//it is a list of analysis assets attached to the asset trees in the single input).
	QList<QSharedPointer<Asset>> assetsToCopy;
	if(copyAnalysis && (assets.size() == 1) && (!dynamic_cast<AssociateElement*>(assets.first().data())))
	{
		//Get the analysis Id.
		QUuid analysisId;
		analysisId = dynamic_cast<AssociateRoot*>(associateRootHost)->getAssociateId();

		//Get all the Analyses attached to the tree underneath the experimental element and add them to the
		//assetsToCopy list.
		if(assets.first()->inherits("Picto::DataStore"))
		{
			assetsToCopy.append(assets.first().staticCast<DataStore>()->getAssociateDescendants(analysisId));
		}
		if(assetsToCopy.isEmpty())
		{
			latestMessage_ = "The selected element does not contain any analysis elements from the " + dynamic_cast<Analysis*>(associateRootHost)->getName() + " Analysis.";
			return "";
		}
	}
	else
	{
		//If we're copying experimental or individual analysis assets, just put them in the assetsToCopy list.
		assetsToCopy = assets;
	}
	
	//Get the associateId of the User Interface elements attached to the current AssociateRootHost (Experiment or Analysis).
	QSharedPointer<AssociateRoot> assocRoot = associateRootHost->getAssociateRoot("UIData");
	if(!copyAnalysis && !assocRoot)
	{
		Q_ASSERT(false);
		latestMessage_ = "Hmm... that didn't work...";
		return "";
	}

	//Loop through all assets to be copied, add each one's xml to the element text and add its UI XML to the uiText.
	QString elementText;
	QString uiText;
	QList<QSharedPointer<Asset>> uiDescendants;
	QString elementTag = copyAnalysis?(exportAnalysis?"AnalysisExport":"AnalysisCopy"):"ElementCopy";
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

	QString parentPath;
	AssociateElement* assocElem = dynamic_cast<AssociateElement*>(assets.first().data());
	if(assocElem)
	{
		parentPath = assocElem->getParentPath();
	}
	else
	{
		//For analysis export, the parent path is the selected asset,
		//For experimental copy, the parent path is the assets' parent
		if(copyAnalysis)
			parentPath = assets.first()->getPath();
		else
			parentPath = assets.first()->getParentAsset()->getPath();
	}

	//Add identifying tags to the xml strings and put them in the copy text.
	QString copyText =	"<CopyData>"
						"<CopiedFrom>"
						+parentPath
						+"</CopiedFrom>"
						+elementText
						+uiText
						+"</CopyData>";
	return copyText;
}

/*! \brief Imports text exported from exportToText() into the input pasteParent
 *	\details pasteParent is the Asset which will be the parent of the imported Assets.
 *	pasteText is the text that was exported from exportToText.  experimentRootHost is the AssoicateRootHost pointer to the
 *	Experiment object to which the imported experimental or analysis assets are associated pastePosition is the position
 *	at which the assets should be placed (ie. If the user right clicked and selected paste somewhere, that position would
 *	go here).  analysisRootHost is the AssoicateRootHost pointer to the Analysis object into which any analysis elements
 *	will be imported (it may be null if importing only experimental assets).  The returned value is an enum from
 *	AssetExportImport::IMPORT_RESULT_TYPE.
 *	
 *	The import algorithm is fairly involved considering the fact that imported assets may be experimental or analysis, and
 *	they have associated UIElement data which references its associated elements by asset id, yet asset ids can change if
 *	assets are immediately imported into an experiment where one of their asset ids is already taken.  The best way to
 *	understand what is happening is to just read the code, but in a nutshell we take care of most of these issues by
 *	serializing the imported text in with a temporary DesignConfig, then once the associate links are set up moving the
 *	assets to the correct DesignConfig and only updating their AssetIds at that point.
 *
 *	\note A common event is that the import will succeed with warnings, and the warnings will include a list of elements
 *	that could not be imported.  This occurs when an experiment has changed slightly and we are trying to import an
 *	analysis.  Frequently most of the analysis elements will find their attached experimental elements, but a few of them
 *	will fail since the experimental elements' names have changed.  In this case the users might undo, fix the names and
 *	reimport, or simply import the analysis elements whose associated experimental elements could not be found one by one
 *	at the appropriate level.
 */
int AssetExportImport::importFromText(QSharedPointer<Asset> pasteParent, QString pasteText, AssociateRootHost* experimentRootHost, QPoint pastePosition, AssociateRootHost* analysisRootHost)
{
	latestMessage_.clear();
	if(!pasteParent)
	{
		latestMessage_ = "No indication was provided as to the level where the elements should go.";
		return IMPORT_FAILED;
	}

	QString copiedText = pasteText;
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
	bool isAnalysisImport  = false;
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
			if(xmlStreamReader->readNext() == QXmlStreamReader::Invalid)
			{
				error = true;
				break;
			}
			copiedFrom = xmlStreamReader->text().toString();
		}
		else if(tagName == "AnalysisCopy" || tagName == "AnalysisExport")
		{
			if(tagName == "AnalysisExport")
				isAnalysisImport = true;
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
		latestMessage_ = "Invalid data.";
		return IMPORT_FAILED;
	}

	//Verify that necessary data was included in the copied string
	if(!(hadAnalysis || hadUI) || !hadCopiedFrom || !(hadAnalysis || hadExperiment) || !elems.size())
	{
		latestMessage_ = "Invalid data.";
		return IMPORT_FAILED;
	}

	if(hadAnalysis && hadExperiment)
	{
		latestMessage_ = "Invalid data.";
		return IMPORT_FAILED;
	}

	if(hadAnalysis && !analysisRootHost)
	{
		latestMessage_ = "No target analysis was provided.";
		return IMPORT_FAILED;
	}

	if(isAnalysisImport)
	{
		//If we're importing an analysis, the caller must clear all current analysis elements out first
		if(pasteParent.staticCast<DataStore>()->getAssociateDescendants(dynamic_cast<AssociateRoot*>(analysisRootHost)->getAssociateId()).size())
		{
			latestMessage_ = "In order to import Analysis elements into an Experiment element, all Analysis descendants of that Experiment element must first be removed.";
			return IMPORT_FAILED;
		}
	}

	//Create a temporary design config and set it to the current window asset.
	//We use a temporary design config so that newly imported assets will not have their asset ids change and UIElements
	//	can reattach to these assets based on their asset ids instead of using path names (which could lead to problems if
	//	there was another asset with the same path).
	QSharedPointer<DesignConfig> origDesignConfig = dynamic_cast<Asset*>(experimentRootHost)->getDesignConfig();
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
				latestMessage_ = errorOutput.toLatin1();
				return IMPORT_FAILED_ROLLBACK;
			}
		}
	}
	else if(hadAnalysis)
	{
		//Get the active analysis
		Asset* activeAnalysis = dynamic_cast<Asset*>(analysisRootHost);
		Q_ASSERT(activeAnalysis->inherits("Picto::DataStore"));
		activeAnalysis->setDesignConfig(tempDesignConfig);
		foreach(QString elem,elems)
		{
			newAnalysisElems.append(static_cast<DataStore*>(activeAnalysis)->importChildAsset(elem,errorOutput));
			if(errorOutput.size())
			{
				latestMessage_ = errorOutput.toLatin1();
				return IMPORT_FAILED_ROLLBACK;
			}
		}
	}

	////////////////////
	//Import UI elements
	//Get UIRoot
	AssociateRootHost* assocRootHost;
	if(hadExperiment)
		assocRootHost = experimentRootHost;
	else
		assocRootHost = analysisRootHost;
	Q_ASSERT(assocRootHost);
	QSharedPointer<AssociateRoot> uiRoot = assocRootHost->getAssociateRoot("UIData");
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
			latestMessage_ = errorOutput.toLatin1();
			return IMPORT_FAILED_ROLLBACK;
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
			latestMessage_ = "Invalid data.";
			return IMPORT_FAILED_ROLLBACK;
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

		assocElem->postLinkUpdate();
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
		//If this is an analysis import, don't adjust positions.
		//If its an analysis copy, move the Analysis Elements that are linked to the paste parent level only.
		//This was not a problem with Experimental Elements because our list only
		//included those top level assets.
		if(isAnalysisImport || assocElem->getLinkedAsset() != pasteParent)
			continue;
		elemsToMove.append(elem.staticCast<UIEnabled>());
		if(elemsToMove.last()->getPos().isNull())
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
		QString failedLinkString = "All Analysis elements in the following paths were skipped since appropriate parent elements could not be found for them.\n";
		foreach(QSharedPointer<Asset> asset,failedLinks)
		{
			failedLinkString.append(dynamic_cast<AssociateElement*>(asset.data())->getParentPath()+"\n");
		}
		latestMessage_ = failedLinkString;
		return IMPORT_SUCCEEDED_WITH_WARNINGS;
	}
	return IMPORT_SUCCEEDED;
}

/*! \brief Returns the type of command that can be performed with the input text where the input text should have been
*	produced with exportToString().
 *	\details The returned value is a AssetExportImport::TEXT_COMMAND_TYPE.
 *	\note The returned value is based entirely on the contents of the input text string.
 */
int AssetExportImport::commandTypeOfText(QString text)
{
	int returnVal = NONE;
	QSharedPointer<QXmlStreamReader> xmlStreamReader(new QXmlStreamReader(text));
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
			if(xmlStreamReader->readNext() == QXmlStreamReader::Invalid)
			{
				error = true;
				break;
			}
			copiedFrom = xmlStreamReader->text().toString();
		}
		else if(tagName == "AnalysisCopy" || tagName == "AnalysisExport")
		{
			if(tagName == "AnalysisCopy")
				returnVal = ANALYSIS_PASTE;
			else
				returnVal = ANALYSIS_IMPORT;
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
			returnVal = EXPERIMENT_PASTE;
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
		return NONE;
	}

	//Verify that necessary data was included in the copied string
	if(!(hadAnalysis || hadUI) || !hadCopiedFrom || !(hadAnalysis || hadExperiment) || !elems.size())
	{
		return NONE;
	}

	if(hadAnalysis && hadExperiment)
	{
		return NONE;
	}

	return returnVal;
}

/*! \brief A convenience function used with ImportFromText() that reads in text from the input xmlReader at the current
 *	read pointer up to the appearance of the input endTag.
 *	\details Output text is set into the output string reference.  Returns true on success, false on failure due to
 *	invalid syntax, not finding the end tag, or other problems.
 */
bool AssetExportImport::getContentsUntilEndTag(QSharedPointer<QXmlStreamReader> xmlReader, QString endTag, QString& output)
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