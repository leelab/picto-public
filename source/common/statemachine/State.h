#ifndef _STATE_H_
#define _STATE_H_

#include "../common.h"
#include "../statemachine/StateMachineElement.h"
#include "../controlelements/ControlElement.h"
#include "scene.h"

namespace Picto {

#if defined WIN32 || defined WINCE
class PICTOLIB_API State : public StateMachineElement
#else
class State : public StateMachineElement
#endif
{
public:
	State();
	QString run();

	bool serializeAsXml(QSharedPointer<QXmlStreamWriter> xmlStreamWriter);
	bool deserializeFromXml(QSharedPointer<QXmlStreamReader> xmlStreamReader);

	void setRevision(int rev) { revision_ = rev; };
	int getRevision() { return revision_; };
	void setEngineNeeded(int eng) { engineNeeded_ = eng; };
	int getEngineNeeded() { return engineNeeded_; };

private:
	QSharedPointer<Scene> scene_;
	QList<QSharedPointer<ControlElement> > controlElements_;
	int revision_;
	int engineNeeded_;
};


}; //namespace Picto

#endif
