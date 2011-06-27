#ifndef _CANVAS_H_
#define _CANVAS_H_

#include "../common.h"
#include "../statemachine/UIEnabled.h"
#include "Layer.h"
#include "../engine/PictoEngine.h"

namespace Picto {

/*!	\brief A Canvas contains the layers of a scene
 *
 *	The layers are stored in a list, with the exception of the
 *	background layer, which is defined as a single opaque color
 */

#if defined WIN32 || defined WINCE
class PICTOLIB_API Canvas : public UIEnabled
#else
class Canvas : public UIEnabled
#endif
{
	Q_OBJECT
public:
	Canvas();
	static QSharedPointer<Asset> Create(){return QSharedPointer<Asset>(new Canvas());};

	void bindToScriptEngine(QScriptEngine& qsEngine);

	void setBackgroundColor(QColor color) { backgroundColor_ = color; };

	void addLayer(QSharedPointer<Layer> layer);

	void draw(QSharedPointer<Engine::PictoEngine> engine);

	void reset();
	virtual QString assetType(){return "Canvas";};

	//DataStore functions
	//bool serializeAsXml(QSharedPointer<QXmlStreamWriter> xmlStreamWriter);
	//bool deserializeFromXml(QSharedPointer<QXmlStreamReader> xmlStreamReader);
protected:
	virtual QString defaultTagName(){return "Canvas";};
	virtual void postSerialize();
	virtual bool validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader);

private:
	QList<QSharedPointer <Layer> > layers_;
	QColor backgroundColor_;
};


}; //namespace Picto

#endif
