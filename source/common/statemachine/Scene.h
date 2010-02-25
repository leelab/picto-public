#ifndef _SCENE_H_
#define _SCENE_H_

#include <QXmlStreamWriter>
#include <QXmlStreamREader>

#include "../common.h"
#include "../storage/DataStore.h"
#include "Canvas.h"

namespace Picto {

#if defined WIN32 || defined WINCE
class PICTOLIB_API Scene : public DataStore
#else
class Scene : public DataStore
#endif
{
public:
	Scene();

	void setCanvas(QSharedPointer<Canvas> canvas) { canvas_ = canvas; }
	//void setAudioElement(....)

	//DataStore functions
	bool serializeAsXml(QSharedPointer<QXmlStreamWriter> xmlStreamWriter);
	bool deserializeFromXml(QSharedPointer<QXmlStreamReader> xmlStreamReader);

private:
	QSharedPointer<Canvas> canvas_;
};


}; //namespace Picto

#endif
