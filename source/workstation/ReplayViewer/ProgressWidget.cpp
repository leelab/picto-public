#include <QSlider>
#include <QLCDNumber>
#include <QHBoxLayout>
#include <QLabel>
#include <QtAlgorithms>
#include <QStylePainter>
#include <QStyleOptionSlider>
#include "ProgressWidget.h"
#include "../../common/memleakdetect.h"

#define TICKSPERSEC 100
#define MAXSESSIONSECDIGITS 5
#define LCDPRECISION 3
#define HIGHLIGHTSATURATIONRANGE 40
#define HIGHLIGHTBORDERWIDTH 0.005
#define REFRESHINTERVALMS 30
ProgressWidget::ProgressWidget(QWidget *parent) :
QFrame(parent)
{
	progSlider_ = new QSlider(Qt::Horizontal);
	progSlider_->setMinimum(0);
	progSlider_->setSingleStep(1);
	progSlider_->setSliderPosition(0);
	connect(progSlider_,SIGNAL(sliderPressed()),this,SLOT(sliderPressed()));
	connect(progSlider_,SIGNAL(sliderReleased()),this,SLOT(sliderReleased()));
	connect(progSlider_,SIGNAL(valueChanged(int)),this,SLOT(sliderValueChanged(int)));

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
	setStyleSheet(	"QSlider::groove:horizontal { \
							border: 1px solid #bbb; \
							background: #fff; \
							height: 10px; \
							border-radius: 4px; \
							} \
							QSlider::groove:horizontal:disabled { \
							border-color: #999; \
							background: #bbb; \
							} \
							 \
							QSlider::handle:horizontal { \
							background: qlineargradient(x1:0, y1:0, x2:1, y2:1, \
								stop:0 #eee, stop:1 #ccc); \
							border: 1px solid #777; \
							width: 13px; \
							margin-top: -2px; \
							margin-bottom: -2px; \
							border-radius: 4px; \
							} \
							 \
							QSlider::handle:horizontal:hover { \
							background: qlineargradient(x1:0, y1:0, x2:1, y2:1, \
								stop:0 #fff, stop:1 #ddd); \
							border: 1px solid #444; \
							border-radius: 4px; \
							} \
							 \
							QSlider::handle:horizontal:disabled { \
							background: #eee; \
							border: 1px solid #aaa; \
							border-radius: 4px; \
							}"
					);
	frameTimer_.start();
}

ProgressWidget::~ProgressWidget()
{
}

void ProgressWidget::setMaximum(double max)
{
	progSlider_->setMaximum(max*TICKSPERSEC);
	maxProgress_->setText(QString::number(max,'f',LCDPRECISION));
	max_ = max;
	updateHighlights();
}

double ProgressWidget::getHighlightMin(int index)
{
	verifyHighlightIndex(index);
	return highlights_[index].getMin();
}

void ProgressWidget::setHighlightMin(int index,double progress)
{
	verifyHighlightIndex(index);
	if(progress < 0)
		progress = 0;
	if(progress > max_)
		progress = max_;
	if(progress > highlights_[index].getMax())
		highlights_[index].setMax(progress);
	highlights_[index].setMin(progress);
	updateHighlights();
}

double ProgressWidget::getHighlightMax(int index)
{
	verifyHighlightIndex(index);
	return highlights_[index].getMax();
}

void ProgressWidget::setHighlightMax(int index,double progress)
{
	verifyHighlightIndex(index);
	if(progress < 0)
		progress = 0;
	if(progress > max_)
		progress = max_;
	if(progress < highlights_[index].getMin())
		highlights_[index].setMin(progress);
	highlights_[index].setMax(progress);
	updateHighlights();
}

void ProgressWidget::setHighlightRange(int index,double minProgress,double maxProgress)
{
	if(maxProgress < minProgress)
		maxProgress = minProgress;
	setHighlightMin(index,minProgress);
	setHighlightMax(index,maxProgress);
}

QColor ProgressWidget::getHighlightColor(int index)
{
	verifyHighlightIndex(index);
	return highlights_[index].getColor();
}

void ProgressWidget::setHighlightColor(int index, QColor color)
{
	verifyHighlightIndex(index);
	highlights_[index].setColor(color);
	updateHighlights();
}

double ProgressWidget::getSliderProgress()
{
	return sliderToProgress(progSlider_->sliderPosition());
}

void ProgressWidget::setSliderProgress(double progress)
{
	if(progress < 0)
		progress = 0;
	if(progress > max_)
		progress = max_;
	progSlider_->setSliderPosition(progressToSlider(progress));
}

void ProgressWidget::verifyHighlightIndex(int index)
{
	if(!highlights_.contains(index))
	{
		highlights_[index] = HighlightData();
	}
}

void ProgressWidget::updateHighlights()
{
	//To update the highlight values we just change the widget stylesheet.  This takes a lot of processor
	//time but is by far the easiest way to do this.  In order to avoid causing UI lag when this function
	//is called a lot, we just make sure that we only reset the stylesheet one per REFRESHINTERVALMS
	//which should be on the order of 30ms.
	if(frameTimer_.elapsed() < REFRESHINTERVALMS)
		return;
	frameTimer_.restart();
	//Use styles to create highlight bar in widget
	QString styleUpdate = QString(	"QSlider::groove:horizontal { \
										background: #fff, stop: 1 #fff); \
									}");
	if(progressToSlider(max_))
	{	
		//Build list of necessary color stops to include in style string
		QList<HighlightData> highlights = highlights_.values();
		qStableSort<QList<HighlightData>>(highlights);
		QVector<QString> colors;
		QVector<double> pos;
		double lastStopPt = 0.0;
		foreach(HighlightData data,highlights)
		{
			double min = data.getMin()/max_;
			double max = data.getMax()/max_;
			if(max < lastStopPt + 2*HIGHLIGHTBORDERWIDTH)
				continue;
			if(min < lastStopPt+HIGHLIGHTBORDERWIDTH)
				min = lastStopPt+HIGHLIGHTBORDERWIDTH;
			if(min > 1-2*HIGHLIGHTBORDERWIDTH)
				break;
			if(min > lastStopPt+2*HIGHLIGHTBORDERWIDTH)
			{
				colors.append("#fff");
				pos.append(lastStopPt+HIGHLIGHTBORDERWIDTH);
				colors.append("#fff");
				pos.append(min-HIGHLIGHTBORDERWIDTH);
			}
			colors.append(data.getMinColorStr());
			pos.append(min);

			if(max < min + HIGHLIGHTBORDERWIDTH)
				max = min + HIGHLIGHTBORDERWIDTH;
			if(max > 1-HIGHLIGHTBORDERWIDTH)
				max = 1-HIGHLIGHTBORDERWIDTH;
			colors.append(data.getMaxColorStr());
			pos.append(max);
			lastStopPt = max;
		}
		//Add white color stop if last stop isn't near the end of the bar
		if(pos.size() && pos.last() < 1-HIGHLIGHTBORDERWIDTH)
		{
			pos.append(pos.last()+HIGHLIGHTBORDERWIDTH);
			colors.append("#fff");
		}

		//Add color stops to string
		QString stopStr = "";
		for(int i=0;i<pos.size();i++)
		{
			stopStr.append(QString("stop:%1 %2,")
					.arg(pos[i])
					.arg(colors[i]));
		}
		styleUpdate = QString("QSlider::groove:horizontal{background:qlineargradient(x1:0,y1:0.5,x2:1,y2:.5,stop:0 #fff,%1stop:1 #fff);}").arg(stopStr);
	}

	//Apply style to widget
	progSlider_->setStyleSheet(styleUpdate);
	//QStylePainter painter(this);
	//QStyleOptionSlider sliderOp;
	//sliderOp.maximum_ = progressToSlider(max_);
	//sliderOp.maximum_ = progressToSlider(max_);
	//sliderOp.singleStep(1);
	//sliderOp.subControls
	//painter.drawComplexControl(QStyle::CC_Slider,
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
	emit userAction(true);
}

void ProgressWidget::sliderReleased()
{
	emit userAction(false);
	emit valueRequested(getSliderProgress()); 
}

void ProgressWidget::sliderValueChanged(int value)
{
	currProgress_->display(QString::number(sliderToProgress(value),'f',LCDPRECISION));
}







HighlightData::HighlightData()
{
	setMin(0);
	setMax(0);
	setColor(QColor(205,205,255));
}
void HighlightData::setColor(QColor color)
{
	if(color.saturation() < HIGHLIGHTSATURATIONRANGE/2)
		color.setHsv(color.hue(),20,color.value());
	else if(color.saturation() > (255-HIGHLIGHTSATURATIONRANGE/2))
		color.setHsv(color.hue(),(255-HIGHLIGHTSATURATIONRANGE/2),color.value());
	color_ = color;
}

QColor HighlightData::getColor()  const
{
	return color_;
}

QString HighlightData::getMaxColorStr()  const
{
	QColor max;
	max.setHsv(color_.hue(),color_.saturation()+HIGHLIGHTSATURATIONRANGE/2,color_.value());
	return getColorStr(max);
}
QString HighlightData::getMinColorStr()  const
{
	QColor min;
	min.setHsv(color_.hue(),color_.saturation()-HIGHLIGHTSATURATIONRANGE/2,color_.value());
	return getColorStr(min);
}
void HighlightData::setMin(double min)
{
	min_ = min;
}
double HighlightData::getMin()  const
{
	return min_;
}
void HighlightData::setMax(double max)
{
	max_ = max;
}
double HighlightData::getMax()  const
{
	return max_;
}

bool HighlightData::operator<(const HighlightData& rhs)  const
{ 
	if(getMin() == rhs.getMin()) 
		return getMax() < rhs.getMax();
	return getMin() < rhs.getMin(); 
}

QString HighlightData::getColorStr(QColor color)  const
{
	return QString("#%1%2%3").arg(color.red()/16,0,16).arg(color.green()/16,0,16).arg(color.blue()/16,0,16);
}