#ifndef _COMPOSITINGSURFACE_H_
#define _COMPOSITINGSURFACE_H_

#include <QImage>

#include "../common.h"

namespace Picto {

/*!	\brief A base class for compositing surfaces
 *
 *	All compositing surfaces will inherit from this base class, and will need to 
 *	implement the pure virutal functions
 */
struct PICTOLIB_CLASS CompositingSurface
{
public:
	CompositingSurface();
	virtual ~CompositingSurface();

	/*! \brief Converts the passed in QImage to whatever internal format we need
	 *
	 *	Each compositing surface will need to implement this function so that when called
	 *	the passed in QImage gets read stored in whatever internal format is appropriate.
	 */
	virtual void convertImage(QImage image) = 0;

	//! \brief Returns the name of the compositing surface (e.g. "Direct3D")
	virtual QString getTypeName() = 0;
};


}; //namespace Picto

#endif
