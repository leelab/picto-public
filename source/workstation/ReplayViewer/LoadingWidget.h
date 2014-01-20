#ifndef _LOADINGWIDGET_H_
#define _LOADINGWIDGET_H_

#include <QVBoxLayout>
 
#include <QLabel>
#include <QMovie>
 
/*!	\brief NOT USED.  This is no longer used and should probably be deleted.
 *	\author Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2013
 */
class LoadingWidget : public QWidget {
	Q_OBJECT
public:
	LoadingWidget(QString animationPath,
	                QWidget* parent);
	LoadingWidget(QString animationPath,
	                QSize size,
	                QWidget* parent);
	virtual ~LoadingWidget();
 
public slots:
	void start();
	void stop();
 
private:
	QPointer<QLabel> _container;
	QPointer<QMovie> _animation;
 
	void init(const QString& animationPath,
	          const QSize& size);
};

#endif