#include "DesignConfig.h"
#include "Asset.h"
#include "../statemachine/Transition.h"
#include "../memleakdetect.h"

using namespace Picto;

DesignConfig::DesignConfig()
{
	reset();
}

/*! \brief Clears out all of this object's saved data.  Returning it to its initial state.*/
void DesignConfig::reset()
{
	assetsById_.clear();
	managedElements_.clear();
	managedProperties_.clear();
	managedTransitions_.clear();
	elemInfo_.clear();
	propInfo_.clear();
	transInfo_.clear();
	assetHash_.clear();
	unsortedIdAssets_.clear();
	lastUsedId_ = 0;
	allowIdDuplication_ = true;
	analysisHash_.clear();
	frameTimerFactory_ = QSharedPointer<Controller::FrameTimerFactory>(new Controller::FrameTimerFactory());
}

/*! \brief Serializes this DesignConfig's Property, Transition and Design Element contents to XML for transmission.*/
bool DesignConfig::toXml(QSharedPointer<QXmlStreamWriter> xmlStreamWriter)
{
	QSharedPointer<Asset> sAsset;
	QSharedPointer<Transition> sTAsset;
	
	xmlStreamWriter->writeStartElement("DesignConfig");

	xmlStreamWriter->writeStartElement("Properties");
	foreach(QWeakPointer<Asset> wAsset,managedProperties_)
	{
		sAsset = wAsset;
		if(!sAsset)continue;
		xmlStreamWriter->writeStartElement("Prop");
		xmlStreamWriter->writeAttribute("i",QString::number(sAsset->getAssetId()));
		xmlStreamWriter->writeAttribute("n",sAsset->getName());
		xmlStreamWriter->writeAttribute("p",
			sAsset->getParentAsset()?QString::number(sAsset->getParentAsset()->getAssetId()):0);
		xmlStreamWriter->writeEndElement();
	}
	xmlStreamWriter->writeEndElement();

	xmlStreamWriter->writeStartElement("Transitions");
	foreach(QWeakPointer<Asset> wAsset,managedTransitions_)
	{
		sAsset = wAsset.toStrongRef();
		if(!sAsset)continue;
		sTAsset = sAsset.staticCast<Transition>();
		xmlStreamWriter->writeStartElement("Tran");
		xmlStreamWriter->writeAttribute("i",QString::number(sTAsset->getAssetId()));
		xmlStreamWriter->writeAttribute("n",sTAsset->getName());
		xmlStreamWriter->writeAttribute("p",
			sTAsset->getParentAsset()?QString::number(sTAsset->getParentAsset()->getAssetId()):0);
		xmlStreamWriter->writeAttribute("s",sTAsset->getSource());
		xmlStreamWriter->writeAttribute("r",sTAsset->getSourceResult());
		xmlStreamWriter->writeAttribute("d",sTAsset->getDestination());
		xmlStreamWriter->writeEndElement();
	}
	xmlStreamWriter->writeEndElement();

	xmlStreamWriter->writeStartElement("Elements");
	foreach(QWeakPointer<Asset> wAsset,managedElements_)
	{
		sAsset = wAsset;
		if(!sAsset)continue;
		xmlStreamWriter->writeStartElement("Elem");
		xmlStreamWriter->writeAttribute("i",QString::number(sAsset->getAssetId()));
		xmlStreamWriter->writeAttribute("p",sAsset->getPath());
		xmlStreamWriter->writeEndElement();
	}
	xmlStreamWriter->writeEndElement();
	xmlStreamWriter->writeEndElement();
	return true;
}

/*! \brief Deserializes this DesignConfig's Property, Transition and Design Element contents from XML.*/
bool DesignConfig::fromXml(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	//Do some basic error checking
	if(!xmlStreamReader->isStartElement() || xmlStreamReader->name() != "DesignConfig")
	{
		addError("Incorrect tag, expected <DesignConfig>");
		return false;
	}
	xmlStreamReader->readNext();	//Move past the DesignConfig starting tag
	while(!(xmlStreamReader->isEndElement() && xmlStreamReader->name().toString() == "DesignConfig") && !xmlStreamReader->atEnd())
	{
		if(!xmlStreamReader->isStartElement())
		{
			//Do nothing unless we're at a start element
			xmlStreamReader->readNext();
			continue;
		}

		QString name = xmlStreamReader->name().toString();
		if(name == "Properties")
		{
			xmlStreamReader->readNext();	//Move past the DesignConfig starting tag
			while(!(xmlStreamReader->isEndElement() && xmlStreamReader->name().toString() == "Properties") && !xmlStreamReader->atEnd())
			{
				if(!xmlStreamReader->isStartElement())
				{
					//Do nothing unless we're at a start element
					xmlStreamReader->readNext();
					continue;
				}

				PropInfo propInf;
				if(xmlStreamReader->attributes().hasAttribute("i"))
				{
					propInf.id = xmlStreamReader->attributes().value("i").toString().toInt();
				}
				else
				{
					addError("Property missing i (index) attribute");
					return false;
				}

				if(xmlStreamReader->attributes().hasAttribute("n"))
				{
					propInf.name = xmlStreamReader->attributes().value("n").toString();
				}
				else
				{
					addError("Property missing n (name) attribute");
					return false;
				}

				if(xmlStreamReader->attributes().hasAttribute("p"))
				{
					propInf.parent = xmlStreamReader->attributes().value("p").toString().toInt();
				}
				else
				{
					addError("Property missing p (parent) attribute");
					return false;
				}
				propInfo_.append(propInf);
				xmlStreamReader->readNext();
			}
		}
		else if(name == "Transitions")
		{
			xmlStreamReader->readNext();	//Move past the Transitions starting tag
			while(!(xmlStreamReader->isEndElement() && xmlStreamReader->name().toString() == "Transitions") && !xmlStreamReader->atEnd())
			{
				if(!xmlStreamReader->isStartElement())
				{
					//Do nothing unless we're at a start element
					xmlStreamReader->readNext();
					continue;
				}

				TransInfo transInf;
				if(xmlStreamReader->attributes().hasAttribute("i"))
				{
					transInf.id = xmlStreamReader->attributes().value("i").toString().toInt();
				}
				else
				{
					addError("Transition missing i (index) attribute");
					return false;
				}

				if(xmlStreamReader->attributes().hasAttribute("n"))
				{
					transInf.name = xmlStreamReader->attributes().value("n").toString();
				}
				else
				{
					addError("Transition missing n (name) attribute");
					return false;
				}
				
				if(xmlStreamReader->attributes().hasAttribute("p"))
				{
					transInf.parent = xmlStreamReader->attributes().value("p").toString().toInt();
				}
				else
				{
					addError("Transition missing p (parent) attribute");
					return false;
				}

				if(xmlStreamReader->attributes().hasAttribute("s"))
				{
					transInf.source = xmlStreamReader->attributes().value("s").toString();
				}
				else
				{
					addError("Transition missing s (source) attribute");
					return false;
				}

				if(xmlStreamReader->attributes().hasAttribute("r"))
				{
					transInf.result = xmlStreamReader->attributes().value("r").toString();
				}
				else
				{
					addError("Transition missing r (source result) attribute");
					return false;
				}

				if(xmlStreamReader->attributes().hasAttribute("d"))
				{
					transInf.dest = xmlStreamReader->attributes().value("d").toString();
				}
				else
				{
					addError("Transition missing d (destination) attribute");
					return false;
				}
				transInfo_.append(transInf);
				xmlStreamReader->readNext();
			}
		}
		else if(name == "Elements")
		{
			xmlStreamReader->readNext();	//Move past the Elements starting tag
			while(!(xmlStreamReader->isEndElement() && xmlStreamReader->name().toString() == "Elements") && !xmlStreamReader->atEnd())
			{
				if(!xmlStreamReader->isStartElement())
				{
					//Do nothing unless we're at a start element
					xmlStreamReader->readNext();
					continue;
				}

				AssetInfo elemInf;
				if(xmlStreamReader->attributes().hasAttribute("i"))
				{
					elemInf.id = xmlStreamReader->attributes().value("i").toString().toInt();
				}
				else
				{
					addError("Element missing i (index) attribute");
					return false;
				}

				if(xmlStreamReader->attributes().hasAttribute("p"))
				{
					elemInf.path = xmlStreamReader->attributes().value("p").toString();
				}
				else
				{
					addError("Element missing p (path) attribute");
					return false;
				}
				elemInfo_.append(elemInf);
				xmlStreamReader->readNext();
			}
		}
		else
		{
			addError("Invalid Tag: " + name.toLatin1());
			return false;
		}
		xmlStreamReader->readNext();
	}
	return true;
}

/*! \brief Adds an Error to the underlying Serializable class.
 *	\details Just redirects the input to Serializable::addErrorToList() after adding a new line constant.
 */
void DesignConfig::addError(QString errorMessage)
{
	addErrorToList(errorMessage + "\n");
}

/*! \brief After this function is called, calls to fixDuplicatedAssetIds() will correct any duplicate AssetIds in
 *	Assets that are included in this DesignConfig.  
 *	\details Until this function is called, calling fixDuplicatedAssetIds() does nothing and duplicated AssetIds are allowed.
 */
void DesignConfig::disallowIdDuplication()
{
	allowIdDuplication_ = false;
}

/*! \brief Returns a new AssetId that was not yet used by any Asset included in this object.
 *	\note This naming is historical and should probably be changed to getNewAssetId().
*/
int DesignConfig::getNewDataId()
{
	++lastUsedId_;
	//Increment the id until you find an unused one
	while(assetsById_.contains(lastUsedId_))
	{
		lastUsedId_++;
	}
	return lastUsedId_;
}

/*! \brief Adds the input Asset to this DesignConfig so that it can include it in its list of all Assets in the design and take care of things like
 *	fixing duplicate AssetId values.
 */
void DesignConfig::addManagedAsset(QSharedPointer<Asset> asset)
{
	QWeakPointer<Asset> wAsset(asset);
	if(assetHash_.contains(asset.data()))
		return;
	assetHash_[asset.data()] = wAsset;
	if(asset->inherits("Picto::Transition"))
	{
		managedTransitions_.append(wAsset);
	}
	else if(asset->inherits("Picto::Property"))
	{
		managedProperties_.append(wAsset);
	}
	else
	{
		managedElements_.append(wAsset);
	}
	unsortedIdAssets_.append(wAsset);
}

/*! \brief Calling this function changes AssetIds of Assets that are used in more than one included Asset; however, it only does this
 *	after disallowIdDuplication() is called.
 *	\details We wait for disallowIdDuplication() to be called for multiple reasons, the most important of which is avoiding an n^2 ( or at least n (log n))
 *	complexity algorithm where all Assets are checked for a duplicate Asset Id every time a new one is called.
 */
void DesignConfig::fixDuplicatedAssetIds()
{
	if(allowIdDuplication_)
		return;
	QSharedPointer<Asset> sAsset;
	int id;
	QList<QSharedPointer<Asset>> fixList;
	
	while(unsortedIdAssets_.size())
	{
		QWeakPointer<Asset> wAsset = unsortedIdAssets_.takeFirst();
		sAsset = QSharedPointer<Asset>(wAsset);
		if(!sAsset)
			continue;	//This asset was already deleted
		id = sAsset->getAssetId();
		if(id == 0 || assetsById_.contains(id))
		{
			fixList.append(sAsset);
			continue;
		}
		assetsById_[id] = wAsset;		//Add the asset to the id map with its id.
	}
	while(fixList.size())
	{
		sAsset = fixList.takeFirst();
		id = getNewDataId();		//Get a new asset id and set it to the asset.
		sAsset->setAssetId(id);
		assetsById_[id] = QWeakPointer<Asset>(sAsset);		//Add the asset to the id map with its id.
	}
}

/*! \brief Returns the Asset with the input AssetId or an empty pointer if no such Asset can be found.*/
QSharedPointer<Asset> DesignConfig::getAsset(int id)
{
	QSharedPointer<Asset> returnVal;
	if(assetsById_.contains(id))
		returnVal = assetsById_[id];
	return returnVal;
}
/*! \brief Returns a list (of weak pointers to) all non-Transition, non-Property Assets that were added to this DesignConfig.*/
QList<QWeakPointer<Asset>> DesignConfig::getAssets()
{
	return assetsById_.values();
}
/*! \brief Sets the AnalysisIds (AssociateIds of Analyses) that should be considered active (running) in Assets using this DesignConfig.*/
void DesignConfig::setActiveAnalysisIds(QList<QUuid> analysisList)
{
	analysisHash_.clear();
	foreach(QUuid uuid,analysisList)
	{
		analysisHash_[uuid] = true;
	}
	emit activeAnalysisIdsChanged();
}
/*! \brief Returns a list of all active AnalysisIds in this DesignConfig.
 *	\details In a running experiment, all Analyses with Ids that are returned here will be executed.
 *	\sa setActiveAnalysisIds()
 */
QList<QUuid> DesignConfig::getActiveAnalysisIds()
{
	return analysisHash_.keys();
}

/*! \brief Returns true if the Analysis with the input analysisId is active.
 *	\sa getActiveAnalysisIds(), setActiveAnalysisIds()
 */
bool DesignConfig::isAnalysisActive(QUuid analysisId)
{
	return analysisHash_.contains(analysisId);
}

/*! \brief Called when a new Task run is started.  Emits the runStarted() signal to inform the outside world that the run is staring.
 *	\details This function also generates a unique RunId to be used to reference the new Run.
 *	\note One place where this is used is AnalysisOutput.  File names depend on the name of the Task Run.  This tells AnalysisOutput objects
 *	to start a new Output file/plot/etc and name it according to the latestRunName_.
 */
void DesignConfig::markRunStart(QString runName)
{
	latestRunId_ = QUuid::createUuid();
	latestRunName_ = runName;
	emit runStarted(latestRunId_);
}

/*! \brief Called when the latest Task run ends.  Emits the runEnded() signal to inform the outside world that the run is ending.
 *	\note One place where this is used is AnalysisOutput.  This tells the AnalysisOutput to flush any data that is has been caching because no more
 *	is going to come in and we're done.
 */
void DesignConfig::markRunEnd()
{
	emit runEnded();
}

/*! \brief Returns the Run Id of the latest run.
 *	\sa markRunStart()
 */
QUuid DesignConfig::getLatestRunId()
{
	return latestRunId_;
}

/*! \brief Returns the Run name of the latest run.
 *	\sa markRunStart()
 */
QString DesignConfig::getLatestRunName()
{
	return latestRunName_;
}

/*! \brief A FrameTimerFactory is used to create FrameTimers with correct single frame resolution for the current experiment.
 *	The FrameTimerFactory needs to be associated with a single Experiment and stored in a place where all parts of the Experiment can access it.
 *	The DesignConfig fits the bill, so we put it here.
 */
QSharedPointer<Controller::FrameTimerFactory> DesignConfig::getFrameTimerFactory()
{
	return frameTimerFactory_;
}

/*! \brief Various Analysis elements need access to various data reader classes so that they can poll data over set timespans.  This function sets the FrameReader that
 *	they should use for this experiment.
 *	\details Since All Assets in an Experiment have access to the DesignConfig, this is the place to put it.
 */
void DesignConfig::setFrameReader(QSharedPointer<FrameReader> frameReader)
{
	frameReader_ = frameReader;
}

/*! \brief Various Analysis elements need access to various data reader classes so that they can poll data over set timespans.  This function gets the FrameReader that
 *	they should use for this experiment.
 *	\details Since All Assets in an Experiment have access to the DesignConfig, this is the place to put it.
 */
QSharedPointer<FrameReader> DesignConfig::getFrameReader()
{
	return frameReader_;
}

/*! \brief Various Analysis elements need access to various data reader classes so that they can poll data over set timespans.  This function sets the LfpReader that
 *	they should use for this experiment.
 *	\details Since All Assets in an Experiment have access to the DesignConfig, this is the place to put it.
 */
void DesignConfig::setLfpReader(QSharedPointer<LfpReader> lfpReader)
{
	lfpReader_ = lfpReader;
}

/*! \brief Various Analysis elements need access to various data reader classes so that they can poll data over set timespans.  This function sets the RewardReader that
 *	they should use for this experiment.
 *	\details Since All Assets in an Experiment have access to the DesignConfig, this is the place to put it.
 */
void DesignConfig::setRewardReader(QSharedPointer<RewardReader> rewardReader)
{
	rewardReader_ = rewardReader;
}

/*! \brief Various Analysis elements need access to various data reader classes so that they can poll data over set timespans.  This function sets the RunNotesReader that
 *	they should use for this experiment.
 *	\details Since All Assets in an Experiment have access to the DesignConfig, this is the place to put it.  The RunNotesReader is not quite like the other data reader
 *	classes because it doesn't represent a quantity that changes over time, but it still needs to be set once and available from multiple parts of an Experiment, so
 *	this is sstill a good place to put it.
 */
void DesignConfig::setRunNotesReader(QSharedPointer<RunNotesReader> runNotesReader)
{
	runNotesReader_ = runNotesReader;
}

/*! \brief Various Analysis elements need access to various data reader classes so that they can poll data over set timespans.  This function sets the SpikeReader that
 *	they should use for this experiment.
 *	\details Since All Assets in an Experiment have access to the DesignConfig, this is the place to put it.
 */
void DesignConfig::setSpikeReader(QSharedPointer<SpikeReader> spikeReader)
{
	spikeReader_ = spikeReader;
}

/*! \brief Various Analysis elements need access to various data reader classes so that they can poll data over set timespans.  This function gets the LfpReader that
 *	they should use for this experiment.
 *	\details Since All Assets in an Experiment have access to the DesignConfig, this is the place to put it.
 */
QSharedPointer<LfpReader> DesignConfig::getLfpReader()
{
	return lfpReader_;
}

/*! \brief Various Analysis elements need access to various data reader classes so that they can poll data over set timespans.  This function gets the RewardReader that
 *	they should use for this experiment.
 *	\details Since All Assets in an Experiment have access to the DesignConfig, this is the place to put it.
 */
QSharedPointer<RewardReader> DesignConfig::getRewardReader()
{
	return rewardReader_;
}

/*! \brief Various Analysis elements need access to various data reader classes so that they can poll data over set timespans.  This function gets the RunNotesReader that
 *	they should use for this experiment.
 *	\details Since All Assets in an Experiment have access to the DesignConfig, this is the place to put it.  The RunNotesReader is not quite like the other data reader
 *	classes because it doesn't represent a quantity that changes over time, but it still needs to be set once and available from multiple parts of an Experiment, so
 *	this is sstill a good place to put it.
 */
QSharedPointer<RunNotesReader> DesignConfig::getRunNotesReader()
{
	return runNotesReader_;
}

/*! \brief Various Analysis elements need access to various data reader classes so that they can poll data over set timespans.  This function gets the SpikeReader that
 *	they should use for this experiment.
 *	\details Since All Assets in an Experiment have access to the DesignConfig, this is the place to put it.
 */
QSharedPointer<SpikeReader> DesignConfig::getSpikeReader()
{
	return spikeReader_;
}

/*! \brief Various Analysis elements need access to various data reader classes so that they can poll data over set timespans.  This function sets the SignalReader
 *	 for the input Signal name (ie. "Position") that they should use for this experiment.
 *	\details Since All Assets in an Experiment have access to the DesignConfig, this is the place to put it.
 */
void DesignConfig::setSignalReader(QString name, QSharedPointer<SignalReader> signalReader)
{
	signalReaders_[name.toLower()] = signalReader;
}

/*! \brief Removes all SignalReader objects that were included in this DesignConfig.
*/
void DesignConfig::clearSignalReaders()
{
	signalReaders_.clear();
}

/*! \brief Various Analysis elements need access to various data reader classes so that they can poll data over set timespans.  This function gets the SignalReader that
 *	they should use for this experiment for the input signal name (ie. "Position").
 *	\details Since All Assets in an Experiment have access to the DesignConfig, this is the place to put it.
 */
QSharedPointer<SignalReader> DesignConfig::getSignalReader(QString name)
{
	if(!signalReaders_.contains(name.toLower()))
		return QSharedPointer<SignalReader>();
	return signalReaders_[name.toLower()];
}

/*! \brief Returns a list of AssetInfo data on all Non-Property, Non-Transition Assets that were added to this DesignConfig.*/
QList<AssetInfo> DesignConfig::getElementInfo()
{
	if(!elemInfo_.size())
		fromXml(toXml());
	return elemInfo_;
}
/*! \brief Returns a list of PropInfo data on all Property Assets that were added to this DesignConfig.*/
QList<PropInfo> DesignConfig::getPropertyInfo()
{
	if(!propInfo_.size())
		fromXml(toXml());
	return propInfo_;
}
/*! \brief Returns a list of all TransInfo data on Transition Assets that were added to this DesignConfig.*/
QList<TransInfo> DesignConfig::getTransitionInfo()
{
	if(!transInfo_.size())
		fromXml(toXml());
	return transInfo_;
}