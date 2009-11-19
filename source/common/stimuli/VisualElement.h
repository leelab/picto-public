#ifndef _VISUALELEMENT_H_
#define _VISUALELEMENT_H_

#include "../common.h"

#include "StimulusElement.h"
#include "../compositor/CompositingSurface.h"
#include "../random/MersenneTwister.h"

#include <QImage>
#include <QSharedPointer>
#include <QString>
#include <QMap>

namespace Picto {

struct PICTOLIB_CLASS VisualElement : public StimulusElement
{
public:
	VisualElement();

	void addCompositingSurface(QString surfaceType, QSharedPointer<CompositingSurface> compositingSurface);

	virtual void draw() = 0;
	void setRandomNumberGeneratorState(MTRand::uint32 * rng);
	MTRand::uint32 * getRandomNumberGeneratorState();

protected:
	QImage image_;
	bool shouldRedrawImage_;
	QMap<QString, QSharedPointer<CompositingSurface> > compositingSurfaces_;
	
	void updateCompositingSurfaces();

	MTRand random;
};


}; //namespace Picto

#endif
