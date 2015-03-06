#ifndef _OPERATORCLICKPARAMETER_H_
#define _OPERATORCLICKPARAMETER_H_

#include "../common.h"

#include "parameter.h"

namespace Picto {

/*! \brief A Parameter that can be used in an Experiment to detect when the Operator (researcher controlling the experiment)
 *	clicks on their expimental view window in the Remote Viewer.
 *	
 *	This Parameter uses two Properties, NewClick and Pos.  Pos always holds the latest Operator click position and is 
 *	accessible in javascript using the x, and y script properties.  NewClick is set to true whenever the Operator clicks
 *	in the viewer window and may be set back to false by experimental code.  It is also initialized to false when the
 *	OperatorClickParameter enters scope.  Typical usage would be:
 *	\code
		//If the operator clicked
		if(operatorClickParamName.newClick)
		{
			//Move some graphic to the click location
			someGraphic.x = operatorClickParamName.x;
			someGraphic.y = operatorClickParamName.y;
			//Reset the newClick flag so that we can detect when another new click comes in
			operatorClickParamName.newClick = false;
		}
	\endcode
 *	\note There is a design error in the way that OperatorClickParameters are updated with new Operator click data.
 *	It is the same problem that is described in the SignalValueParameter, so if this class is fixed, that one should be looked at too.
 *	Currently, the Create() function causes all newly generated OperatorClickParameters to be added to a static list.
 *	Whenever an OperatorClick occurs or information about that click arrives over the network, addClick() is called
 *	which sends the click information to all OperatorClickParameters in the list.  The problem here is that we cannot
 *	be sure that two separate Experiments won't be open and running at the same time in the same application.  In this
 *	case, Operator clicks in one would end up being delivered to OperatorClickParameter's in the other.  We dealt with
 *	this kind of issue in the FrameResolutionTimer by creating a FrameTracker class, such that each experiment has a
 *	single FrameTracker object that is passed into all FrameResolutionTimer objects in a single experiment.  Each
 *	Experiment's FrameTracker can then be updated separately, and its updates be passed to only the FrameResolutionTimers
 *	in its own experiment.  We should really set something like this up for the OperatorClickParameter, but in the 
 *	meantime the OperatorClickParameter is really only used for calibration purposes and not "mission critical" so 
 *	the current system will work for now.
 *	\author Trevor Stavropoulos, Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2015
 */
#if defined WIN32 || defined WINCE
	class PICTOLIB_API OperatorClickParameter : public Parameter
#else
class OperatorClickParameter : public Parameter
#endif
{
	Q_OBJECT
	/*! \brief Returns the x component of the latest operator click position.
	 *	\note Setting this value does nothing.
	 */
	Q_PROPERTY(int x READ getX WRITE setX);
	/*! \brief Returns the y component of the latest operator click position.
	 *	\note Setting this value does nothing.
	*/
	Q_PROPERTY(int y READ getY WRITE setY);
	/*! \brief Returns true if there has been a new click since the experiment started or since the last time
	 *	that setNewClick(false) was called.
	 *	\details The idea here is that newClick will be set to true whenever a new Operator click occurs.  In
	 *	order to differentiate two Operator clicks in a row, the Experimental code will have to set newClick
	 *	back to false in between them.  For this reason, as a rule of thumb, whenever newClick's value is used
	 *	it should be set back to false afterwards. ie.
	 *	\code
			//Find out if there was a new click
			var newClickHappened = OperatorClickParameterName.newClick;
			//Set the new click flag back to false so that we can see when another new click occurs
			OperatorClickParameterName.newClick = false;
		\endcode
	 */
	Q_PROPERTY(bool newClick READ getNewClick WRITE setNewClick);

public:

	static QSharedPointer<Asset> Create();
	virtual ~OperatorClickParameter(){};

	/*! \brief Returns the x component of the latest operator click position.*/
	int getX(){propertyContainer_->setPropertyValue("NewClick",false);return propertyContainer_->getPropertyValue("Pos").toPoint().x();};
	/*! \brief This function doesn't do anything*/
	void setX(int){};
	/*! \brief Returns the y component of the latest operator click position.*/
	int getY(){propertyContainer_->setPropertyValue("NewClick",false);return propertyContainer_->getPropertyValue("Pos").toPoint().y();};
	/*! \brief This function doesn't do anything*/
	void setY(int){};
	/*! \brief Returns the current value of the NewClick property.
	 *	\details The NewClick Property is set to true whenever the Operator performs a new click.  It is only ever set to false
	 *	by the user (or when the experiment is first started).
	 *	\note As of syntax version 0.0.1 we changed getNewClick() such that it no longer updates the value of "NewClick".  In older
	 *	versions, every time getNewClick() was called, "NewClick" was set back to false.  Now that is no longer happening.  This means that older
	 *	experiments will need to use \code OperatorClickParameter.newClick \endcode and then reset newClick with \code OperatorClickParameter.newClick = false; \endcode 
	 *	in order to continue to function properly.
	 */
	bool getNewClick(){return propertyContainer_->getPropertyValue("NewClick").toBool();};
	/*! \brief Sets the "NewClick" Property to the input value.
	*/
	void setNewClick(bool value){propertyContainer_->setPropertyValue("NewClick",value);};
	void setLastClick(QPoint point);
	static void addClick(QPoint point);

	virtual QString friendlyTypeName(){return "Operator Click";};
	virtual QString getUITemplate(){return "OperatorClick";};

protected:
	virtual void postDeserialize();
	virtual bool validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader);
private:
	OperatorClickParameter();
	static QList<QWeakPointer<OperatorClickParameter>> params_;

};


}; //namespace Picto

#endif
