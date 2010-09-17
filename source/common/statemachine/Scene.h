#ifndef _SCENE_H_
#define _SCENE_H_

#include <QXmlStreamWriter>
#include <QXmlStreamReader>
#include <QScriptEngine>

#include "../common.h"
#include "../storage/DataStore.h"
#include "Canvas.h"
#include "../controlelements/ControlElement.h"
#include "../compositor/RenderingTarget.h"
#include "../engine/PictoEngine.h"

namespace Picto {

/*!	\brief A scene consists of the graphical stimuli
 *
 *	A scene is the thing that actually puts images on the screen.  Scenes 
 *	are closesly tied to the state machine element "State".  Each state must
 *	contain a single Scene.  The scenes also have include the control elements
 *	since those need to get passed down to the rendering loop.
 *
 *	NOTE: At some point we will need to add support for audio elements.  This
 *	will probably require adding them to the scene.
 */
#if defined WIN32 || defined WINCE
class PICTOLIB_API Scene : public DataStore
#else
class Scene : public DataStore
#endif
{
public:
	Scene();

	void setCanvas(QSharedPointer<Canvas> canvas) { canvas_ = canvas; }
	QSharedPointer<Canvas> getCanvas() { return canvas_; };
	//void setAudioElement(....)

	void render(QSharedPointer<Engine::PictoEngine> engine);

	void bindToScriptEngine(QSharedPointer<QScriptEngine> qsEngine);

	void reset();

	//DataStore functions
	bool serializeAsXml(QSharedPointer<QXmlStreamWriter> xmlStreamWriter);
	bool deserializeFromXml(QSharedPointer<QXmlStreamReader> xmlStreamReader);

private:
	QSharedPointer<Canvas> canvas_;
	QMap<QString, QSharedPointer<ControlElement> > controlElements_;
};


}; //namespace Picto

#endif
