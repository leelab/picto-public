#ifndef _PAUSEPOINT_H_
#define _PAUSEPOINT_H_

#include "../common.h"
#include "StateMachineElement.h"
#include "../engine/PictoEngine.h"
#include "scene.h"

namespace Picto {

/*!	\brief A PausePoint is the place where execution is suspended when pause is pressed
 */

#if defined WIN32 || defined WINCE
class PICTOLIB_API PausePoint : public StateMachineElement
#else
class PausePoint : public StateMachineElement
#endif
{
	Q_OBJECT
public:
	PausePoint();
	virtual ~PausePoint(){};
	static QSharedPointer<Asset> Create();

	QString run(QSharedPointer<Engine::PictoEngine> engine);
	QString runAsSlave(QSharedPointer<Engine::PictoEngine> engine);
	virtual QString getUITemplate(){return "PausePoint";};
	virtual QString assetType(){return "PausePoint";};

protected:
	virtual void postDeserialize();
	virtual bool validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader);

private:
	QSharedPointer<Scene> scene_;
	QSharedPointer<SignalChannel> sigChannel_;
	bool hasCursor_;
};


}; //namespace Picto

#endif
