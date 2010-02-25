#ifndef _VISUALELEMENT_H_
#define _VISUALELEMENT_H_

#include "../common.h"

#include "StimulusElement.h"
#include "../storage/DataStore.h"
#include "../compositor/CompositingSurface.h"
#include "../random/MersenneTwister.h"
#include "../property/PropertyContainer.h"

#include <QImage>
#include <QSharedPointer>
#include <QString>
#include <QMap>
#include <QTime>

namespace Picto {

struct PICTOLIB_CLASS VisualElement : public StimulusElement, public DataStore
{
public:
	VisualElement();
	~VisualElement();

	void addCompositingSurface(QString surfaceType, QSharedPointer<CompositingSurface> compositingSurface);
	QSharedPointer<CompositingSurface> getCompositingSurface(QString surfaceType);

	virtual void draw() = 0;
	virtual void updateAnimation(int frame, QTime elapsedTime);
	void setRandomNumberGeneratorState(MTRand::uint32 * rng);
	MTRand::uint32 * getRandomNumberGeneratorState();

	QPoint getPosition();
	QRect getBoundingRect();

	void setPosition(QPoint position);

	void setOrder(int order) { order_ = order; };
	int getOrder() { return order_; }

	bool serializeAsXml(QSharedPointer<QXmlStreamWriter> xmlStreamWriter);
	bool deserializeFromXml(QSharedPointer<QXmlStreamReader> xmlStreamReader);


protected:
	//This virtual functions handle the deserialization of
	//each element's properties.  The serialization functios needs to write all 
	//of .  Likewise, The deserialization function needs to read the content 
	//between the <VisualElement type="blah"> and </VisualElement>tags and
	//modify itself to match the content of the XML.  This is done 
	//at the subclass level to simplify the code.
	virtual bool deserializePropertiesFromXML(QSharedPointer<QXmlStreamReader> xmlStreamReader)=0;

	QImage image_;
	bool shouldUpdateCompositingSurfaces_;
	QMap<QString, QSharedPointer<CompositingSurface> > compositingSurfaces_;
	
	void updateCompositingSurfaces();

	MTRand random;

	PropertyContainer propertyContainer_;

	int order_;		//The order in which this visual element should be drawn within its layer
};

}; //namespace Picto

#endif
