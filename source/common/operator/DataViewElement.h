#ifndef _DATA_VIEW_ELEMENT_H_
#define _DATA_VIEW_ELEMENT_H_
#include <QImage>
#include <QSharedPointer>
#include <QString>
#include <QMap>
#include <QTime>

#include "../common.h"

#include "../statemachine/ContainerElement.h"
#include "../property/PropertyContainer.h"

namespace Picto {

/*!	\brief A Base class for all Elements representing data that is output to the Observer while viewing an experiment.
 *
 *	\details It is useful to be able to group together these types of elements such that they can be handled in similar
 *	ways.  This way, any element that can define some kind of output to the Observer subject can inherit DataViewElement
 *	and all of its associated logic.
 *	\author Trevor Stavropoulos, Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2015
 */
#if defined WIN32 || defined WINCE
class PICTOLIB_CLASS DataViewElement : public ContainerElement
#else
class DataViewElement : public ContainerElement
#endif
{
	Q_OBJECT
public:
	DataViewElement();
	virtual ~DataViewElement();

	virtual QString friendlyTypeName(){return "Data View Element";};
	virtual QString getUIGroup(){return "Operator Features";};
};

}; //namespace Picto

#endif