#ifndef _TEST_CONTROLLER_H_
#define _TEST_CONTROLLER_H_

#include "../common.h"

#include "ControlElement.h"

namespace Picto {

/*! \brief A simple controller for test purposes
 *
 *	This controller can be modified to do anything for test purposes
 *	Initially, it will simply output to the debuggin console, and return true
 *	the 120th time it is called (2 seconds if we're running at 60Hz).
 */

#if defined WIN32 || defined WINCE
class PICTOLIB_API TestController : public ControlElement
#else
class TestController : public ControlElement
#endif
{
	Q_OBJECT

public:
	TestController();
	virtual ~TestController(){};

	static ControlElement* NewTestController();
	static QSharedPointer<Asset> Create();

	static QString ControllerType();

	bool isDone(QSharedPointer<Engine::PictoEngine> engine);
	QString getResult();
	void start(QSharedPointer<Engine::PictoEngine> engine);

	//DataStore Functions
	//bool serializeAsXml(QSharedPointer<QXmlStreamWriter> xmlStreamWriter);
	//bool deserializeFromXml(QSharedPointer<QXmlStreamReader> xmlStreamReader);
protected:
	virtual void postDeserialize();
	virtual bool validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader);

private:
	int timesCalled_;
};


}; //namespace Picto

#endif
