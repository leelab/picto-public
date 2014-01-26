#include "ResultContainer.h"
#include "../memleakdetect.h"

using namespace Picto;

/*! \brief Constructs a new ResultContainer object.
 *	\details This constructor sets up an AssetFactory for creating Result objects.
 */
ResultContainer::ResultContainer()
: resultFactory_(new AssetFactory(0,-1))
{
	defineResultFactoryType("",QSharedPointer<AssetFactory>(new AssetFactory(0,-1,AssetFactory::NewAssetFnPtr(Result::Create))));
	AddDefinableObjectFactory("Result",resultFactory_);
}

void ResultContainer::enteredScope()
{
	ScriptableContainer::enteredScope();
	//Clear the latest result whenever this ResultContainer enters scope so that the 
	//result from last time through won't be returned from getLatestResult().
	latestResult_ = "";
}

/*! \brief Returns a list of all possible results in this ResultContainer.
 */
QStringList ResultContainer::getResultList()
{
	return results_.keys();
}

/*! \brief Returns the Result with the input name.
*/
QSharedPointer<Result> ResultContainer::getResult(QString name)
{
	if(results_.contains(name))
		return results_[name];
	return QSharedPointer<Result>();
}

bool ResultContainer::hasEditableDescendants()
{
	int maxResults = 0;
	int currMax;
	//Add up maxAssets of all types of all resultFactories and put it in maxResults.
	//If any of them have a -1 max, it means that there is no limit and therefore
	//There are user addable results and we should return true.
	foreach(QSharedPointer<AssetFactory> assetFactory,resultFactoryByType_)
	{
		foreach(QString type,assetFactory->getTypes())
		{
			currMax = assetFactory->getMaxAssets(type);
			if(currMax < 0)
				return true;
			maxResults += currMax;
		}
	}

	//Required assets are not editable, all others are.  If the number of required results equals
	//maxResults, there are no editable descendants; otherwise, there are.
	int numRequiredResults = requiredResults_.size();
	Q_ASSERT(numRequiredResults <= maxResults);
	if(numRequiredResults == maxResults)
		return false;
	return true;
}

/*! \brief Returns the name of the latest Result to have been triggered by this ResultContainer.
 *	\note This function will only work properly if the class that is actually handling the internal
 *	run logic tells this container class what the triggered result is using setLatestResult().
 */
QString ResultContainer::getLatestResult()
{
	return latestResult_;
}


/*! \brief Adds a required result to this ResultContainer with the input resultName.  
 *	\details The idea here is that some elements contain certain Results by default.  A StopwatchController for example
 *	always contains a "Success" result that is triggered when the element times out.  This function is used to add these 
 *	types of Results.  Some elements have some required, and some optional results.  In cases like these, setMaxOptionalResults()
 *	should be called first if it is necessary to limit the number of optional results.  After that, any required results can be
 *	added.  An example of an element with some required and some optional results is the ChoiceController.  It includes some
 *	required results like "Broke Fixation" and "Total Time Exceeded" but includes optional ControlTargetResults as well which
 *	are used to define the targets that the test subject should be fixating on.
 *
 *	This should be called in a child constructor.  If a result with this result's name isn't serialized in, it will be added 
 *	automatically in postDeserialize().
*/
void ResultContainer::addRequiredResult(QString resultName)
{
	if(maxOptionalResults_[""] != -1)
	{
		//This will increase the max results value in the factory according to 
		//the new number of required results of this type.  If we didn't do
		//this, we couldn't get a new result from the factory below.
		//Note: if we didn't add the new result to requiredResults_ below, this
		//would make the factories max Assets inconsistant with requireResults_.
		resultFactoryByType_[""]->setMaxAssets(resultFactoryByType_[""]->getMaxAssets()+1);
		resultFactoryByType_[""]->setMinAssets(resultFactoryByType_[""]->getMinAssets()+1);
	}
	QSharedPointer<Result> requiredResult = createChildAsset("Result","",QString()).staticCast<Result>();
	Q_ASSERT(requiredResult);
	requiredResult->setName(resultName);
	requiredResult->getPropertyContainer()->getProperty("Name")->setGuiEditable(false);
	requiredResults_.insert("",requiredResult);
}

/*! \brief Sets the maximum number of allowed optional results of the input type to the input max value.
 *	\details Regular results always have a type of "".  Other results (ie. ControlTargetResults for a 
 *	ChoiceController) may have other type names that are set up in a call to defineResultFactoryType().
 *	\sa defineResultFactoryType(), addRequiredResults()
 */
void ResultContainer::setMaxOptionalResults(int max, QString type)
{
	Q_ASSERT_X(resultFactoryByType_.contains(type),"ResultContainer",
		QString("You must add an asset factory for type: \"%1\" before calling setMaxOptionalResults"
		" for that type.").arg(type).toLatin1());
	maxOptionalResults_[type] = max;
	resultFactoryByType_[type]->setMaxAssets(max+requiredResults_.count(type));
}

/*! \brief Call this function to add a result factory for a non-standard Result type.
 *	\details The input type is a name for this type of Result.  The input resultFactory is an AssetFactory
 *	used to generate that type of Result.  
 *	\note The signicance of the type string is that it will be the value in the 'type' field of the
 *	xml design file.  For example, in the ChoiceController, this function is used to set up an AssetFactory
 *	for ControlTargetResult objects under the name "Target".  The XML for that type of result will then be: 
 *	\code <Result type='Target'>...</Result> \endcode
 */
void ResultContainer::defineResultFactoryType(QString type,QSharedPointer<AssetFactory> resultFactory)
{
	resultFactoryByType_[type] = resultFactory;
	resultFactory_->addAssetType(type,resultFactory);
	maxOptionalResults_[type] = -1;
}

void ResultContainer::postDeserialize()
{
	ScriptableContainer::postDeserialize();
	//Check child list to see if any of the requiredResults were serialized in, if not, add them to the child
	//list.
	QList<QSharedPointer<Asset>> deserializedResults = getGeneratedChildren("Result");
	for(QMultiMap<QString,QSharedPointer<Result>>::iterator it = requiredResults_.begin();it!=requiredResults_.end();it++)
	{
		bool found = false;
		foreach(QSharedPointer<Asset> deserializedResult,deserializedResults)
		{
			if(deserializedResult->getName() == it.value()->getName())
			{
				found = true;
				break;
			}
		}
		if(!found)
		{
			AddChild("Result",it.value());
			//Since the requiredResults weren't generated by the factory, we need to increment the number
			//of generated results so that the factory will no if it can generate any more of the given type.
			resultFactoryByType_[it.key()]->setGeneratedAssets(resultFactoryByType_[it.key()]->getGeneratedAssets()+1);
		}
	}

	//Add all child results to results_ map.
	results_.clear();
	QList<QSharedPointer<Asset>> results = getGeneratedChildren("Result");
	foreach(QSharedPointer<Asset> result,results)
	{
		results_[result->getName()] = result.staticCast<Result>();
	}
}

bool ResultContainer::validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	if(!ScriptableContainer::validateObject(xmlStreamReader))
		return false;
	return true;
}

/*! \brief Sets the name of the latest Result to have been triggered by this ResultContainer so that it will be accessible from getLatestResult().
 *	\note This function must be called by the class that is actually handling the internal run logic.  It should be called just before triggering 
 *	the result script in all ResultContainers such that any result script that calls getLatestResult() on its container will get its own name in return.
 */
void ResultContainer::setLatestResult(QString latestResult)
{
	latestResult_ = latestResult;
}