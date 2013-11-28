#include "OperatorClickParameter.h"
#include "../memleakdetect.h"

namespace Picto {

QList<QWeakPointer<OperatorClickParameter>> OperatorClickParameter::params_;

/*! \brief Constructs an OperatorClickParameter
 *	\details Adds two Properties that will be set invisible in postDeserialize():
 *		- "Pos" - Stores the latest Operator click position
 *		- "NewClick" - Is set to true whenever the Operator performs a click and set false by experimental code that uses
 *			the click infomration.
 */
OperatorClickParameter::OperatorClickParameter()
: Parameter()
{
	AddDefinableProperty(QVariant::Point,"Pos",QPoint(0,0));
	AddDefinableProperty(QVariant::Bool,"NewClick",false);
}

/*! \brief Creates a new OperatorClickParameter and returns a shared Asset pointer to it.
*	\details Also adds the new OperatorClickParameter to the static params_ list of OperatorClickParameters.
*	This is important so that operator clicks can be sent to all OperatorClickParameters at once.  See
*	the main OperatorClickParameter documentation for more detail on this.
*/
QSharedPointer<Asset> OperatorClickParameter::Create()
{
	QSharedPointer<Asset> newParam(new OperatorClickParameter());
	//Add this parameter to the list that we will use to send click info.
	params_.append(QWeakPointer<OperatorClickParameter>(newParam.staticCast<OperatorClickParameter>()));
	return newParam;
}

/*! \brief Adds a new operator click to all OperatorClickParameters.
 *	\details Calls setLastClick() on all OperatorClickParameter objects currently loaded in Picto with
 *	the input click point.
 *	\note Using a static function for this isn't ideal and we should probably fix it.  See the main 
 *	OperatorClickParameter documentation for more details.
 */
void OperatorClickParameter::addClick(QPoint point)
{
	QList<QSharedPointer<Property>> affectedProps;
	QList<QWeakPointer<OperatorClickParameter>>::iterator it;
	for(it = params_.begin();it != params_.end();)
	{
		QSharedPointer<OperatorClickParameter> sharedParam = (*it).toStrongRef();
		if(!sharedParam)
		{
			it = params_.erase(it);
			continue;
		}
		sharedParam->setLastClick(point);
		it++;
	}
}

/*! \brief Sets this objects Parameters to reflect a new click at the input position.
 *	\details "Pos" is set to the new click point.
 *	"NewClick" is set to true.
 */
void OperatorClickParameter::setLastClick(QPoint point)
{
	propertyContainer_->setPropertyValue("Pos",point);
	propertyContainer_->setPropertyValue("NewClick",true);
}

void OperatorClickParameter::postDeserialize()
{
	Parameter::postDeserialize();
	propertyContainer_->getProperty("Pos")->setVisible(false);
	propertyContainer_->getProperty("NewClick")->setVisible(false);
}

bool OperatorClickParameter::validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	if(!Parameter::validateObject(xmlStreamReader))
		return false;
	return true;
}

}; //namespace Picto
