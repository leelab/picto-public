#ifndef _LOADINGWIDGET_H_
#define _LOADINGWIDGET_H_

#include <QWidget>

class QLabel;
/*!	\brief	Widget for use in Replay system
 * 
 */
class LoadingWidget : public QWidget
{
	Q_OBJECT
public:
	LoadingWidget(QWidget *parent=0);
	virtual ~LoadingWidget();
};

#endif