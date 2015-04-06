#ifndef _OUTPUTELEMENT_H_
#define _OUTPUTELEMENT_H_
#include <QImage>
#include <QSharedPointer>
#include <QString>
#include <QMap>
#include <QTime>

#include "../common.h"

#include "../statemachine/ContainerElement.h"
#include "../compositor/CompositingSurface.h"
#include "../random/MersenneTwister.h"
#include "../property/PropertyContainer.h"

namespace Picto {

/*!	\brief A Base class for all Elements representing data that is output to the Experiment subject during frame rendering.
 *
 *	\details It is useful to be able to group together these types of elements such that they can be handled in similar
 *	ways.  This way, any element that can define some kind of output to the Experiment subject can inherit
 *	OutputElementContainer and all of its associated logic.  Currently, VisualElement objects, AudioElement objects, and
 *	OutputSignal objects (for digital voltage output) are considered OutputElements.  
 *	\author Trevor Stavropoulos, Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2015
 */
#if defined WIN32 || defined WINCE
class PICTOLIB_CLASS OutputElement : public ContainerElement
#else
class OutputElement : public ContainerElement
#endif
{
	Q_OBJECT
public:
	OutputElement(){};
	virtual ~OutputElement(){};

	virtual QString friendlyTypeName(){return "Output Element";};
	virtual QString getUIGroup(){return "Stimulus Elements";};
};

}; //namespace Picto

#endif
