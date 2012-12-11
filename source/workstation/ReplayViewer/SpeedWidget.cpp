#include <QSlider>
#include <QDoubleSpinBox>
#include <QVBoxLayout>
#include <QLabel>
#include "SpeedWidget.h"
#include "../../common/memleakdetect.h"


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

double SpeedWidget::sliderValToSpeed(int sliderVal)
{
	if(sliderVal >= 0)
		return (sliderVal*resolution_)+1.0;
	return 1+(double(sliderVal)*resolution_/ticksPerResBelow1_);
}
int SpeedWidget::speedToSliderVal(double speed)
{
	if(speed >= 1.0)
		return (speed-1.0)/resolution_;
	return -ticksPerResBelow1_*(1.0-speed)/resolution_;
}

void SpeedWidget::sliderMoved(int newVal)
{
	double newSpeed = sliderValToSpeed(newVal);
	if(speedIndicator_->value() != newSpeed)
	{
		speedIndicator_->setValue(newSpeed);
		spinboxUpdated();
	}
}

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