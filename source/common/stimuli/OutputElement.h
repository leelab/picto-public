#ifndef _OUTPUTELEMENT_H_
#define _OUTPUTELEMENT_H_
#include <QImage>
#include <QSharedPointer>
#include <QString>
#include <QMap>
#include <QTime>

#include "../common.h"

#include "StimulusElement.h"
#include "../statemachine/scriptable.h"
#include "../compositor/CompositingSurface.h"
#include "../random/MersenneTwister.h"
#include "../property/PropertyContainer.h"

namespace Picto {

/*!	\brief A Base class for all Output Elements
 *
 *	Currently these include Visual Elements and Output Signals
 */
#if defined WIN32 || defined WINCE
class PICTOLIB_CLASS OutputElement : public Scriptable
#else
class ScriptableContainer : public Scriptable, protected QScriptable
#endif
{
	Q_OBJECT
public:
	OutputElement(){};
	virtual ~OutputElement(){};
};

}; //namespace Picto

#endif