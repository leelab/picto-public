#ifndef _SCENE_H_
#define _SCENE_H_

#include <QObject>
#include <QXmlStreamWriter>
#include <QXmlStreamReader>
#include <QMutex>

#include "../common.h"
#include "../stimuli/AudioElement.h"
#include "../controlelements/ControlElement.h"
#include "../compositor/RenderingTarget.h"
#include "../engine/PictoEngine.h"
#include "../stimuli/visualelement.h"
#include "../stimuli/outputsignal.h"

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
	class PICTOLIB_API Scene : public QObject
#else
class Scene : public QObject
#endif
{
	Q_OBJECT
public:

	static QSharedPointer<Scene> createScene();

	void render(QSharedPointer<Engine::PictoEngine> engine,int callerId);
	void reset();
	void setBackgroundColor(QColor color);
	void addVisualElement(QSharedPointer<VisualElement> element);
	void addAudioElement(QSharedPointer<AudioElement> element);
	void addOutputSignal(QSharedPointer<OutputSignal> element);
	static void setZoom(float zoom);

signals:
	void readyForRender(int callerId);


private:
	Scene();
	QSharedPointer<QMutex> mutex_;
	QSharedPointer<Engine::PictoEngine> engine_;
	QColor backgroundColor_;
	QList<QSharedPointer <VisualElement> > visualElements_;
	QList<QSharedPointer <VisualElement> > unaddedVisualElements_;

	QList<QSharedPointer <AudioElement> > audioElements_;
	QList<QSharedPointer <AudioElement> > unaddedAudioElements_;

	QList<QSharedPointer <OutputSignal> > outputSignals_;
	QList<QSharedPointer <OutputSignal> > unaddedOutputSignals_;
	
	int frame_;
	QTime elapsedTime_;
	double firstPhosphorTime_;
	bool readyToRender_;
	static float zoom_;
	static QMutex staticMutex_;

private slots:
	void doRender(int callerId);
};


}; //namespace Picto

#endif
