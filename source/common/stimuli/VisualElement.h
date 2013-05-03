#ifndef _VISUALELEMENT_H_
#define _VISUALELEMENT_H_
#include <QImage>
#include <QSharedPointer>
#include <QString>
#include <QMap>
#include <QTime>

#include "../common.h"

#include "StimulusElement.h"
#include "OutputElement.h"
#include "../statemachine/scriptable.h"
#include "../compositor/CompositingSurface.h"
#include "../random/MersenneTwister.h"
#include "../property/PropertyContainer.h"

namespace Picto {

/*!	\brief A Base class for all Visual Elements
 *
 *	This base class handles all of the StimulusElements that are visual in 
 *	nature.  In particular, it provides the XML serialization code.  Since all
 *	visual elements use properties to describe themselves, we can serialize
 *	them by generating a list of their properties.  This class also handles
 *	the compositing surfaces.
 */
struct PICTOLIB_CLASS VisualElement : public OutputElement
{
	Q_OBJECT
	Q_PROPERTY(int layer READ getLayer WRITE setLayer)
	Q_PROPERTY(bool visible READ getVisible WRITE setVisible)
	Q_PROPERTY(bool operatorView READ getOperatorView WRITE setOperatorView)
	Q_PROPERTY(bool subjectView READ getSubjectView WRITE setSubjectView)
	Q_PROPERTY(int x READ getX WRITE setX)
	Q_PROPERTY(int y READ getY WRITE setY)
	Q_PROPERTY(int red READ getRed WRITE setRed)
	Q_PROPERTY(int green READ getGreen WRITE setGreen)
	Q_PROPERTY(int blue READ getBlue WRITE setBlue)
	Q_PROPERTY(int alpha READ getAlpha WRITE setAlpha)
public:
	VisualElement(QPoint position=QPoint(), QColor color=QColor());
	virtual ~VisualElement();

	void addCompositingSurface(QString surfaceType, QSharedPointer<CompositingSurface> compositingSurface);
	QSharedPointer<CompositingSurface> getCompositingSurface(QString surfaceType);

	virtual void draw() = 0;
	virtual void updateAnimation(int frame, QTime elapsedTime);
	void setRandomNumberGeneratorState(MTRand::uint32 * rng);
	MTRand::uint32 * getRandomNumberGeneratorState();

	QPoint getPosition();
	//This should return the offset from the top left visual element corner to the visual element center
	//for visual elements that need to be drawn with their center's at the element position.
	virtual QPoint getPositionOffset(){return QPoint(0,0);};
	QRect getBoundingRect();
	void setPosition(QPoint position);

	QColor getColor();
	void setColor(QColor color);

	void setLayer(int layer) { propertyContainer_->setPropertyValue("Layer",layer); };
	int getLayer() { return propertyContainer_->getPropertyValue("Layer").toInt(); }
	virtual void setVisible(bool visible){propertyContainer_->setPropertyValue("Visible",visible);};
	virtual bool getVisible(){return propertyContainer_->getPropertyValue("Visible").toBool();};
	bool getVisibleByUser(bool subject);
	virtual void setOperatorView(bool visible){propertyContainer_->setPropertyValue("OperatorView",visible);};
	virtual bool getOperatorView(){return propertyContainer_->getPropertyValue("OperatorView").toBool();};
	virtual void setSubjectView(bool visible){propertyContainer_->setPropertyValue("SubjectView",visible);};
	virtual bool getSubjectView(){return propertyContainer_->getPropertyValue("SubjectView").toBool();};
	int getX(){return getPosition().x();};
	void setX(int x){return setPosition(QPoint(x,getPosition().y()));};
	int getY(){return getPosition().y();};
	void setY(int y){return setPosition(QPoint(getPosition().x(),y));};
	virtual QString getUITemplate(){return "VisualElement";};
	virtual QString friendlyTypeName(){return "Visual Element";};
	int getRed() { return getColor().red(); };
	int getGreen() { return getColor().green(); };
	int getBlue() { return getColor().blue(); };
	int getAlpha() { return getColor().alpha(); };
	void setRed(int r){QColor val = getColor(); val.setRed(r);setColor(val);};
	void setGreen(int g){QColor val = getColor(); val.setGreen(g);setColor(val);};
	void setBlue(int b){QColor val = getColor(); val.setBlue(b);setColor(val);};
	void setAlpha(int a){QColor val = getColor(); val.setAlpha(a);setColor(val);};

	void setScalable(bool scalable);
	bool scalable(){return scalable_;};

	virtual void upgradeVersion(QString deserializedVersion);

public slots:
	void setColor(int r, int g, int b, int a=255){setColor(QColor(r,g,b,a));};
	void setPos(int x, int y){setPosition(QPoint(x,y));};

	//bool serializeAsXml(QSharedPointer<QXmlStreamWriter> xmlStreamWriter);
	//bool deserializeFromXml(QSharedPointer<QXmlStreamReader> xmlStreamReader);

protected:

	virtual void postDeserialize();
	virtual bool validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader);

	QImage image_;
	bool shouldUpdateCompositingSurfaces_;
	QMap<QString, QSharedPointer<CompositingSurface> > compositingSurfaces_;
	
	void updateCompositingSurfaces();

	//MTRand random;

	//QMap<QString, QVariant> initialProperties_;
	//bool visible_;
private:
	bool scalable_;
private slots:
	virtual void slotPropertyValueChanged(Property* prop,QVariant);
};

}; //namespace Picto

#endif
