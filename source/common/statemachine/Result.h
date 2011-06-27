#ifndef _RESULT_H_
#define _RESULT_H_

#include "../common.h"
#include "../statemachine/ScriptableContainer.h"

namespace Picto {

/*!	\brief A Result is the exit point from a StateMachineElement or ControlElement
 *	
 */

#if defined WIN32 || defined WINCE
class PICTOLIB_API Result : public ScriptableContainer
#else
class Result : public ScriptableContainer
#endif
{
public:
	Result();
	/*! \brief Constructs new result with input name and option to define result as default.
	 *	Define the result as default if it is added automatically by your ResultContainer and
	 *	need not be serialized out.
	 */
	Result(QString name);
	static QSharedPointer<Asset> Create(){return QSharedPointer<Asset>(new Result());};

	virtual QString assetType(){return "Result";};

protected:
	virtual QString defaultTagName(){return "Result";};
	virtual void postSerialize();
	virtual bool validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader);
};


}; //namespace Picto

#endif
