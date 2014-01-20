#ifndef _COMPOSITINGSURFACE_H_
#define _COMPOSITINGSURFACE_H_

#include <QImage>

#include "../common.h"

namespace Picto {

/*!	\brief A base class that represents an area with a drawing on it.
 *	\details QImages are converted and drawn onto CompositingSurface objects.  Multiple
 *	CompositingSurface objects are in turn drawn onto a VisualTarget which is what is finally
 *	rendered on screen.  In essence, a CompositingSurface can be thought of as an image object
 *	that is drawn in a language that the VisualTarget understands, or looked at another way, a
 *	compositing surface is a translater, translating from the language of the QImage to the
 *	language of the VisualTarget.
 *	\author Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2013
 */
struct PICTOLIB_CLASS CompositingSurface
{
public:
	CompositingSurface();
	virtual ~CompositingSurface();

	/*! \brief Converts the passed in QImage to whatever internal format the VisualTarget that
	 *	created this CompositeSurface is going to need.
	 */
	virtual void convertImage(QImage image) = 0;

	//! \brief Returns the name of the compositing surface (e.g. "Direct3D")
	virtual QString getTypeName() = 0;

	/*! \brief Sets this CompositingSurface as non-scalable.
	 * \details This is useful for graphics that are overlayed on top of the display such as the
	 * user position cursor.  When zooming into the current display, we want all of the displayed
	 * graphics to be zoomed, but the cursor and other overlayed graphics should stay the same size.
	 */
	void dontAllowScaling(){ scalable_ = false;};
	/*! \brief Returns true if the compositing surface is scalable, and false otherwise.
	 *	\sa dontAllowScaling()
	 */
	bool scalable(){return scalable_;};
private:
	bool scalable_;

};

}; //namespace Picto

#endif
