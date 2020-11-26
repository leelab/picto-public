#ifndef _TARGETFACTORY_H_
#define _TARGETFACTORY_H_

#include <QRect>
#include "../common.h"
#include "controltarget.h"

namespace Picto {

#if defined WIN32 || defined WINCE
class PICTOLIB_API TargetFactory : public ControlTarget
#else
class TargetFactory : public ControlTarget
#endif
{
	Q_OBJECT
	/*! \brief Sets/gets the default target width.*/
	Q_PROPERTY(int targetWidth READ getTargetWidth WRITE setTargetWidth)
	/*! \brief Sets/gets the default target height.*/
	Q_PROPERTY(int targetHeight READ getTargetHeight WRITE setTargetHeight)
	/*! \brief Sets/gets the default target height.*/
	Q_PROPERTY(QString targetShape READ getTargetShape WRITE setTargetShape)
	/*! \brief Sets/gets the number of targets in this factory.*/
	Q_PROPERTY(int numTargets READ getNumTargets WRITE setNumTargets)
	/*! \brief Gets the index of the target currently being fixated on/.*/
	Q_PROPERTY(int fixatedIndex READ getFixatedIndex)

public:
	TargetFactory(QPoint position = QPoint(), QRect dimensions = QRect(), QColor color = QColor());
	void draw();
	static QSharedPointer<Asset> Create();

	virtual void enteredScope();
	virtual bool contains(int x, int y);

	QRect getTargetDimensions();
	void setTargetDimensions(QRect dimensions);
	/*! \brief Gets the default token width.*/
	int getTargetWidth(){ return getTargetDimensions().width(); };
	/*! \brief Gets the default token height.*/
	int getTargetHeight(){ return getTargetDimensions().height(); };
	/*! \brief Sets the default token width.*/
	void setTargetWidth(int w){ QRect dims = getTargetDimensions(); dims.setWidth(w); setTargetDimensions(dims); };
	/*! \brief Sets the default token height.*/
	void setTargetHeight(int h){ QRect dims = getTargetDimensions(); dims.setHeight(h); setTargetDimensions(dims); };
	QString getTargetShape();
	void setTargetShape(QString shape);
	int getNumTargets();
	void setNumTargets(int num);
	QPoint getPositionOffset();
	/*! \brief Gets the index of either the current target being fixated on, or the last target successfully fixated on.*/
	int getFixatedIndex(){ return currTargetIndex_; };

	virtual QString getUITemplate(){ return "TargetFactory"; };
	virtual QString friendlyTypeName(){ return "Target Factory"; };
public slots:
	/*! \brief Sets the default size of targets.*/
	void setTargetDimensions(int w, int h){ setTargetWidth(w); setTargetHeight(h); };
	void setTargetPos(int index, int x, int y);
	void setTargetLocalPos(int index, int x, int y);
	void setTargetX(int index, int x);
	void setTargetY(int index, int y);
	void setTargetLocalX(int index, int x);
	void setTargetLocalY(int index, int y);
	void setTargetSize(int index, double size);
	void setTargetColor(int index, int r, int g, int b, int a = 255);
	void setTargetColor(int index, QVariant color);
	void setTargetShape(int index, QString shape);
	void setTargetEnabled(int index, bool enabled);
	int getTargetX(int index);
	int getTargetY(int index);
	int getTargetLocalX(int index);
	int getTargetLocalY(int index);
	double getTargetSize(int index);
	int getTargetRed(int index);
	int getTargetGreen(int index);
	int getTargetBlue(int index);
	int getTargetAlpha(int index);
	QColor getTargetColor(int index);
	QString getTargetShape(int index);
	bool getTargetEnabled(int index);
protected:
	virtual void postDeserialize();
	virtual bool validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader);
private:
	void updateListSizes();
	QRect getImageDims();
	int currTargetIndex_;
	QStringList shapeList_;
	QPoint posOffset_;
	QStringList defaults_;
	QStringList listNames_;
};


}; //namespace Picto

#endif