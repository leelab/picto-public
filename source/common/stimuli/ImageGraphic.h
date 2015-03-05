#ifndef _ImageGraphic_H_
#define _ImageGraphic_H_

#include <QWeakPointer>
#include "../common.h"

#include "VisualElement.h"
#include "../parameter/FileParameter.h"
#include "../property/PropertyContainer.h"
#include "../property/Property.h"

namespace Picto {

/*!	\brief A VisualElement that displays an Image File.
 *	\details Unlike other VisualElements that create images "on the fly" based on their Properties, this one
 *	simply displays an image from a file.  This is very useful for the purpose of generality so that even if 
 *	there is no way to produce an image automatically, it can always be designed in other tools and then added 
 *	to the Picto design.
 *	
 *	Objects of this class include an ImageFile Property that is used to reference an ImageFileParameter which includes the actual
 *	image file that will be displayed.  This indirection allows us to switch the ImageFile associated with an ImageGraphic
 *	during the course of an experiment.
 *
 *	\author Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2013
 */
class PICTOLIB_CLASS ImageGraphic : public VisualElement
{
	Q_OBJECT
	/*! \brief Sets/gets the width of this graphic in pixels.*/
	Q_PROPERTY(int width READ getWidth WRITE setWidth)
	/*! \brief Sets/gets the height of this graphic in pixels.*/
	Q_PROPERTY(int height READ getHeight WRITE setHeight)
	/*! \brief Sets/gets the name of the ImageFIle element from which this ImageGraphic will draw its image.*/
	Q_PROPERTY(QString file READ getFile WRITE setFile)
public:
	ImageGraphic();

	void draw();
	static QSharedPointer<Asset> Create();
	static const QString type;

	QRect getDimensions();
	void setDimensions(QRect dimensions);
	/*! \brief Returns the width of this graphic in pixels.*/
	int getWidth(){return getDimensions().width();};
	/*! \brief Returns the height of this graphic in pixels.*/
	int getHeight(){return getDimensions().height();};
	/*! \brief Sets the width of this graphic in pixels.*/
	void setWidth(int w){QRect dims = getDimensions(); dims.setWidth(w);setDimensions(dims);};
	/*! \brief Sets the height of this graphic in pixels.*/
	void setHeight(int h){QRect dims = getDimensions(); dims.setHeight(h);setDimensions(dims);};
	QString getFile();
	void setFile(QString fileObjectName);
	QPoint getPositionOffset();

	virtual QString friendlyTypeName(){return "Image";};
	virtual QString getUITemplate(){return "ImageElement";};

public slots:
	/*! \brief Set the dimensions (width, height) of this graphic.*/
	void setDimensions(int w, int h){setWidth(w);setHeight(h);};

protected:
	virtual void postDeserialize();
	virtual bool validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader);
	virtual void scriptableContainerWasReinitialized();
	virtual bool executeSearchAlgorithm(SearchRequest searchRequest);
private:
	void updateFileObject();
	QPoint posOffset_;
	QWeakPointer<FileParameter> fileObj_;
private slots:
	
};


}; //namespace Picto

#endif
