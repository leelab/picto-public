#include <QMap>
#include <QPair>
#include <QDebug>
#include <QScriptValue>
#include <QScriptValueIterator>
#include <QAction>
#include <QTextEdit>
#include <QCoreApplication>

#include "OutputElementContainer.h"
#include "../parameter/AssociateElement.h"
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

void OutputElementContainer::ClearAssociateChildren(QUuid associateId)
{
	StateMachineElement::ClearAssociateChildren(associateId);

	//Go through the outputelements list and remove all associate children with the input associate id.
	bool somethingWasRemoved = false;
	AssociateElement* associateElem;
	for(QList<QSharedPointer<OutputElement>>::iterator iter = outputElements_.begin();iter!=outputElements_.end();)
	{
		associateElem = dynamic_cast<AssociateElement*>(iter->data());
		if(!associateElem)
		{
			iter++;
			continue;
		}
		if(associateElem->getAssociateId() == associateId)
		{
			iter = outputElements_.erase(iter);
			somethingWasRemoved = true;
			continue;
		}
		iter++;
	}
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