/*! \brief A Layer contains a set of VisualElements. */
#ifndef _LAYER_H_
#define _LAYER_H_

#include <QSharedPointer>

#include "../common.h"
#include "../storage/DataStore.h"
#include "../stimuli/VisualElement.h"

namespace Picto {

#if defined WIN32 || defined WINCE
class PICTOLIB_API Layer : public DataStore
#else
class Layer : public DataStore
#endif
{
public:
	Layer();

	void addVisualElement( QSharedPointer<VisualElement> v) { visualElements_.push_back(v); };
	void setOrder(int order) { order_ = order; };
	void setName(QString name) { name_ = name; };


	//DataStore functions
	bool serializeAsXml(QSharedPointer<QXmlStreamWriter> xmlStreamWriter);
	bool deserializeFromXml(QSharedPointer<QXmlStreamReader> xmlStreamReader);
private:
	QList<QSharedPointer <VisualElement> > visualElements_;
	int order_;
	QString name_;

};


}; //namespace Picto

#endif
