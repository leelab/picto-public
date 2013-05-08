#ifndef _TOKENTRAYGRAPHIC_H_
#define _TOKENTRAYGRAPHIC_H_

#include "../common.h"

#include "VisualElement.h"
#include "../property/PropertyContainer.h"
#include "../property/Property.h"

namespace Picto {

	/*!	\brief A box graphic (a rectangle).
 *
 *	A TokenTrayGraphic has the following properties:
 *	- Position: the position of the middle corner of the box (in screen coordinates)
 *	- Dimensions: the size of the rectangle
 *	- Color: The color of the rectangle
 */

class PICTOLIB_CLASS TokenTrayGraphic : public VisualElement
{
	Q_OBJECT
	Q_PROPERTY(int tokenWidth READ getTokenWidth WRITE setTokenWidth)
	Q_PROPERTY(int tokenHeight READ getTokenHeight WRITE setTokenHeight)
	Q_PROPERTY(QString tokenShape READ getTokenShape WRITE setTokenShape)
	Q_PROPERTY(bool outline READ getOutline() WRITE setOutline);
	Q_PROPERTY(int outlineWidth READ getOutlineWidth() WRITE setOutlineWidth);
	Q_PROPERTY(int numTokens READ getNumTokens WRITE setNumTokens)
	Q_PROPERTY(int width READ getTrayWidth WRITE setTrayWidth)
	Q_PROPERTY(int height READ getTrayHeight WRITE setTrayHeight)
	Q_PROPERTY(QString shape READ getTrayShape WRITE setTrayShape)
	Q_PROPERTY(int phase READ getPhase WRITE setPhase)

public:
	TokenTrayGraphic(QPoint position=QPoint(), QRect dimensions=QRect(), QColor color=QColor());

	//virtual void updateAnimation(int frame, QTime elapsedTime);
	void draw();
	static VisualElement* NewVisualElement();
	static QSharedPointer<Asset> Create();
	virtual void reset();

	QRect getTokenDimensions();
	void setTokenDimensions(QRect dimensions);
	int getTokenWidth(){return getTokenDimensions().width();};
	int getTokenHeight(){return getTokenDimensions().height();};
	void setTokenWidth(int w){QRect dims = getTokenDimensions(); dims.setWidth(w);setTokenDimensions(dims);};
	void setTokenHeight(int h){QRect dims = getTokenDimensions(); dims.setHeight(h);setTokenDimensions(dims);};
	QString getTokenShape();
	void setTokenShape(QString shape);
	void setNumTokens(int num);
	int getNumTokens();
	QRect getTrayDimensions();
	void setTrayDimensions(QRect dimensions);
	int getTrayWidth(){return getTrayDimensions().width();};
	int getTrayHeight(){return getTrayDimensions().height();};
	void setTrayWidth(int w){QRect dims = getTrayDimensions(); dims.setWidth(w);setTrayDimensions(dims);};
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

	virtual QString friendlyTypeName(){return "Token Tray";};
public slots:
	void setDimensions(int w, int h){setTrayWidth(w);setTrayHeight(h);};
	void setTokenDimensions(int w, int h){setTokenWidth(w);setTokenHeight(h);};
	void setTokenSize(int index,double size);
	void setTokenColor(int index, int r, int g, int b, int a=255);
	void setTokenColor(int index,QVariant color);
	void setTokenShape(int index, QString shape);
	void setTokenOutline(int index, bool on);
	void setTokenOutlineWidth(int index, int pixels);
	int getTokenX(int index);
	int getTokenY(int index);
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
