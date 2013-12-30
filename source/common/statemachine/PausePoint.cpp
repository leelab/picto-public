#include <QCoreApplication>
#include "PausePoint.h"
#include "LogicResult.h"
#include "../engine/PictoEngine.h"
#include "../stimuli/CursorGraphic.h"
#include "../storage/ObsoleteAsset.h"
#include "../stimuli/OutputElement.h"
#include "../parameter/AssociateElement.h"
#include "../memleakdetect.h"
namespace Picto
{

/*! \brief Constructs a new PausePoint object.
 *	\details Adds a ForcePause Property that, when 'true', causes the Experiment to pause whenever control reaches this PausePoint.
 */
PausePoint::PausePoint()
: scene_(Scene::createScene()),
hasCursor_(false)
{
	//PausePoints don't contain transitions.  All of their results are logic results which differ
	//from regular results in that they don't have scripts.  Replace the default result factory with
	//a factory that creates LogicResults that are required.
	defineResultFactoryType("",QSharedPointer<AssetFactory>(new AssetFactory(0,-1,AssetFactory::NewAssetFnPtr(LogicResult::Create))));
	setMaxOptionalResults(0);
	//AddDefinableProperty("Type","PausePoint");	/*! \todo this shouldn't be a DEFINABLE property, but it needs to be here so that in StateMachine, element->type() gives the correct value.  Do something about this.*/
	AddDefinableProperty(QVariant::Color,"BackgroundColor","");
	AddDefinableObjectFactory("PausingScript",QSharedPointer<AssetFactory>(new AssetFactory(0,-1,AssetFactory::NewAssetFnPtr(ObsoleteAsset::Create))));
	AddDefinableObjectFactory("RestartingScript",QSharedPointer<AssetFactory>(new AssetFactory(0,-1,AssetFactory::NewAssetFnPtr(ObsoleteAsset::Create))));
	AddDefinableProperty(QVariant::Bool,"ForcePause",false);
	addRequiredResult("done");
}

/*! \brief Creates a new PausePoint object and returns a shared Asset pointer to it.*/
QSharedPointer<Asset> PausePoint::Create()
{
	return QSharedPointer<Asset>(new PausePoint());
}

/*! \brief rebuilds the scene whenever run mode is enable in case something about the scene changed.
*/
void PausePoint::enableRunMode(bool enable)
{
	OutputElementContainer::enableRunMode(enable);
	if(!enable)
		return;
	rebuildScene();
}
/*! \brief Runs this PausePoint's execution logic within the framework of the input PictoEngine.
 *	\details This function handles running entry and exit scripts, and, if the engine says so (ie. the operator pressed the pause button),
 *	pausing execution.  When ForcePause is true, this function always pauses until the engines says to resume (ie. the operator presses the play
 *	button again).
 *
 *	While Paused, PausePoints display all in scope OutputElements and this function handles that during the course of the Pause.
 *
 *	Since PausePoint objects always have a single "done" result, this function always returns "done".
*/
QString PausePoint::run(QSharedPointer<Engine::PictoEngine> engine)
{
	resetScriptableValues();
	int command = engine->getEngineCommand();
	//Pause the engine if force pause is enabled
	if( (command != Engine::PictoEngine::StopEngine) 
		&& propertyContainer_->getPropertyValue("ForcePause").toBool())
	{
		//We don't directly pause the engine because then whoever is controlling
		//the engine (componentStatusManager, TestViewer, etc) won't know about it.
		//Instead we use requestPause() to ask the master to pause the engine for us.
		engine->requestPause();

		command = engine->getEngineCommand();
	}
	//If engine is paused, suspend execution until play is pressed
	runEntryScript();
	while(command == Engine::PictoEngine::PauseEngine)
	{
		//Show frame
		scene_->render(engine,getAssetId());
		QCoreApplication::processEvents();
		command = engine->getEngineCommand();
	}
	//If it was a stop that got us out of the pause, return an EngineAbort.
	if(command == Engine::PictoEngine::StopEngine)
	{
		setLatestResult("EngineAbort");
		return "EngineAbort";
	}
	setLatestResult("done");
	getResult("done")->runResultScript();
	runExitScript();
	return "done";
}

/*! \brief When a PausePoint is run as a slave, it really doesn't do anything except render frames until Pause ends.  This function is therefore empty
 *	and slaveRenderFrame() handles frame rendering.
 */
QString PausePoint::slaveRun(QSharedPointer<Engine::PictoEngine> engine)
{
	QString result;
	return result; 
}

/*! \brief Handles rendering of the latest frame to the display during a Pause when the Experiment is running in slave mode.
*/
QString PausePoint::slaveRenderFrame(QSharedPointer<Engine::PictoEngine> engine)
{
	//Add the cursor to the scene if it isn't there already
	if(!hasCursor_)
	{
		sigChannel_ = engine->getSignalChannel("Position");
		QSharedPointer<CursorGraphic> cursor(new CursorGraphic(sigChannel_, QColor(255,50,0,255)));
		cursor->setLayer(100000);
		scene_->addVisualElement(cursor);
		hasCursor_ = true;
	}

	QString result;
	scene_->render(engine,getAssetId());
	
	//---------   Erase the latest cursor values (This happens in master when data is sent to server)
	//sigChannel_->getValues();
	engine->emptySignalChannels();	//Make sure all signal channels are emptied, not just the one that
									//we used.
	return result; 
}

/*! \brief Extends OutputElementContainer::upgradeVersion() to copy any "PausingScript" Property value from an older Design syntax and paste
 *	it into the "EntryScript" Property which now exists on all StateMachineElement objects.
 */
void PausePoint::upgradeVersion(QString deserializedVersion)
{
	OutputElementContainer::upgradeVersion(deserializedVersion);
	if(deserializedVersion < "0.0.1")
	{	//Before 0.0.1, Scripts for pause elements were called PausingScript and RestartingScript.  These
		//have been changed to EntryScript and ExitScript respectively for consistency with the rest of the system.
		//Here we copy contents of the old script properties into the new script properties.
		QList<QSharedPointer<Asset>> childList = getGeneratedChildren("PausingScript");
		Q_ASSERT(childList.size() <= 1);
		if(childList.size())
		{
			getPropertyContainer()->setPropertyValue("EntryScript",childList.first().staticCast<ObsoleteAsset>()->getValue());
		}

		//No one used this so we don't need to do it, and we're getting rid of PausePoint exit scripts.
		//childList = getGeneratedChildren("RestartingScript");
		//Q_ASSERT(childList.size() <= 1);
		//if(childList.size())
		//{
		//	getPropertyContainer()->setPropertyValue("ExitScript",childList.first().staticCast<ObsoleteAsset>()->getValue());
		//}
	}
}

void PausePoint::setDesignConfig(QSharedPointer<DesignConfig> designConfig)
{
	//We need to know whenever Analyses are activated or deactivated, so we connect to the appropriate signal from the DesignConfig.
	if(getDesignConfig())
		disconnect(getDesignConfig().data(),SIGNAL(activeAnalysisIdsChanged()),this,SLOT(activeAnalysisIdsChanged()));
	OutputElementContainer::setDesignConfig(designConfig);
	connect(getDesignConfig().data(),SIGNAL(activeAnalysisIdsChanged()),this,SLOT(activeAnalysisIdsChanged()));
}

void PausePoint::postDeserialize()
{
	OutputElementContainer::postDeserialize();
	setPropertyRuntimeEditable("ForcePause");

	//We're not using this right now, but maybe someday we will, so we're not getting rid of it, just hiding it from
	//the UI.
	propertyContainer_->getProperty("BackgroundColor")->setVisible(false);
}

bool PausePoint::validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	if(!OutputElementContainer::validateObject(xmlStreamReader))
		return false;

	if(propertyContainer_->getPropertyValue("Name").toString() == "EngineAbort")
	{
		addError("EngineAbort is a resticted keyword, and may not be used as the name of a PausePoint");
		return false;
	}

	return true;
}

/*! \brief Initializes the Scene based on all current in scope OutputElement objects so that it can
 *	correctly render the current scene each frame.
 *	\details This needs to be called at least once before a PausePoint is run and after all OutputElement objects
 *	that are in scope have been added.
 */
void PausePoint::rebuildScene()
{
	//Since the scene needs access to visual elements stored above it in the tree, we get
	//our output elements from the output element list.
	scene_ = Scene::createScene();
	hasCursor_ = false;
	QList<QSharedPointer<OutputElement>> outputs = getOutputElementList();
	QList<QUuid> activeAnalyses = getDesignConfig()->getActiveAnalysisIds();
	QHash<QUuid,bool> activeAnalysisHash;
	foreach(QUuid analysisId,activeAnalyses)
	{
		activeAnalysisHash[analysisId] = true;
	}
	foreach(QSharedPointer<OutputElement> output,outputs)
	{
		if(output.isNull())
			continue;
		if(output.dynamicCast<VisualElement>())
		{
			//If this is an analysis element and its analysis id isn't active, don't add it to the scene.
			AssociateElement* analysisVisElem = dynamic_cast<AssociateElement*>(output.data());
			if(analysisVisElem && !activeAnalysisHash.contains(analysisVisElem->getAssociateId()))
				continue;
			scene_->addVisualElement(output.staticCast<VisualElement>());
		}
		else if (output.dynamicCast<OutputSignal>())
		{
			scene_->addOutputSignal(output.staticCast<OutputSignal>());
		}
		else if(output.dynamicCast<AudioElement>())
		{
			scene_->addAudioElement(output.staticCast<AudioElement>());
		}
	}
	QColor backgroundColor;
	backgroundColor.setNamedColor(propertyContainer_->getPropertyValue("BackgroundColor").toString());
	scene_->setBackgroundColor(QColor(propertyContainer_->getPropertyValue("BackgroundColor").toString()));
}

/*! \brief Calls rebuildScene() any time the Active Analysis changes.
 *	\details This should be more useful when/if we start adding Analysis Output Elements (plots or text info)
 *	to the scene in the Workstation when Analyses are active.
*/
void PausePoint::activeAnalysisIdsChanged()
{
	//Even though this happens as a result of a change in the UI.  Since its connected on a signal->slot basis, the function
	//should run in the Experiment thread.  Since the experiment thread never runs during the course of scene rendering (if 
	//they are in two different threads, the experiment thread pauses and waits), there should be no issue of accidentally
	//changing scene values while they are being used in the UI thread.
	rebuildScene();
}


}//namespace Picto