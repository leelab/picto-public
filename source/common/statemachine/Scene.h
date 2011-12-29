#ifndef _SCENE_H_
#define _SCENE_H_

#include <QXmlStreamWriter>
#include <QXmlStreamReader>

#include "../common.h"
#include "../stimuli/AudioElement.h"
#include "../controlelements/ControlElement.h"
#include "../compositor/RenderingTarget.h"
#include "../engine/PictoEngine.h"
#include "../stimuli/visualelement.h"

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
class PICTOLIB_API Scene 
#else
class Scene
#endif
{
public:
	Scene();

	void render(QSharedPointer<Engine::PictoEngine> engine);
	double getLatestFirstPhosphorTime();

	void reset();
	void setBackgroundColor(QColor color);
	void addVisualElement(QSharedPointer<VisualElement> element);
	void addAudioElement(QSharedPointer<AudioElement> element);
	static void setZoom(float zoom){zoom_ = zoom;};
	//virtual QString assetType(){return "Scene";};

	//DataStore functions
	//bool serializeAsXml(QSharedPointer<QXmlStreamWriter> xmlStreamWriter);
	//bool deserializeFromXml(QSharedPointer<QXmlStreamReader> xmlStreamReader);
//protected:
//	virtual QString defaultTagName(){return "Scene";};
//	virtual void postDeserialize();
//	virtual bool validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader);


private:
	QColor backgroundColor_;
	QList<QSharedPointer <VisualElement> > visualElements_;
	QList<QSharedPointer <VisualElement> > unaddedVisualElements_;

	QList<QSharedPointer <AudioElement> > audioElements_;
	QList<QSharedPointer <AudioElement> > unaddedAudioElements_;
	
	int frame_;
	QTime elapsedTime_;
	double firstPhosphorTime_;
	static float zoom_;

	//QSharedPointer<Canvas> canvas_;
	//QMap<QString, QSharedPointer<ControlElement> > controlElements_;
};


}; //namespace Picto

#endif
