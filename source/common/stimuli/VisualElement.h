#ifndef _VISUALELEMENT_H_
#define _VISUALELEMENT_H_

#include "../common.h"

#include "StimulusElement.h"
#include "../statemachine/scriptable.h"
#include "../compositor/CompositingSurface.h"
#include "../random/MersenneTwister.h"
#include "../property/PropertyContainer.h"

#include <QImage>
#include <QSharedPointer>
#include <QString>
#include <QMap>
#include <QTime>

namespace Picto {

/*!	\brief A Base class for all Visual Elements
 *
 *	This base class handles all of the StimulusElements that are visual in 
 *	nature.  In particular, it provides the XML serialization code.  Since all
 *	visual elements use properties to describe themselves, we can serialize
 *	them by generating a list of their properties.  This class also handles
 *	the compositing surfaces.
 */
struct PICTOLIB_CLASS VisualElement : /*public StimulusElement,*/ public Scriptable
{
	Q_OBJECT
public:
	VisualElement(QPoint position=QPoint(), QColor color=QColor());
	~VisualElement();

	void addCompositingSurface(QString surfaceType, QSharedPointer<CompositingSurface> compositingSurface);
	QSharedPointer<CompositingSurface> getCompositingSurface(QString surfaceType);

	virtual void draw() = 0;
	virtual void updateAnimation(int frame, QTime elapsedTime);
	void setRandomNumberGeneratorState(MTRand::uint32 * rng);
	MTRand::uint32 * getRandomNumberGeneratorState();

	void reset();

	QPoint getPosition();
	QRect getBoundingRect();
	void setPosition(QPoint position);

	QColor getColor();
	void setColor(QColor color);

	void setOrder(int order) { propertyContainer_->setPropertyValue("Order",order); };
	int getOrder() { return propertyContainer_->getPropertyValue("Order").toInt(); }
	virtual QString assetType(){return "VisualElement";};

	//bool serializeAsXml(QSharedPointer<QXmlStreamWriter> xmlStreamWriter);
	//bool deserializeFromXml(QSharedPointer<QXmlStreamReader> xmlStreamReader);

//These public slots exist for binding visual element properties to scripts
public slots:
	int getX() { return getPosition().x(); };
	int getY() { return getPosition().y(); };
	void setPosition(int x, int y) { setPosition(QPoint(x,y)); };
	void setX(int x){setPosition(x,getY());};
	void setY(int y){setPosition(getX(),y);};

	int getAlpha() { return getColor().alpha(); };
	int getRed() { return getColor().red(); };
	int getBlue() { return getColor().blue(); };
	int getGreen() { return getColor().green(); };
	void setColor(int r, int g, int b, int a=255) { setColor(QColor(r,g,b,a)); };
	void setVisible(bool visible) { visible_ = visible; };
	virtual bool getVisible(){return visible_;};

protected:
	void backupProperties();
	void restoreProperties();

	virtual void postSerialize();
	virtual bool validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader);

	QImage image_;
	bool shouldUpdateCompositingSurfaces_;
	QMap<QString, QSharedPointer<CompositingSurface> > compositingSurfaces_;
	
	void updateCompositingSurfaces();

	MTRand random;

	QMap<QString, QVariant> initialProperties_;
	bool visible_;

private slots:
	virtual void slotPropertyValueChanged(QString propertyName, int index, QVariant propertyValue);
};

}; //namespace Picto

#endif
