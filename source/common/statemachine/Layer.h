/*! \brief A Layer contains a set of VisualElements. */
#ifndef _LAYER_H_
#define _LAYER_H_

#include <QSharedPointer>
#include <QTime>

#include "../common.h"
#include "../storage/DataStore.h"
#include "../stimuli/VisualElement.h"
#include "../compositor/VisualTarget.h"

namespace Picto {

#if defined WIN32 || defined WINCE
class PICTOLIB_API Layer : public DataStore
#else
class Layer : public DataStore
#endif
{
public:
	Layer();

	void addVisualElement( QSharedPointer<VisualElement> v);
	void setOrder(int order) { order_ = order; };
	int getOrder() { return order_; };
	void setName(QString name) { name_ = name; };

	void draw(QSharedPointer<VisualTarget> visualTarget);


	//DataStore functions
	bool serializeAsXml(QSharedPointer<QXmlStreamWriter> xmlStreamWriter);
	bool deserializeFromXml(QSharedPointer<QXmlStreamReader> xmlStreamReader);
private:
	QList<QSharedPointer <VisualElement> > visualElements_;
	int order_;
	QString name_;

	int frame_;
	QTime elapsedTime_;

};


}; //namespace Picto

#endif
