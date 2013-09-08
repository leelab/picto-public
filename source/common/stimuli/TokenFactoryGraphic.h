#ifndef _TokenFactoryGraphic_H_
#define _TokenFactoryGraphic_H_

#include "../common.h"

#include "VisualElement.h"
#include "../property/PropertyContainer.h"
#include "../property/Property.h"

namespace Picto {

	/*!	\brief A Token Factory Graphic (spawns tokens).
 *
 */

class PICTOLIB_CLASS TokenFactoryGraphic : public VisualElement
{
	Q_OBJECT
	Q_PROPERTY(int tokenWidth READ getTokenWidth WRITE setTokenWidth)
	Q_PROPERTY(int tokenHeight READ getTokenHeight WRITE setTokenHeight)
	Q_PROPERTY(QString tokenShape READ getTokenShape WRITE setTokenShape)
	Q_PROPERTY(bool outline READ getOutline() WRITE setOutline);
	Q_PROPERTY(int outlineWidth READ getOutlineWidth() WRITE setOutlineWidth);
	Q_PROPERTY(int numTokens READ getNumTokens WRITE setNumTokens)

public:
	TokenFactoryGraphic(QPoint position=QPoint(), QRect dimensions=QRect(), QColor color=QColor());

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
	bool getOutline();
	void setOutline(bool on);
	int getOutlineWidth();
	void setOutlineWidth(int pixels);
	QPoint getPositionOffset();

	static const QString type;

	virtual QString getUITemplate(){return "TokenFactoryElement";};
	virtual QString friendlyTypeName(){return "Token Factory";};
public slots:
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
	//QPoint getTokenPosition(int index);
	//QPoint getTokenOffset(int index);
	//void updateParameterLists();
	//bool updatingParameterLists_;
	//int getMaxTokenDiam();
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
