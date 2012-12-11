#ifndef _PROGRESSWIDGET_H_
#define _PROGRESSWIDGET_H_

#include <QFrame>

class QSlider;
class QLCDNumber;
class QLabel;
/*!	\brief	Widget for use in Replay system
 * 
 */
class ProgressWidget : public QFrame
{
	Q_OBJECT
public:
	ProgressWidget(QWidget *parent=0);
	virtual ~ProgressWidget();
	void setMaximum(double max);

signals:
	void userSliderOperation(bool mouseDown);
	void jumpToProgress(double progress);
public slots:
	void setProgress(double progress);

private:
	int progressToSlider(double progress);
	double sliderToProgress(int sliderVal);
	QSlider* progSlider_;
	QLCDNumber* currProgress_;
	QLabel* maxProgress_;
	double ticksPerUnitProg_;
	double max_;
private slots:
	void sliderPressed();
	void sliderReleased();
	void userSetSlider(int sliderVal);
};

#endif