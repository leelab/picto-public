#ifndef _ImageGraphic_H_
#define _ImageGraphic_H_

#include <QWeakPointer>
#include "../common.h"

#include "VisualElement.h"
#include "../parameter/FileParameter.h"
#include "../property/PropertyContainer.h"
#include "../property/Property.h"

namespace Picto {

/*!	\brief A circle graphic.
 *
 *	A ImageGraphic has the following properties:
 *	- Position: The position of the top-left corner of the box containing the circle (in screen coordinates)
 *	- Radius: Duh.
 *	- Color: The color of the circle
 */

class PICTOLIB_CLASS ImageGraphic : public VisualElement
{
	Q_OBJECT
	Q_PROPERTY(int width READ getWidth WRITE setWidth)
	Q_PROPERTY(int height READ getHeight WRITE setHeight)
	Q_PROPERTY(QString file READ getFile WRITE setFile)
public:
	ImageGraphic();

	void draw();
	static VisualElement* NewVisualElement();
	static QSharedPointer<Asset> Create();
	static const QString type;

	QRect getDimensions();
	void setDimensions(QRect dimensions);
	int getWidth(){return getDimensions().width();};
	int getHeight(){return getDimensions().height();};
	void setWidth(int w){QRect dims = getDimensions(); dims.setWidth(w);setDimensions(dims);};
	void setHeight(int h){QRect dims = getDimensions(); dims.setHeight(h);setDimensions(dims);};
	QString getFile();
	void setFile(QString fileObjectName);
	QPoint getPositionOffset();

	virtual QString friendlyTypeName(){return "Image";};
	virtual QString getUITemplate(){return "ImageElement";};

public slots:
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
