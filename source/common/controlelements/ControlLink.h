#ifndef _CONTROLLINK_H_
#define _CONTROLLINK_H_

#include <QObject>

#include "../statemachine/Transition.h"

namespace Picto {

/*!	\brief UNUSED This used to essentially be a Transition for inside a State.  Now Elements inside a State just used Transition objects too.
 *	\author Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2013
 */

#if defined WIN32 || defined WINCE
	class PICTOLIB_API ControlLink : public Transition
#else
class ControlLink  : public Transition
#endif
{
	Q_OBJECT
public:
	ControlLink();
	virtual ~ControlLink(){};
	ControlLink(QSharedPointer<Asset> source, QSharedPointer<Asset> sourceResult, QSharedPointer<Asset> destination);
	static QSharedPointer<Asset> Create();

	virtual QString friendlyTypeName(){return "Control Link";};

protected:
	virtual QString defaultTagName(){return "Link";};

};


}; //namespace Picto

#endif
