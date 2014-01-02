#ifndef _TokenFactoryGraphic_H_
#define _TokenFactoryGraphic_H_

#include "../common.h"

#include "VisualElement.h"
#include "../property/PropertyContainer.h"
#include "../property/Property.h"

namespace Picto {

/*!	\brief Displays any number of small token shapes.  The token shapes are configurable to create all sorts of useful symbols for points, 
 *	rewards, quantities or lots of other things.
 *	
 *	\details A Token Factory is a very general class that can be used for clustering lots of graphics to work together.  Individual tokens might
 *	represent quantites or reward, points, timeouts or many other things.
 *	
 *	TokenFactoryGraphics are highly configurable to handle just about any token situation you might want to use.
 *	The number of tokens can be set. Individual token colors and sizes can be set.  Individual tokens can be set to display only their outline.  Tokens 
 *	can have their individual shapes changed and Token positions can be set as well, both globally or with respect to the overall TokenFactoryGraphic position.
 *	This class is very similar to the TokenTrayGraphic.  In fact, the TokenTrayGraphic should really have been written as a special case of this class.
 *	It might be a good idea to reimplement the TokenTrayGraphic that way at some point.  One snag that you might come across though in doing this kind 
 *	of reimplementation is that in the TokenTrayGraphic token property lists, "-1" is used to indicate that a default value should be used for a particular token.  
 *	In this class, "_" is used to avoid problems with x and y positions of -1.
 *
 *	\note Unlike in the case of most other VisualElements, the size of the TokenFactoryGraphic is not set by the designer.  The size of the QImage
 *	required to display all of the tokens is calculated during draw() by getImageDims() to make sure that all tokens appear without being clipped.  This
 *	also means that a TokenFactoryGraphic with a high layer and tokens that are spread out very far could cover other graphics.  We may want to think
 *	of a clean way to deal with this at some point.  In the meantime, TokenFactoryGraphics should be set to have a low layer so that other things will
 *	be in front of them.
 *	\author Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2013
 */
class PICTOLIB_CLASS TokenFactoryGraphic : public VisualElement
{
	Q_OBJECT
		/*! \brief Sets/gets the default token width.*/
	Q_PROPERTY(int tokenWidth READ getTokenWidth WRITE setTokenWidth)
	/*! \brief Sets/gets the default token height.*/
	Q_PROPERTY(int tokenHeight READ getTokenHeight WRITE setTokenHeight)
	/*! \brief Sets/gets the default TokenShape as a string ("Ellipse", "Rectangle", or "Diamond").*/
	Q_PROPERTY(QString tokenShape READ getTokenShape WRITE setTokenShape)
	/*! \brief Sets/gets whether only the outline of the tokens will be shown by default.*/
	Q_PROPERTY(bool outline READ getOutline WRITE setOutline);
	/*! \brief Sets/gets the default thickness of the outline that will be shown if "Outline" is true.*/
	Q_PROPERTY(int outlineWidth READ getOutlineWidth WRITE setOutlineWidth);
	/*! \brief Sets/gets the number of tokens to be displayed.*/
	Q_PROPERTY(int numTokens READ getNumTokens WRITE setNumTokens)

public:
	TokenFactoryGraphic(QPoint position=QPoint(), QRect dimensions=QRect(), QColor color=QColor());

	void draw();
	static VisualElement* NewVisualElement();
	static QSharedPointer<Asset> Create();
	virtual void enteredScope();

	QRect getTokenDimensions();
	void setTokenDimensions(QRect dimensions);
	/*! \brief Gets the default token width.*/
	int getTokenWidth(){return getTokenDimensions().width();};
	/*! \brief Gets the default token height.*/
	int getTokenHeight(){return getTokenDimensions().height();};
	/*! \brief Sets the default token width.*/
	void setTokenWidth(int w){QRect dims = getTokenDimensions(); dims.setWidth(w);setTokenDimensions(dims);};
	/*! \brief Sets the default token height.*/
	void setTokenHeight(int h){QRect dims = getTokenDimensions(); dims.setHeight(h);setTokenDimensions(dims);};
	QString getTokenShape();
	void setTokenShape(QString shape);
	void setNumTokens(int num);
	int getNumTokens();
	bool getOutline();
	void setOutline(bool on);
	int getOutlineWidth();
	void setOutlineWidth(int pixels);
	QPoint getPositionOffset();

	static const QString type;

	virtual QString getUITemplate(){return "TokenFactoryElement";};
	virtual QString friendlyTypeName(){return "Token Factory";};
public slots:
	/*! \brief Sets the default size of tokens.*/
	void setTokenDimensions(int w, int h){setTokenWidth(w);setTokenHeight(h);};
	void setTokenPos(int index,int x,int y);
	void setTokenLocalPos(int index,int x,int y);
	void setTokenX(int index,int x);
	void setTokenY(int index,int y);
	void setTokenLocalX(int index,int x);
	void setTokenLocalY(int index,int y);
	void setTokenSize(int index,double size);
	void setTokenColor(int index, int r, int g, int b, int a=255);
	void setTokenColor(int index,QVariant color);
	void setTokenShape(int index, QString shape);
	void setTokenOutline(int index, bool on);
	void setTokenOutlineWidth(int index, int pixels);
	int getTokenX(int index);
	int getTokenY(int index);
	int getTokenLocalX(int index);
	int getTokenLocalY(int index);
	double getTokenSize(int index);
	int getTokenRed(int index);
	int getTokenGreen(int index);
	int getTokenBlue(int index);
	int getTokenAlpha(int index);
	QColor getTokenColor(int index);
	bool getTokenOutline(int index);
	int getTokenOutlineWidth(int index);
	QString getTokenShape(int index);


protected:
	virtual void postDeserialize();
	virtual bool validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader);
private:
	void updateListSizes();
	QRect getImageDims();
	QPoint posOffset_;
	QStringList shapeList_;
	QHash<int,QString> sizeDelta_;
	QHash<int,QString> colorDelta_;
	QHash<int,QString> shapeDelta_;
	QHash<int,QString> outlineDelta_;
	QHash<int,QString> outlineWidthDelta_;
	QHash<int,QString> xDelta_;
	QHash<int,QString> yDelta_;
	QVector<QHash<int,QString>*> deltaHashs_;
	QStringList defaults_;
	QStringList listNames_;

};


}; //namespace Picto

#endif
