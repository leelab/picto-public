#ifndef _PICTUREGRAPHIC_H_
#define _PICTUREGRAPHIC_H_

#include "../common.h"

#include "VisualElement.h"
#include "../property/PropertyContainer.h"
#include "../property/Property.h"

namespace Picto {

/*!	\brief This is ALMOST an obsolete class that does the same thing as ImageFile but in a less logical way.  We can't quite get rid of it yet though because
 *	it is being used to display the BlockDesign splash screen.
 *
 *	\details This class was created long ago as a way to show the BlockDesign splash screen in Picto.  It is very limited though in the fact that it
 *	requires the image that it wants to display to actually be loaded on the hard disk of the computer running Picto.  This means that in order to
 *	use it for images in experimental designs, we would have to make sure to also copy all of the correct image files to the Director computer 
 *	before running the Picto Experiment or else it wouldn't work.  The ImageGraphic class takes care of these issues by embedding the image file code
 *	in the Design file itself.  The ImageGraphic system also separated ImageFileParameter objects and ImageGraphic objects so that a given ImageGraphic
 *	can quickly switch the particular graphic that is being displayed.
 *	
 *	In summary, this class should be removed, but first we will have to set Picto up to use the ImageGraphic for displaying the BlockDesign splash
 *	screen.  This was never a high proirity so it hasn't happened yet.
 *	\author Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2013
 */
class PICTOLIB_CLASS PictureGraphic : public VisualElement
{
	Q_OBJECT
	/*! \brief Sets/Gets the current image file path.*/
	Q_PROPERTY(QString imageFile READ getImageFile WRITE setImageFile)
public:
	PictureGraphic(QPoint position=QPoint(), QString imageFile="");
	static QSharedPointer<Asset> Create();
	void draw();
	static VisualElement* NewVisualElement();

	/*! \brief Gets the current image file path.*/
	QString getImageFile() { return propertyContainer_->getPropertyValue("ImageFile").toString(); };
	/*! \brief Sets the image file path to the input value.*/
	void setImageFile(QString fileName) { propertyContainer_->setPropertyValue("ImageFile",fileName); };

	static const QString type;

	virtual QString friendlyTypeName(){return "Picture";};
protected:
	virtual QString defaultTagName(){return "Picture";};
	virtual void postDeserialize();
	virtual bool validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader);


private slots:
	
};


}; //namespace Picto

#endif
