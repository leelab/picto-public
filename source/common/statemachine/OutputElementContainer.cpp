#include <QMap>
#include <QPair>
#include <QDebug>
#include <QScriptValue>
#include <QScriptValueIterator>
#include <QAction>
#include <QTextEdit>
#include <QCoreApplication>

#include "OutputElementContainer.h"
#include "../parameter/AnalysisElement.h"
#include "../memleakdetect.h"

using namespace Picto;

OutputElementContainer::OutputElementContainer()
{
}

/*! \brief Copies the OutputElements from the passed in container
 *	
 *	This is used when adding an child element to a parent OutputElementContainer.  By passing
 *	in the parent OutputElement container, we now have access to all 
 *	of the parent's OutputElements.  This allows us to
 *	have OutputElements with Picto scope.
 */
void OutputElementContainer::addOutputElements(QSharedPointer<OutputElementContainer> outputElementContainer)
{
	addOutputElements(outputElementContainer.data());
}

void OutputElementContainer::addOutputElements(OutputElementContainer *outputElementContainer)
{
	QList<QSharedPointer<OutputElement>> OutputElements = outputElementContainer->getOutputElementList();
	foreach(QSharedPointer<OutputElement> OutputElement,OutputElements)
	{
		addOutputElement(OutputElement);
	}
}

void OutputElementContainer::addChildOutputElementContainer(QSharedPointer<OutputElementContainer> child)
{
	outputElementContainers_.push_back(child);
	child->addOutputElements(this);
}

//! \brief Returns a list of all contained parameters
QList<QSharedPointer<OutputElement>> OutputElementContainer::getOutputElementList()
{
	return outputElements_;
}

void OutputElementContainer::ClearAnalysisChildren(QUuid analysisId)
{
	StateMachineElement::ClearAnalysisChildren(analysisId);

	//Go through the outputelements list and remove all analysis children with the input analysis id.
	bool somethingWasRemoved = false;
	AnalysisElement* analysisElem;
	for(QList<QSharedPointer<OutputElement>>::iterator iter = outputElements_.begin();iter!=outputElements_.end();)
	{
		analysisElem = dynamic_cast<AnalysisElement*>(iter->data());
		if(!analysisElem)
			continue;
		if(analysisElem->getAnalysisId() == analysisId)
		{
			iter = outputElements_.erase(iter);
			somethingWasRemoved = true;
			continue;
		}
		iter++;
	}
}

void OutputElementContainer::enableRunMode(bool enable)
{

}

void OutputElementContainer::postDeserialize()
{
	StateMachineElement::postDeserialize();
	QStringList childTags = getDefinedChildTags();
	foreach(QString childTag,childTags)
	{
		QList<QSharedPointer<Asset>> tagChildren = getGeneratedChildren(childTag);
		foreach(QSharedPointer<Asset> tagChild,tagChildren)
		{
			addChildToElementLists(tagChild);
		}
	}
	connect(this,SIGNAL(childAddedAfterDeserialize(QSharedPointer<Asset>)),this,SLOT(addChildToElementLists(QSharedPointer<Asset>)));
}

bool OutputElementContainer::validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	if(!StateMachineElement::validateObject(xmlStreamReader))
		return false;
	return true;
}

void OutputElementContainer::addOutputElement(QSharedPointer<OutputElement> outputElement)
{
	//If the OutputElement is empty return
	if(outputElement.isNull())
		return;
	//Add all visual elements to the list whether they are part of the experiment or analysis
	outputElements_.push_back(outputElement);

	foreach(QSharedPointer<OutputElementContainer> child,outputElementContainers_)
	{
		child->addOutputElement(outputElement);
	}
}

//If a child is added to this element after postDeserialize then we may need to add it
//using addOutputElement() or addChildOutputElementContainer().  We handle that here.
void OutputElementContainer::addChildToElementLists(QSharedPointer<Asset> newChild)
{
	if(newChild.dynamicCast<OutputElement>())
		addOutputElement(newChild.staticCast<OutputElement>());
	if(newChild.dynamicCast<OutputElementContainer>())
		addChildOutputElementContainer(newChild.staticCast<OutputElementContainer>());

}