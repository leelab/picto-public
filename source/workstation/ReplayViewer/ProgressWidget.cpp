#include <QSlider>
#include <QLCDNumber>
#include <QHBoxLayout>
#include <QLabel>
#include "ProgressWidget.h"
#include "../../common/memleakdetect.h"

#define TICKSPERSEC 100
#define MAXSESSIONSECDIGITS 5
#define LCDPRECISION 3
ProgressWidget::ProgressWidget(QWidget *parent) :
QFrame(parent)
{
	progSlider_ = new QSlider(Qt::Horizontal);
	progSlider_->setMinimum(0);
	progSlider_->setSingleStep(1);
	progSlider_->setSliderPosition(0);
	connect(progSlider_,SIGNAL(sliderMoved(int)),this,SLOT(userSetSlider(int)));
	connect(progSlider_,SIGNAL(sliderPressed()),this,SLOT(sliderPressed()));
	connect(progSlider_,SIGNAL(sliderReleased()),this,SLOT(sliderReleased()));

	
	maxProgress_ = new QLabel();

	currProgress_ = new QLCDNumber(MAXSESSIONSECDIGITS+LCDPRECISION);
	currProgress_->setSegmentStyle(QLCDNumber::Flat);

	setMaximum(0);
	
	QHBoxLayout* progLayout = new QHBoxLayout();
	progLayout->addWidget(currProgress_,0,Qt::AlignVCenter);
	progLayout->addWidget(progSlider_,10,Qt::AlignVCenter);
	progLayout->addWidget(maxProgress_,0,Qt::AlignVCenter);

	setFrameStyle(QFrame::Sunken | QFrame::StyledPanel);
	setLayout(progLayout);
}

ProgressWidget::~ProgressWidget()
{
}

void ProgressWidget::setMaximum(double max)
{
	progSlider_->setMaximum(max*TICKSPERSEC);
	maxProgress_->setText(QString::number(max,'f',LCDPRECISION));
	max_ = max;
}

void ProgressWidget::setProgress(double progress)
{
	if(progress < 0)
		progress = 0;
	if(progress > max_)
		progress = max_;
	int sliderPos = progressToSlider(progress);
	if(!progSlider_->isSliderDown())
		progSlider_->setSliderPosition(sliderPos);
	currProgress_->display(QString::number(progress,'f',LCDPRECISION));
}

int ProgressWidget::progressToSlider(double progress)
{
	return progress*TICKSPERSEC;
}

double ProgressWidget::sliderToProgress(int sliderVal)
{
	return double(sliderVal)/TICKSPERSEC;
}

void ProgressWidget::sliderPressed()
{
	emit userSliderOperation(true);
}

void ProgressWidget::sliderReleased()
{
	emit userSliderOperation(false);
	emit jumpToProgress(sliderToProgress(progSlider_->sliderPosition()));
}

void ProgressWidget::userSetSlider(int sliderVal)
{
	//emit jumpToProgress(sliderToProgress(sliderVal));
}