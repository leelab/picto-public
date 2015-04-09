#include "StateMachineElementContainer.h"
#include "../memleakdetect.h"

using namespace Picto;

/*! \brief Copies the Elements from the passed in container
 *
 *	This is used when adding an child element to a parent ElementContainer.  By passing in the parent ElementContainer, we
 *	now have access to all of the parent's Elements.  This allows us to have Elements with Picto scope.
 */
void StateMachineElementContainer::addElements(QSharedPointer<StateMachineElementContainer> elementContainer)
{
	addElements(elementContainer.data());
}

/*! \brief Copies the Elements from the passed in container
 *
 *	This is used when adding an child element to a parent ElementContainer.  By passing in the parent ElementContainer, we
 *	now have access to all of the parent's Elements.  This allows us to have Elements with Picto scope.
 */
void StateMachineElementContainer::addElements(StateMachineElementContainer* elementContainer)
{
	QList<QSharedPointer<ContainerElement>> Elements = elementContainer->getElementList();
	foreach(QSharedPointer<ContainerElement> element, Elements)
	{
		addElement(element);
	}
}

/*! \brief Adds the input ElementContainer to this one's list of child ElementContainers and uses the child's
 *	addElements() function to add this one's Elements to it.
 *	\details We need to store child ElementContainers so that whenever a new Element is added to this container, it can be
 *	added automatically to the child's ElementContainers as well.
 */
void StateMachineElementContainer::addChildElementContainer(QSharedPointer<StateMachineElementContainer> child)
{
	elementContainers_.push_back(child);
	child->addElements(this);
}

/*! \brief Returns a list of all Element objects that are in scope for this object.
 */
QList<QSharedPointer<ContainerElement>> StateMachineElementContainer::getElementList()
{
	return elements_;
}

/*! \brief Extends StateMachineElement::ClearAssociateChildren() to remove AssociateChildren with the input associateId
 *	from this element's in-scope elements_ list as well.
 */
void StateMachineElementContainer::ClearAssociateChildren(QUuid associateId)
{
	StateMachineElement::ClearAssociateChildren(associateId);

	//Go through the elements list and remove all associate children with the input associate id.
	bool somethingWasRemoved = false;
	AssociateElement* associateElem;
	for (QList<QSharedPointer<ContainerElement>>::iterator iter = elements_.begin(); iter != elements_.end();)
	{
		associateElem = dynamic_cast<AssociateElement*>(iter->data());
		if (!associateElem)
		{
			iter++;
			continue;
		}
		if (associateElem->getAssociateId() == associateId)
		{
			iter = elements_.erase(iter);
			somethingWasRemoved = true;
			continue;
		}
		iter++;
	}
}

void StateMachineElementContainer::postDeserialize()
{
	StateMachineElement::postDeserialize();
	QStringList childTags = getDefinedChildTags();
	foreach(QString childTag, childTags)
	{
		QList<QSharedPointer<Asset>> tagChildren = getGeneratedChildren(childTag);
		foreach(QSharedPointer<Asset> tagChild, tagChildren)
		{
			addChildToElementLists(tagChild);
		}
	}
	connect(this, SIGNAL(childAddedAfterDeserialize(QSharedPointer<Asset>)), this, SLOT(addChildToElementLists(QSharedPointer<Asset>)));
}

bool StateMachineElementContainer::validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	if (!StateMachineElement::validateObject(xmlStreamReader))
		return false;
	return true;
}

/*! \brief Adds a new Element to this object's in-scope Elements list as well as those of its child ElementContainers.
 */
void StateMachineElementContainer::addElement(QSharedPointer<ContainerElement> element)
{
	//If the Element is empty return
	if (element.isNull())
		return;
	//Add all visual elements to the list whether they are part of the experiment or analysis
	elements_.push_back(element);

	foreach(QSharedPointer<StateMachineElementContainer> child, elementContainers_)
	{
		child->addElement(element);
	}
}

/*! \brief Handles the case where a child is added to this element after postDeserialize().  In that case, depending
 *	on the child's type, we may need to add it using either addElement() or addChildElementContainer().  We handle
 *	that here.
 */
void StateMachineElementContainer::addChildToElementLists(QSharedPointer<Asset> newChild)
{
	if (newChild.dynamicCast<ContainerElement>())
		addElement(newChild.staticCast<ContainerElement>());
	if (newChild.dynamicCast<StateMachineElementContainer>())
		addChildElementContainer(newChild.staticCast<StateMachineElementContainer>());

}