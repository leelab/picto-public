#ifndef _SPEEDWIDGET_H_
#define _SPEEDWIDGET_H_

#include <QFrame>

class QSlider;
class QDoubleSpinBox;
/*!	\brief A Widget used with the ReplayViewer to set the current playback speed.
 * 
 *	\details This widget consists of a Slider bar and a numerical field.  To change
 *	the speed setting, the slider can be dragged or the numerical field can be updated.
 *	Each of these updates the other automatically.  The widget's max and min can be
 *	set along with its resoltion, where the resolution value is the minimum value increment.
 *	The widget is set up so that a speed of 1 (normal playback) is in the center of the 
 *	slider.  The space between the center and the bottom moves playback from 1 to 0.  The 
 *	space between the middle of the slider and the top moves playback from 1 to 100.
 *	The playback speed value is interpreted by the ReplayViewer as a factor by which
 *	playback speed is multiplied. Therefore, a playback speed of 3 means that playback
 *	will occur at 3 times the normal rate. A value of .5 means that playback will occur
 *	at half the normal rate.
 *	\author Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2013
 */
class SpeedWidget : public QFrame
{
	Q_OBJECT
public:
	SpeedWidget(int maxSpeed,double minSpeed,double resolution,double defaultSpeed,QWidget *parent=0);
	virtual ~SpeedWidget();
signals:
	/*! \brief Emitted when the user changes the speed value.  speed parameter is the new speed value.*/
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