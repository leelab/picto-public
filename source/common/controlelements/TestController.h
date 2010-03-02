/*! \brief A simple controller for test purposes
 *
 *	This controller can be modified to do anything for test purposes
 *	Initially, it will simply output to the debuggin console, and return true
 *	the 120th time it is called (2 seconds if we're running at 60Hz).
 */

#ifndef _TEST_CONTROLLER_H_
#define _TEST_CONTROLLER_H_

#include "../common.h"

#include "ControlElement.h"

namespace Picto {

#if defined WIN32 || defined WINCE
class PICTOLIB_API TestController : public ControlElement
#else
class TestController : public ControlElement
#endif
{
	Q_OBJECT

public:
	TestController();

	static ControlElement* NewTestController();
	static QString ControllerType();

	bool isDone();
	QString getResult();
	void start();

	//DataStore Functions
	bool serializeAsXml(QSharedPointer<QXmlStreamWriter> xmlStreamWriter);
	bool deserializeFromXml(QSharedPointer<QXmlStreamReader> xmlStreamReader);

private:
	int timesCalled_;
};


}; //namespace Picto

#endif
