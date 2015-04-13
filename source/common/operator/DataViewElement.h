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
#include "DataViewSpecs.h"

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

	/*! \brief Abstract method for getting the title of the DataViewElement.
	 */
	virtual const QString getTitle() const = 0;
	/*! \brief Abstract method for setting the title of the DataViewElement.
	 */
	virtual void setTitle(const QString &newTitle) = 0;

	/*! \brief Abstract method used to prepare drawn elements.
	 */
	virtual void draw() = 0;

	/*! \brief Abstract method used to prepare the DataViewElement for a new run.
	 */
	virtual void reset() = 0;

	virtual QString friendlyTypeName(){return "Data View Element";};
	virtual QString getUIGroup(){return "Operator Features";};

	//! Get the default view size for this element
	DataViewSize::ViewSize getDefaultViewSize() const;

protected:
	virtual void postDeserialize();
	//! A List used to keep track of the possible default sizes.
	QStringList sizeList_;
};

}; //namespace Picto

#endif
