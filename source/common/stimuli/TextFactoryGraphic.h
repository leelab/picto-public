#ifndef _TextFactoryGraphic_H_
#define _TextFactoryGraphic_H_

#include "../common.h"

#include "VisualElement.h"
#include "../property/PropertyContainer.h"
#include "../property/Property.h"

namespace Picto {

/*!	\brief Displays any number of text items.  
 *	\author Vered Zafrany, Trevor Stavropoulos, Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2017
 */
class PICTOLIB_CLASS TextFactoryGraphic : public VisualElement
{
	Q_OBJECT
	/*! \brief Sets/gets the width of the graphic window for the text in pixels.*/
	Q_PROPERTY(int textwidth READ getTextWidth WRITE setTextWidth)
	/*! \brief Sets/gets the height of the graphic window for the text in pixels.*/
	Q_PROPERTY(int textheight READ getTextHeight WRITE setTextHeight)
	/*! \brief Sets/gets the text that is displayed in the graphic.*/
	Q_PROPERTY(QString textcontent READ getTextContent WRITE setTextContent)
public:
	TextFactoryGraphic(QPoint position = QPoint(), QRect dimensions = QRect(), QColor color = QColor(), QString text = QString());
	
	void draw();
	static QSharedPointer<Asset> Create();
	static const QString type;

	/*! \brief Returns a QRect with the dimensions (width, height) of this graphic.
	*	\note The returned value contains no position information.  Only the Width, Height
	*	values should be used.
	*/
	QRect getDimensions(){ return QRect(QPoint(), propertyContainer_->getPropertyValue("Size").toSize()); };
	/*! \brief Set the dimensions (width, height) of this graphic.
	*	\note Only the width and height of the input QRect are used.  x,y position values will be ignored.
	*/
	void setDimensions(QRect dimensions){ propertyContainer_->setPropertyValue("Size", dimensions.size()); };

	/*! \brief Returns the width of the graphic window for the text in pixels.*/
	int getTextWidth(){ return getDimensions().width(); };
	/*! \brief Returns the height of the graphic window for the text in pixels.*/
	int getTextHeight(){ return getDimensions().height(); };
	/*! \brief Sets the width of the graphic window for the text in pixels.*/
	void setTextWidth(int w){ QRect dims = getDimensions(); dims.setWidth(w); setDimensions(dims); };
	/*! \brief Sets the height of the graphic window for the text in pixels.*/
	void setTextHeight(int h){ QRect dims = getDimensions(); dims.setHeight(h); setDimensions(dims); };
	/*! \brief Gets the text that is displayed in the graphic.*/
	QString getTextContent() { return propertyContainer_->getPropertyValue("Text").toString(); };
	/*! \brief Sets the text that is displayed in the graphic.*/
	void setTextContent(QString text) { propertyContainer_->setPropertyValue("Text", text); };

	public slots:
	/*! \brief Set the dimensions (width, height) of the graphic window for the text in this graphic.*/
	void setDimensions(int w, int h){ setTextWidth(w); setTextHeight(h); };

protected: 
	virtual QString getUITemplate(){ return "TextFactoryElement"; };
	virtual QString friendlyTypeName(){ return "Text Factory"; };

	virtual void postDeserialize();
	virtual bool validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader);

private:	
	QPoint posOffset_;
};


}; //namespace Picto

#endif
