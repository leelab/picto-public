#ifndef _PAUSEPOINT_H_
#define _PAUSEPOINT_H_

#include "../common.h"
#include "OutputElementContainer.h"
#include "../engine/PictoEngine.h"
#include "scene.h"

namespace Picto {

/*!	\brief A PausePoint is the place where execution is suspended when pause is pressed
 */

#if defined WIN32 || defined WINCE
class PICTOLIB_API PausePoint : public OutputElementContainer
#else
class PausePoint : public OutputElementContainer
#endif
{
	Q_OBJECT
public:
	PausePoint();
	virtual ~PausePoint(){};
	static QSharedPointer<Asset> Create();

	virtual void enableRunMode(bool enable);
	QString run(QSharedPointer<Engine::PictoEngine> engine);
	QString slaveRun(QSharedPointer<Engine::PictoEngine> engine);
	QString slaveRenderFrame(QSharedPointer<Engine::PictoEngine> engine);

	virtual QString getUITemplate(){return "PausePoint";};
	virtual QString friendlyTypeName(){return "Pause Point";};

	virtual void upgradeVersion(QString deserializedVersion);

	virtual void setDesignConfig(QSharedPointer<DesignConfig> designConfig);

	QColor getColor(){return propertyContainer_->getPropertyValue("BackgroundColor").value<QColor>();};
	void setColor(QColor color){propertyContainer_->setPropertyValue("BackgroundColor",color);};
	int getRed() { return getColor().red(); };
	int getGreen() { return getColor().green(); };
	int getBlue() { return getColor().blue(); };
	int getAlpha() { return getColor().alpha(); };
	void setRed(int r){QColor val = getColor(); val.setRed(r);setColor(val);};
	void setGreen(int g){QColor val = getColor(); val.setGreen(g);setColor(val);};
	void setBlue(int b){QColor val = getColor(); val.setBlue(b);setColor(val);};
	void setAlpha(int a){QColor val = getColor(); val.setAlpha(a);setColor(val);};

protected:
	virtual void postDeserialize();
	virtual bool validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader);

private:
	void rebuildScene();
	QSharedPointer<Scene> scene_;
	QSharedPointer<SignalChannel> sigChannel_;
	bool hasCursor_;
private slots:
	void activeAnalysisIdsChanged();
};


}; //namespace Picto

#endif
