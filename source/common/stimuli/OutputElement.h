#ifndef _OUTPUTELEMENT_H_
#define _OUTPUTELEMENT_H_
#include <QImage>
#include <QSharedPointer>
#include <QString>
#include <QMap>
#include <QTime>

#include "../common.h"

#include "StimulusElement.h"
#include "../statemachine/scriptablecontainer.h"
#include "../compositor/CompositingSurface.h"
#include "../random/MersenneTwister.h"
#include "../property/PropertyContainer.h"

namespace Picto {

/*!	\brief A Base class for all Output Elements
 *
 *	Currently these include Visual Elements and Output Signals
 */
#if defined WIN32 || defined WINCE
class PICTOLIB_CLASS OutputElement : public ScriptableContainer
#else
class ScriptableContainer : public ScriptableContainer
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
