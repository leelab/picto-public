#ifndef STATUSLIGHT_H
#define STATUSLIGHT_H

#include <QWidget>
#include <QColor>

QT_BEGIN_NAMESPACE
class QPainter;
class QSize;
QT_END_NAMESPACE

/*! \brief A status light widget
 *
 *	\details We couldn't find any good stock status light widgets so we made our own.
 *	The status light is used in the proxy server's GUI.  It is just a circle that changes colors 
 *	(red, yellow, green) to indicate status.  This was an early attempt at writing a Qt GUI, 
 *	so it's a bit crude.
 *	\author Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2013
 */
class StatusLight : public QWidget
{
	Q_OBJECT
public:
	StatusLight(QWidget *parent = 0, QColor color = Qt::red, int radius=30);
	
	/*! \brief Returns the color of the StatusLight.*/
	QColor getColor() { return currentColor; };
	/*! \brief Sets the color of the StatusLight to the input value.*/
	void setColor(QColor color) { currentColor = color; update(); };
	/*! \brief Sets the radius of the StatusLight to the input value.*/
	void setRadius(int radius) { this->radius=radius; update(); };
	/*! \brief Returns the radius of the StatusLight.*/
	int getRadius() {return radius;};

	QSize sizeHint() const;
	
	void draw(QPainter *painter);

public slots:
	/*! \brief Changes the color of the StatusLight to green.*/
	void turnGreen() { currentColor = Qt::green; update(); };
	/*! \brief Changes the color of the StatusLight to yellow.*/
	void turnYellow() { currentColor = Qt::yellow; update(); };
	/*! \brief Changes the color of the StatusLight to red.*/
	void turnRed() { currentColor = Qt::red; update(); };

protected:
	void paintEvent(QPaintEvent *event);

private:
	QColor currentColor;
	int radius;
};

#endif