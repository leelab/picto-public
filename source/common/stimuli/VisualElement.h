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
#include <QScriptEngine>

namespace Picto {

struct PICTOLIB_CLASS VisualElement : public StimulusElement, public DataStore
{
	Q_OBJECT
public:
	VisualElement();
	~VisualElement();

	void addCompositingSurface(QString surfaceType, QSharedPointer<CompositingSurface> compositingSurface);
	QSharedPointer<CompositingSurface> getCompositingSurface(QString surfaceType);

	virtual void draw() = 0;
	virtual void updateAnimation(int frame, QTime elapsedTime);
	void setRandomNumberGeneratorState(MTRand::uint32 * rng);
	MTRand::uint32 * getRandomNumberGeneratorState();

	void reset();

	void bindToScriptEngine(QSharedPointer<QScriptEngine> engine);

	QPoint getPosition();
	QRect getBoundingRect();
	void setPosition(QPoint position);

	QColor getColor();
	void setColor(QColor color);

	QString getName();
	void setName(QString name);

	void setOrder(int order) { order_ = order; };
	int getOrder() { return order_; }

	bool serializeAsXml(QSharedPointer<QXmlStreamWriter> xmlStreamWriter);
	bool deserializeFromXml(QSharedPointer<QXmlStreamReader> xmlStreamReader);

//These public slots exist for binding visual element properties to scripts
public slots:
	int getX() { return getPosition().x(); };
	int getY() { return getPosition().y(); };
	void setPosition(int x, int y) { setPosition(QPoint(x,y)); };

	int getAlpha() { return getColor().alpha(); };
	int getRed() { return getColor().red(); };
	int getBlue() { return getColor().blue(); };
	int getGreen() { return getColor().green(); };
	void setColor(int r, int g, int b, int a=255) { setColor(QColor(r,g,b,a)); };

protected:
	void backupProperties();
	void restoreProperties();

	QImage image_;
	bool shouldUpdateCompositingSurfaces_;
	QMap<QString, QSharedPointer<CompositingSurface> > compositingSurfaces_;
	
	void updateCompositingSurfaces();

	MTRand random;

	PropertyContainer propertyContainer_;
	QMap<QString, QVariant> initialProperties_;

	int order_;		//The order in which this visual element should be drawn within its layer
};

}; //namespace Picto

#endif
