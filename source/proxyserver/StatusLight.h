#ifndef STATUSLIGHT_H
#define STATUSLIGHT_H

#include <QWidget>
#include <QColor>

QT_BEGIN_NAMESPACE
class QPainter;
class QSize;
QT_END_NAMESPACE

/*! \brief A status light GUI element
 *
 *	The status light is used in the proxy server's GUI.  It is basically a circle
 *	that changes colors (red, yellow, green) to indicate status.  This was one of
 *	my first attempts at writing a Qt GUI, so it's a bit crude.
 */
class StatusLight : public QWidget
{
	Q_OBJECT
public:
	StatusLight(QWidget *parent = 0, QColor color = Qt::red, int radius=30);
	
	QColor getColor() { return currentColor; };
	void setColor(QColor color) { currentColor = color; update(); };
	void setRadius(int radius) { this->radius=radius; update(); };
	int getRadius() {return radius;};

	QSize sizeHint() const;
	
	void draw(QPainter *painter);

public slots:
	void turnGreen() { currentColor = Qt::green; update(); };
	void turnYellow() { currentColor = Qt::yellow; update(); };
	void turnRed() { currentColor = Qt::red; update(); };

protected:
	void paintEvent(QPaintEvent *event);

private:
	QColor currentColor;
	int radius;
};

#endif