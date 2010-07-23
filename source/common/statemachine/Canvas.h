/*!	\brief A Canvas contains the layers of a scene
 *
 *	The layers are stored in a list, with the exception of the
 *	background layer, which is defined as a single opaque color
 */
#ifndef _CANVAS_H_
#define _CANVAS_H_

#include "../common.h"
#include "../storage/DataStore.h"
#include "Layer.h"
#include "../engine/PictoEngine.h"

namespace Picto {

#if defined WIN32 || defined WINCE
class PICTOLIB_API Canvas : public DataStore
#else
class Canvas : public DataStore
#endif
{
public:
	Canvas();

	void bindToScriptEngine(QSharedPointer<QScriptEngine> qsEngine);

	void setBackgroundColor(QColor color) { backgroundColor_ = color; };

	void addLayer(QSharedPointer<Layer> layer);

	void draw(QSharedPointer<Engine::PictoEngine> engine);

	void reset();

	//DataStore functions
	bool serializeAsXml(QSharedPointer<QXmlStreamWriter> xmlStreamWriter);
	bool deserializeFromXml(QSharedPointer<QXmlStreamReader> xmlStreamReader);
private:
	QList<QSharedPointer <Layer> > layers_;
	QColor backgroundColor_;
};


}; //namespace Picto

#endif
