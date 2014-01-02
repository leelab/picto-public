#ifndef _TOKENTRAYGRAPHIC_H_
#define _TOKENTRAYGRAPHIC_H_

#include "../common.h"

#include "VisualElement.h"
#include "../property/PropertyContainer.h"
#include "../property/Property.h"

namespace Picto {

/*!	\brief Displays a series of small token shapes laid out equidistantly along a larger tray shape.  The token shapes and tray shape are configurable to create
 *	all sorts of useful symbols for points, rewards, quantities or lots of other things.
 *	
 *	\details Token Trays are very common in Neurobiology experiments.  They are used to represent quantites or reward, points, timeouts and more.
 *	THis class generates TokenTray objects that are highly configurable to handle just about any token tray type situation you might want to use.
 *	The number of tokens can be set. Individual token colors and sizes can be set.  Individual tokens can be set to display only their outline.  The
 *	phase along the token tray at which tokens are counted can be changed.  The tray shape itself can be resized or stretched.  Tokens can even 
 *	have their individual shapes changed.  We have tried to make this class very general to handle all types of situations; however, if you need
 *	even more generality, the TokenFactoryGraphic class gets rid of the tray all together allowing scripts to handle individual token positions.
 *
 *	\note Unlike in the case of most other VisualElements, the Size Property and the various scripts that change the dimension of the token tray don't
 *	set the total size of the graphic.  The Size Property is actually setting the size of the token tray "path" on which the tokens sit.  This is 
 *	important because if we were to use it to set the overall tray size, changing the size of a particular token could actually end up affecting the
 *	positions of the other tokens since the token whose size changed would no longer fit on the token tray path if we were to satisfy the size constraint.  
 *	This would mean the path would need to change in order to stay inside the total Size setting.  To avoid these problems, we just use the Size Property 
 *	to set the size of the Token tray path and then calculate the size of the actual QImage that we need during draw() to make sure that all tokens appear '
 *	without being clipped.
 *	\author Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2013
 */
class PICTOLIB_CLASS TokenTrayGraphic : public VisualElement
{
	Q_OBJECT
	/*! \brief Sets/gets the default width of the individual tokens in the tray.*/
	Q_PROPERTY(int tokenWidth READ getTokenWidth WRITE setTokenWidth)
	/*! \brief Sets/gets the default height of the individual tokens in the tray.*/
	Q_PROPERTY(int tokenHeight READ getTokenHeight WRITE setTokenHeight)
	/*! \brief Sets/gets the default TokenShape as a string ("Ellipse", "Rectangle", or "Diamond").*/
	Q_PROPERTY(QString tokenShape READ getTokenShape WRITE setTokenShape)
	/*! \brief Sets/gets whether only the outline of the individual tokens will be shown by default.*/
	Q_PROPERTY(bool outline READ getOutline WRITE setOutline);
	/*! \brief Sets/gets the default thickness of the outline that will be shown if "Outline" is true.*/
	Q_PROPERTY(int outlineWidth READ getOutlineWidth WRITE setOutlineWidth);
	/*! \brief Sets/gets the number of tokens to be displayed.*/
	Q_PROPERTY(int numTokens READ getNumTokens WRITE setNumTokens)
	/*! \brief Sets/gets the width of the token tray.*/
	Q_PROPERTY(int width READ getTrayWidth WRITE setTrayWidth)
	/*! \brief Sets/gets the height of the token tray.*/
	Q_PROPERTY(int height READ getTrayHeight WRITE setTrayHeight)
	/*! \brief Sets/gets the default shape of the token tray as a string (currently only "Ellipse" is supported).*/
	Q_PROPERTY(QString shape READ getTrayShape WRITE setTrayShape)
	/*! \brief Sets/gets the Phase in degrees at which the tokens start being drawn starting from the right side and in a counter clockwise direction 
	 *	(ie. 0 - 360 where 0 is along the x axis).
	 */
	Q_PROPERTY(int phase READ getPhase WRITE setPhase)

public:
	TokenTrayGraphic(QPoint position=QPoint(), QRect dimensions=QRect(), QColor color=QColor());

	void draw();
	static VisualElement* NewVisualElement();
	static QSharedPointer<Asset> Create();
	virtual void enteredScope();

	QRect getTokenDimensions();
	void setTokenDimensions(QRect dimensions);
	/*! \brief Gets the default width of the individual tokens in the tray.*/
	int getTokenWidth(){return getTokenDimensions().width();};
	/*! \brief Gets the default height of the individual tokens in the tray.*/
	int getTokenHeight(){return getTokenDimensions().height();};
	/*! \brief Sets the default width of the individual tokens in the tray.*/
	void setTokenWidth(int w){QRect dims = getTokenDimensions(); dims.setWidth(w);setTokenDimensions(dims);};
	/*! \brief Sets the default height of the individual tokens in the tray.*/
	void setTokenHeight(int h){QRect dims = getTokenDimensions(); dims.setHeight(h);setTokenDimensions(dims);};
	QString getTokenShape();
	void setTokenShape(QString shape);
	void setNumTokens(int num);
	int getNumTokens();
	QRect getTrayDimensions();
	void setTrayDimensions(QRect dimensions);
	/*! \brief Gets the width of the token tray.*/
	int getTrayWidth(){return getTrayDimensions().width();};
	/*! \brief Gets the height of the token tray.*/
	int getTrayHeight(){return getTrayDimensions().height();};
	/*! \brief Sets the width of the token tray.*/
	void setTrayWidth(int w){QRect dims = getTrayDimensions(); dims.setWidth(w);setTrayDimensions(dims);};
	/*! \brief Sets the height of the token tray.*/
	void setTrayHeight(int h){QRect dims = getTrayDimensions(); dims.setHeight(h);setTrayDimensions(dims);};
	QString getTrayShape();
	void setTrayShape(QString shape);
	int getPhase();
	void setPhase(int phase);
	bool getOutline();
	void setOutline(bool on);
	int getOutlineWidth();
	void setOutlineWidth(int pixels);
	QPoint getPositionOffset();

	static const QString type;
	virtual void upgradeVersion(QString deserializedVersion);

	virtual QString getUITemplate(){return "TokenTrayElement";};
	virtual QString friendlyTypeName(){return "Token Tray";};
public slots:
	/*! \brief Sets the size of the token tray.*/
	void setDimensions(int w, int h){setTrayWidth(w);setTrayHeight(h);};
	/*! \brief Sets the default size of individual tokens.*/
	void setTokenDimensions(int w, int h){setTokenWidth(w);setTokenHeight(h);};
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
	QPoint getTokenPosition(int index);
	QPoint getTokenOffset(int index);
	//void updateParameterLists();
	//bool updatingParameterLists_;
	int getMaxTokenDiam();
	QPoint posOffset_;
	QStringList shapeList_;
	QHash<int,QString> sizeDelta_;
	QHash<int,QString> colorDelta_;
	QHash<int,QString> shapeDelta_;
	QHash<int,QString> outlineDelta_;
	QHash<int,QString> outlineWidthDelta_;
	QVector<QHash<int,QString>*> deltaHashs_;
	QStringList defaults_;
	QStringList listNames_;

};


}; //namespace Picto

#endif
