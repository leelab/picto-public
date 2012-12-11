#ifndef _SPEEDWIDGET_H_
#define _SPEEDWIDGET_H_

#include <QFrame>

class QSlider;
class QDoubleSpinBox;
/*!	\brief	Widget for use in Replay system
 * 
 */
class SpeedWidget : public QFrame
{
	Q_OBJECT
public:
	SpeedWidget(int maxSpeed,double minSpeed,double resolution,double defaultSpeed,QWidget *parent=0);
	virtual ~SpeedWidget();
signals:
	void speedChanged(double speed);
private:
	double sliderValToSpeed(int sliderVal);
	int speedToSliderVal(double speed);
	QSlider* speedSlider_;
	QDoubleSpinBox* speedIndicator_;
	double ticksPerResBelow1_;
	double resolution_;
private slots:
	void sliderMoved(int newVal);
	void spinboxUpdated();
};

#endif