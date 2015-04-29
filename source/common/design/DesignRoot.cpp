#include <QMessageBox>

#include "DesignRoot.h"

#include "../statemachine/UIData.h"
#include "../parameter/AssociateRootHost.h"
#include "../parameter/AssociateRoot.h"
#include "../storage/OldVersionAsset.h"

#include "../memleakdetect.h"
using namespace Picto;

DesignRoot::DesignRoot()
	: undoStateOffset_(0)
{

}
/*! \brief Rebuilds the Experimental Design from a saved xml string.
 *	\details Returns true if the deserialization was successful, and false otherwise.  If false was returned,
 *	the deserialization error can be checked with getLastError().  If deserialization was successful, there still
 *	may be a warning message that the designer should see.  Check for this using hasWarning() and read it using
 *	getLastWarning().
 *
 *	This function also has the capability to automatically upgrade the input design text.  If it does so, a warning
 *	string will be available to that effect.  Obviously, the original design file remains untouched until the designer
 *	saves these changes back out.
 */
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

	//Has the effect of serializing the root object tree into undoStack
	setUndoPoint();
	setUnmodified();

	emit refreshedFromXml();
	return true;
}

/*! \brief Attempts to create an analysis based on the input design text and import it into the design.
 *	\details If the import fails, the function returns an empty pointer.  If it succeeds it returns the imported analysis
 *	object.
 */
QSharedPointer<Asset> DesignRoot::importAnalysis(const QString &analysisText)
{
	if(!pictoData_)
		return QSharedPointer<Asset>();
	QSharedPointer<Asset> newAsset = pictoData_->importChildAsset(analysisText,QString());
	return newAsset;
}

/*! \brief Attempts to create an analysis and import it into the design.
 */
QSharedPointer<Asset> DesignRoot::createAnalysis()
{
	QSharedPointer<Asset> newAnalysis = importAnalysis("<Analysis/>");
	Q_ASSERT(newAnalysis);

	//Create UI Data for the new Analysis and attach it
	AssociateRootHost* assocRootHost = dynamic_cast<AssociateRootHost*>(newAnalysis.data());
	Q_ASSERT(assocRootHost);
	QUuid hostId = assocRootHost->getHostId();
	QSharedPointer<Asset> newUIData = newAnalysis->getParentAsset().staticCast<DataStore>()->createChildAsset("UIData", QString(), QString());
	QString feedback;
	newUIData.staticCast<AssociateRoot>()->LinkToAsset(newAnalysis, feedback);

	return newAnalysis;
}

/*! \brief Attempts to create a new Task.
 *	\details If the creation fails, the function returns an empty pointer.  If it succeeds it returns the created object.
 */
QSharedPointer<Asset> DesignRoot::createTask()
{
	if (!pictoData_)
		return QSharedPointer<Asset>();
	Q_ASSERT(pictoData_->getExperiment());
	QSharedPointer<Asset> newAsset = pictoData_->getExperiment()->importChildAsset("<Task/>", QString());
	return newAsset;
}

/*! \brief Removes the analysis with the input index from the design where index is a function of the creation/import order.
 *	\details The removal is performed by marking the analysis as deleted.  This means that it will not be serialized out
 *	with the rest of the design, so if you refreshFromXml() the analysis will dissappear.  If you don't refreshFromXml()
 *	though, the analysis will remain until you do so.
 *	/sa refreshFromXml(), getNumAnalyses(),getAnalysis()
 */
bool DesignRoot::removeAnalysis(int index)
{
	QSharedPointer<Asset> analysis = getAnalysis(index);
	if(!analysis)
		return false;
	analysis->setDeleted();
	return true;
}

//! An attempt to truly delete analysis - Use only when safe to do so!
bool DesignRoot::cullAnalysis(int index)
{
	if (removeAnalysis(index))
	{
		return pictoData_->killGeneratedChildren("Analysis", index);
	}

	return false;
}

/*! \brief Returns a shared pointer to this design's Experiment.
 *	\note While a design may contain any number of Analyses, it contains only one Experiment.
 */
QSharedPointer<Asset> DesignRoot::getExperiment()
{
	if(!pictoData_)
		return QSharedPointer<Asset>();
	return pictoData_->getExperiment();
}
/*! \brief Returns the number of analyses currently included in this design
*/
int DesignRoot::getNumAnalyses()
{
	if(!pictoData_)
		return 0;
	return pictoData_->getGeneratedChildren("Analysis").size();
}

/*! \brief Returns a pointer to attached analysis at the input index */
QSharedPointer<Asset> DesignRoot::getAnalysis(int index)
{
	if(index >= getNumAnalyses() || index < 0)
		return QSharedPointer<Asset>();
	return pictoData_->getGeneratedChildren("Analysis")[index];
}

/*! \brief Sets the current state of the design as an undo point.
 *	\details This means that once the next change is made, if someone calls undo, the experiment will return to the state
 *	that it was in when this function was called.  There are certainly vastly more efficient ways to handle undo than
 *	what we're doing here, but in the interest of saving time, we are simply serializaing out the entire design every time 
 *	this function is called, and changing an underlying QTextDocument (which knows how to handle undo and redo)
 *	accordingly.  To perform an undo, we simply call undo on the QTextDocument and then resetDesignRoot() from the new
 *	text.  Its incredibly inefficient, but it works, albeit slowly.  This would be a good thing to upgrade in the future.
 *	\sa undo(), redo()
 */
void DesignRoot::setUndoPoint()
{
	if(!pictoData_)
		return;
	QString newDesignRootText = pictoData_->toXml();
	if (designUndoStack_.empty() || newDesignRootText != designUndoStack_[undoStateOffset_])
	{
		while (undoStateOffset_ > 0)
		{
			undoStateOffset_--;
			designUndoStack_.pop_front();
		}

		//Sometimes with giant experiment designs this fails to allocate memory
		try	
		{
			designUndoStack_.push_front(newDesignRootText);
		}
		catch(...)
		{
			//Retain at least the last 3 changes
			while (designUndoStack_.size() > 3)
			{
				designUndoStack_.pop_back();
		}
	}

		emit redoAvailable(hasRedo());
		emit undoAvailable(hasUndo());
		
}
}

/*! \brief Returns a pointer to the asset that is set as the one open in the designer window or a null pointer if that
 *	asset couldn't be found.
 */
QSharedPointer<Asset> DesignRoot::getOpenAsset()
{
	QSharedPointer<Asset> experiment = getExperiment();
	if(!experiment)
		return QSharedPointer<Asset>();
	AssociateRootHost* expRootHost = dynamic_cast<AssociateRootHost*>(pictoData_->getExperiment().data());
	QSharedPointer<UIData> uiData = expRootHost->getAssociateRoot("UIData").staticCast<UIData>();
	return experiment->getDesignConfig()->getAsset(uiData->getOpenAsset());
}

/*! \brief Sets the asset that will be considered open in the designer window.
 *	\note This information will be saved along with the other UI information (element positions on the canvas) as part of
 *	a UIData category in the serialized xml design.
 */
void DesignRoot::setOpenAsset(QSharedPointer<Asset> asset)
{
	AssociateRootHost* expRootHost = dynamic_cast<AssociateRootHost*>(pictoData_->getExperiment().data());
	QSharedPointer<UIData> uiData = expRootHost->getAssociateRoot("UIData").staticCast<UIData>();
	uiData->setOpenAsset(asset->getAssetId());
}
/*! \brief Returns true if there is an undo available.
 *	\sa setUndoPoint(), undo()
 */
bool DesignRoot::hasUndo()
{
	return designUndoStack_.size() - undoStateOffset_ > 1;
}
/*! \brief Returns true if there is an undo available.
 *	\sa setUndoPoint(), redo()
 */
bool DesignRoot::hasRedo()
{
	return undoStateOffset_ > 0;
}
/*! \brief Moves the state of the experiment back to what it was when setUndoPoint() was last called.
 *	\details As described in the setUndoPoint() comments, what we actually do here is undo the serialized design document,
 *	then refresh the entire experiment from the XML design code.  It's inefficient.  It should be improved.  But it works,
 *	and it fit our schedule.
 *	\sa setUndoPoint(), hasUndo()
 */
void DesignRoot::undo()
{
	if(!hasUndo())
		return;
	undoStateOffset_++;
	refreshFromXml();
	emit redoAvailable(hasRedo());
	emit undoAvailable(hasUndo());
}
/*! \brief Moves the state of the experiment forward to what it was just before the latest undo() was called.
 *	\details As described in the setUndoPoint() comments, what we actually do here is redo the serialized design document,
 *	then refresh the entire experiment from the XML design code.  It's inefficient.  It should be improved.  But it works,
 *	and it fit our schedule.
 *	\sa setUndoPoint(), hasRedo()
 */
void DesignRoot::redo()
{
	if(!hasRedo())
		return;
	undoStateOffset_--;
	refreshFromXml();
	emit redoAvailable(hasRedo());
	emit undoAvailable(hasUndo());
}
/*! \brief Rebuilds the design from the latest underlying serialized XML design text (see setUndoPoint()).
 *	\details Since currently, deleting assets just sets a deleted flag which causes them to not be serialized out, truly
 *	deleting asset objects from a design means, setting their deleted flag, serializing the experiment to xml, then
 *	reloading the experiment from that text.  At some point, this should be made more efficient, in which case this
 *	function won't be necessary.
 */
void DesignRoot::refreshFromXml()
{
	QString text = designUndoStack_[undoStateOffset_];
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
/*! \brief Returns true if the design has been modified by the designer.
 *	\sa setUndoPoint(), undo()
 */
bool DesignRoot::isModified()
{
	//It is important to return False if no design is loaded, because of how the Workstation boots up.
	if (designUndoStack_.isEmpty())
	{
		return false;
}
	return lastSaveVersion_ != designUndoStack_[undoStateOffset_];
}
/*! \brief Sets the latest version of the design as unmodified
 *	\sa setUndoPoint(), undo()
 */
void DesignRoot::setUnmodified()
{
	lastSaveVersion_ = designUndoStack_[undoStateOffset_];
	/*
	while (designUndoStack_.size() > 1)
	{
		designUndoStack_.pop_back();
}
	*/
}
/*! \brief Returns an xml serialized version of this experiment
*/
QString DesignRoot::getDesignRootText()
{
	setUndoPoint();
	return designUndoStack_[undoStateOffset_];
}

/*! \brief Returns true if the current Design succesfully compiles without errors (ie. has valid syntax).
 *	\details If the function returns true, a descriptiong of the compilation errors will appear in the string pointed to
 *	by the errors input.
 *
 *	\note Rather than recompiling every time this function is called, a compiled_ flag is maintained to mark if a new
 *	compiliation needs to occur or not.  The run time of this function is therefore highly dependent on whether the
 *	design has been changed since the last time it was called.  It should also be pointed out that there is no need to
 *	actually "compile" Picto design code.  The code is simply interpreted by Picto and turned into a design tree in RAM
 *	that is run to perform an experiment.  Compiling is therefore simply checking for valid syntax.  If a design has valid
 *	syntax we say that it compiles.  This does not preclude runtime errors in Javascript however.  Javascript is so
 *	flexible that it is notoriously hard to catch javascript syntax errors before runtime.  Any syntax errors that are
 *	catchable will be caught in this function; however, runtime errors will not be exposed until the experiment is tested,
 *	and these will show up in the debug viewer during the test run.
 */
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

/*! \brief Enables/Disables run mode for all assets in the design
 *	\details Since every property has 3 underlying values, savedValue, initValue and runValue, we need to set the
 *	operation mode of the properties depending on what we are doing.  When we are designing the experiment, we can
 *	alter any of the values that we want, by default we just set all 3 at once.  When we are running an experiment,
 *	scripts update only the runValue and parameter changes by the operator change the initValue which is then set to
 *	the runValue each time a property's parent enters scope.  Depending on which runMode we are in, the standard setValue
 *	function on Property objects functions differenlty, setting the appropriate underlying value, so we need to make
 *	sure to use this function to assure that our updates are going to the right place.  This is important because, for
 *	example, if we are building a Design and testing it during the course of development, we want to make sure that when
 *	we save the design out nothing that happened during a test will affect the saved file.
 *	\sa Property::setValue()
 */
void DesignRoot::enableRunMode(bool runMode)
{
	if(!pictoData_)
		return;
	pictoData_->enableRunModeForDescendants(runMode);
}

/*! \brief Marks this design as "not compiled".
 *	\details This is called every time anything in the underlying design is edited.  It sets the compiled_ flag to false
 *	so that the compiles() function will not that it has to recheck the design syntax before it can return true.
 *	\sa compiles()
 */
void DesignRoot::designEdited()
{
	compiled_ = false;
}