#include <QSlider>
#include <QDoubleSpinBox>
#include <QVBoxLayout>
#include <QLabel>
#include "SpeedWidget.h"
#include "../../common/memleakdetect.h"

/*! \brief Constructs a SpeedWidget
 *	@param maxSpeed The maximum allowed speed
 *	@param minSpeed The minimum allowed speed
 *	@param resolution The resolution with which the widget can return values.  This must be less than 1 and
 *		greater than zero.  Ex.  0.1, 0.01, 0.5
 *	@param defaultSpeed The speed setting that the widget uses to start.
 *	@param parent This Widget's parent Widget.
 */
SpeedWidget::SpeedWidget(int maxSpeed,double minSpeed,double resolution,double defaultSpeed,QWidget *parent) :
QFrame(parent)
{
	Q_ASSERT(minSpeed > 0 && minSpeed < 1.0);
	Q_ASSERT(maxSpeed > minSpeed && maxSpeed > 1.0);
	Q_ASSERT(resolution < 1.0 && resolution > 0.0);
	if(minSpeed < 0)
		minSpeed = 0;
	if(maxSpeed < minSpeed)
		maxSpeed = minSpeed;
	resolution_ = resolution;

	int stepsAbove1 = (maxSpeed-1.0)/resolution;
	ticksPerResBelow1_ = stepsAbove1/((1.0-minSpeed)/resolution);

	speedSlider_ = new QSlider(Qt::Vertical);
	speedSlider_->setMinimum(-stepsAbove1);
	speedSlider_->setMaximum(stepsAbove1);
	speedSlider_->setSingleStep(1);
	speedSlider_->setTickInterval(stepsAbove1);
	speedSlider_->setTickPosition(QSlider::TicksBothSides);
	speedIndicator_ = new QDoubleSpinBox();
	speedIndicator_->setButtonSymbols(QAbstractSpinBox::NoButtons);
	connect(speedSlider_,SIGNAL(sliderMoved(int)),this,SLOT(sliderMoved(int)));	
	connect(speedIndicator_,SIGNAL(editingFinished()),this,SLOT(spinboxUpdated()));
	speedIndicator_->setValue(defaultSpeed);
	speedIndicator_->setMinimum(minSpeed);
	speedIndicator_->setMaximum(maxSpeed);

	
	QVBoxLayout* speedLayout = new QVBoxLayout();
	speedLayout->addWidget(new QLabel("Speed"),0,Qt::AlignHCenter);
	speedLayout->addWidget(speedSlider_,0,Qt::AlignHCenter);
	speedLayout->addWidget(speedIndicator_,10,Qt::AlignHCenter);

	setFrameStyle(QFrame::Sunken | QFrame::StyledPanel);
	setLayout(speedLayout);
}

SpeedWidget::~SpeedWidget()
{
}
/*! \brief Converts from slider units to speed units.
 *	\details The slider operates in integer units, but this widget needs to return
 *	floating point units, so we perform a conversion whenever moving between the two.
 */
double SpeedWidget::sliderValToSpeed(int sliderVal)
{
	if(sliderVal >= 0)
		return (sliderVal*resolution_)+1.0;
	return 1+(double(sliderVal)*resolution_/ticksPerResBelow1_);
}

/*! \brief Converts from speed units to slider units.
 *	\details The slider operates in integer units, but this widget needs to return
 *	floating point units, so we perform a conversion whenever moving between the two.
 */
int SpeedWidget::speedToSliderVal(double speed)
{
	if(speed >= 1.0)
		return (speed-1.0)/resolution_;
	return -ticksPerResBelow1_*(1.0-speed)/resolution_;
}

/*! \brief Called when the slider moves with its new value as an input.  Causes the speed
 *	indicator field to be updated.
 */
void SpeedWidget::sliderMoved(int newVal)
{
	double newSpeed = sliderValToSpeed(newVal);
	if(speedIndicator_->value() != newSpeed)
	{
		speedIndicator_->setValue(newSpeed);
		spinboxUpdated();
	}
}

/*! \brief Called when the speed indicator value changes.  Causes the slider position to be updated
 *	and emits a speedChanged() event.
 */
void SpeedWidget::spinboxUpdated()
{
	double newVal = speedIndicator_->value();
	int sliderVal = speedToSliderVal(newVal);
	if(speedSlider_->value() != sliderVal)
	{
		speedSlider_->setSliderPosition(speedToSliderVal(newVal));
	}
	emit speedChanged(newVal);
}