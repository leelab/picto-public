#ifndef _CONTROLLINK_H_
#define _CONTROLLINK_H_

#include <QObject>

#include "../statemachine/Transition.h"

namespace Picto {

/*!	\Brief A ControlLink between a control element and a state's result.
 *
 *	A ControlLink links a specific Result value from a specific ControlElement
 *	to a result in its parent state.  Everything is referenced by by name.
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
	ControlLink(QString source, QString sourceResult, QString destination);
	static QSharedPointer<Asset> Create();

	virtual QString assetType(){return "ControlLink";};

protected:
	virtual QString defaultTagName(){return "Link";};

};


}; //namespace Picto

#endif
