#ifndef _LAYER_H_
#define _LAYER_H_

#include <QSharedPointer>
#include <QTime>

#include "../common.h"
#include "../storage/DataStore.h"
#include "../stimuli/VisualElement.h"
#include "../compositor/VisualTarget.h"
#include "../engine/PictoEngine.h"

namespace Picto {

/*! \brief A Layer contains a set of VisualElements. 
 *
 *	Layers are rendered in order from smallest to largest.  Each Layer contains multiple
 *	VisualElements which make up the layer.
 */
#if defined WIN32 || defined WINCE
class PICTOLIB_API Layer : public DataStore
#else
class Layer : public DataStore
#endif
{
	Q_OBJECT
public:
	Layer();

	static QSharedPointer<Asset> Create(){return QSharedPointer<Asset>(new Layer());};
	void bindToScriptEngine(QSharedPointer<QScriptEngine> qsEngine);

	void reset();

	void addVisualElement(QSharedPointer<VisualElement> v);
	void setOrder(int order) { order_ = order; };
	int getOrder() { return order_; };
	void setName(QString name) { name_ = name; };

	void draw(QSharedPointer<VisualTarget> visualTarget, QSharedPointer<Engine::PictoEngine> engine);
	virtual QString assetType(){return "Layer";};


	//DataStore functions
	//bool serializeAsXml(QSharedPointer<QXmlStreamWriter> xmlStreamWriter);
	//bool deserializeFromXml(QSharedPointer<QXmlStreamReader> xmlStreamReader);

protected:
	virtual QString defaultTagName(){return "Layer";};
	virtual bool validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader);

private:
	QList<QSharedPointer <VisualElement> > visualElements_;
	QList<QSharedPointer <VisualElement> > unaddedVisualElements_;
	int order_;
	QString name_;

	int frame_;
	QTime elapsedTime_;

};


}; //namespace Picto

#endif
