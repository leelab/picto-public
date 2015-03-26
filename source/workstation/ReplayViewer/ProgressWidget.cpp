#include <QSlider>
#include <QLCDNumber>
#include <QHBoxLayout>
#include <QLabel>
#include <QtAlgorithms>
#include <QStylePainter>
#include <QStyleOptionSlider>
#include "ProgressWidget.h"
#include "../../common/memleakdetect.h"

/*! \brief The number of QSlider ticks that will be used for every unit of "progress".*/
#define TICKSPERSEC 100
#define MAXSESSIONSECDIGITS 5
#define LCDPRECISION 3
#define HIGHLIGHTSATURATIONRANGE 40
/*! \brief The border width of the highlights.*/
#define HIGHLIGHTBORDERWIDTH 0.005
/*! \brief The frequency at which highlights will be redrawn.*/
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

/*! \brief Sets the maximum value of the progress bar.*/
void ProgressWidget::setMaximum(double max)
{
	progSlider_->setMaximum(max*TICKSPERSEC);
	maxProgress_->setText(QString::number(max,'f',LCDPRECISION));
	bool needsHUpdate = max != max_;
	max_ = max;
	if(needsHUpdate)
		updateHighlights(true);
}

/*! \brief Gets the maximum value of the progress bar.*/
double ProgressWidget::getMaximum()
{
	return max_;
}

/*! \brief Returns the minimum progress of the highlight bar at the input index.*/
double ProgressWidget::getHighlightMin(int index)
{
	verifyHighlightIndex(index);
	return highlights_[index].getMin();
}

/*! \brief Sets the minimum progress of the highlight bar at the input index.
 *	@param index The index of the highlight
 *	@param progress The minimum progress value
 *	@param forceUpdate Since updating of highlights is a fairly slow process, we only
 *	update highlights once per REFRESHINTERVALMS, not whenever this is called.  
 *	To force updating immediately when this is called, set forceUpdate to true.
 */
void ProgressWidget::setHighlightMin(int index,double progress,bool forceUpdate)
{
	verifyHighlightIndex(index);
	if(progress < 0)
		progress = 0;
	if(progress > max_)
		progress = max_;
	if(progress > highlights_[index].getMax())
		highlights_[index].setMax(progress);
	highlights_[index].setMin(progress);
	updateHighlights(forceUpdate);
}

/*! \brief Returns the maximum progress of the highlight bar at the input index.*/
double ProgressWidget::getHighlightMax(int index)
{
	verifyHighlightIndex(index);
	return highlights_[index].getMax();
}

/*! \brief Sets the maximum progress of the highlight bar at the input index.
 *	@param index The index of the highlight
 *	@param progress The maximum progress value
 *	@param forceUpdate Since updating of highlights is a fairly slow process, we only
 *	update highlights once per REFRESHINTERVALMS, not whenever this is called.  
 *	To force updating immediately when this is called, set forceUpdate to true.
 */
void ProgressWidget::setHighlightMax(int index,double progress,bool forceUpdate)
{
	verifyHighlightIndex(index);
	if(progress < 0)
		progress = 0;
	if(progress > max_)
		progress = max_;
	if(progress < highlights_[index].getMin())
		highlights_[index].setMin(progress);
	highlights_[index].setMax(progress);
	updateHighlights(forceUpdate);
}

/*! \brief Sets the range of progresses that will be highlighted with the highlight bar at the input index.
 *	@param index The index of the highlight
 *	@param minProgress The minimum progress value
  *	@param maxProgress The maximum progress value
 *	@param forceUpdate Since updating of highlights is a fairly slow process, we only
 *	update highlights once per REFRESHINTERVALMS, not whenever this is called.  
 *	To force updating immediately when this is called, set forceUpdate to true.
 */
void ProgressWidget::setHighlightRange(int index,double minProgress,double maxProgress,bool forceUpdate)
{
	if(maxProgress < minProgress)
		maxProgress = minProgress;
	setHighlightMin(index,minProgress);
	setHighlightMax(index,maxProgress,forceUpdate);
}

/*! \brief Returns the color of the highlight at the input index.*/
QColor ProgressWidget::getHighlightColor(int index)
{
	verifyHighlightIndex(index);
	return highlights_[index].getColor();
}

/*! \brief Sets the color of the highlight at the input index.
 *	@param index The index of the highlight
 *	@param color The color that the highlight should take.
 *	@param forceUpdate Since updating of highlights is a fairly slow process, we only
 *	update highlights once per REFRESHINTERVALMS, not whenever this is called.  
 *	To force updating immediately when this is called, set forceUpdate to true.
 */
void ProgressWidget::setHighlightColor(int index, QColor color,bool forceUpdate)
{
	verifyHighlightIndex(index);
	highlights_[index].setColor(color);
	updateHighlights(forceUpdate);
}

/*! \brief Returns the current progress value of the slider.*/
double ProgressWidget::getSliderProgress()
{
	return sliderToProgress(progSlider_->sliderPosition());
}

/*! \brief Sets the current progress value of the slider.*/
void ProgressWidget::setSliderProgress(double progress)
{
	if(progress < 0)
		progress = 0;
	if(progress > max_)
		progress = max_;
	progSlider_->setSliderPosition(progressToSlider(progress));
}

/*! \brief Jumps the slider progress value to the end of the progress bar.*/
void ProgressWidget::jumpToEnd()
{
	sliderPressed();
	setSliderProgress(max_+1);
	sliderReleased();
}

/*! \brief Makes sure that the highlights_ Hash has an entry at the input index.
 *	\details It adds an entry if there isn't one there.
 *	\note This seems like a kind of roundabout way to do something simple.  It might
 *	be worth looking into this.
*/
void ProgressWidget::verifyHighlightIndex(int index)
{
	if(!highlights_.contains(index))
	{
		highlights_[index] = HighlightData();
	}
}

/*! \brief Redraws all highlight bars in the highlights_ hash.
 *	\details To conserve processor time, this function only operates once per
 *	REFRESHINTERVALMS unless forceUpdate is set to true.  When forceUpdate is
 *	true this function always draws the highlight.
 */
void ProgressWidget::updateHighlights(bool forceUpdate)
{
	//To update the highlight values we just change the widget stylesheet.  This takes a lot of processor
	//time but is by far the easiest way to do this.  In order to avoid causing UI lag when this function
	//is called a lot, we just make sure that we only reset the stylesheet one per REFRESHINTERVALMS
	//which should be on the order of 30ms.
	if(!forceUpdate && (frameTimer_.elapsed() < REFRESHINTERVALMS))
		return;
	frameTimer_.restart();
	//Use styles to create highlight bar in widget
	QString styleUpdate = QString("QSlider::groove:horizontal {background: #fff, stop:1 #fff;}");
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

/*! \brief Converts the input value from "progress" units to "tick" units.
 *	\details Since we want to be able to place the slider at non integer points along
 *	the progress bar, we use a TICKSPERSEC define to multiply progress, max and min values
 *	into units that the Progress bar can understand.
*/
int ProgressWidget::progressToSlider(double progress)
{
	return progress*TICKSPERSEC;
}

/*! \brief Converts the input value from "slider" units to "progres" units.
 *	\details Since we want to be able to place the slider at non integer points along
 *	the progress bar, we use a TICKSPERSEC define to multiply progress, max and min values
 *	into units that the Progress bar can understand.
*/
double ProgressWidget::sliderToProgress(int sliderVal)
{
	return double(sliderVal)/TICKSPERSEC;
}

/*! \brief Called whenever the user presses down on the slider.  Causes a userAction() signal to be
 *	emitted with "true" input.
 */
void ProgressWidget::sliderPressed()
{
	emit userAction(true);
}

/*! \brief Called whenever the user releases the slider.  Causes a userAction() signal to be
 *	emitted with "false" input along with a valueRequested() signal with the current slider
 *	progress value.
 *	\details This is how moving the slider signifies that ouside world that the user wants 
 *	to move to a different progress position.
 */
void ProgressWidget::sliderReleased()
{
	emit userAction(false);
	emit valueRequested(getSliderProgress()); 
}

/*! \brief Called whenever the slider values changes.  Updates the QLCDNumber with the latest
 *	progress value.
 */
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
/*! \brief Sets the color of the highlight.*/
void HighlightData::setColor(QColor color)
{
	if(color.saturation() < HIGHLIGHTSATURATIONRANGE/2)
		color.setHsv(color.hue(),20,color.value());
	else if(color.saturation() > (255-HIGHLIGHTSATURATIONRANGE/2))
		color.setHsv(color.hue(),(255-HIGHLIGHTSATURATIONRANGE/2),color.value());
	color_ = color;
}

/*! \brief Gets the color of the highlight.*/
QColor HighlightData::getColor()  const
{
	return color_;
}

/*! \brief Gets a string in \#rrggbb format describing a brighter version of this Highlight's color.*/
QString HighlightData::getMaxColorStr()  const
{
	QColor max;
	max.setHsv(color_.hue(),color_.saturation()+HIGHLIGHTSATURATIONRANGE/2,color_.value());
	return getColorStr(max);
}

/*! \brief Gets a string in \#rrggbb format describing a darker version of this Highlight's color.*/
QString HighlightData::getMinColorStr()  const
{
	QColor min;
	min.setHsv(color_.hue(),color_.saturation()-HIGHLIGHTSATURATIONRANGE/2,color_.value());
	return getColorStr(min);
}

/*! \brief Sets the minimum progress value at which this highllight will appear (the bottom of the highlighted
 *	bar).
 */
void HighlightData::setMin(double min)
{
	min_ = min;
}
/*! \brief Gets the minimum progress value at which this highllight will appear (the bottom of the highlighted
 *	bar).
 */
double HighlightData::getMin()  const
{
	return min_;
}

/*! \brief Sets the maximum progress value at which this highllight will appear (the top of the highlighted
 *	bar).
 */
void HighlightData::setMax(double max)
{
	max_ = max;
}

/*! \brief Gets the maximum progress value at which this highllight will appear (the top of the highlighted
 *	bar).
 */
double HighlightData::getMax()  const
{
	return max_;
}

/*! \brief Returns true if this highlight starts at a lower progress than the input highlight.*/
bool HighlightData::operator<(const HighlightData& rhs)  const
{ 
	if(getMin() == rhs.getMin()) 
		return getMax() < rhs.getMax();
	return getMin() < rhs.getMin(); 
}

/*! \brief Gets a string in #rrggbb format describing the input color.*/
QString HighlightData::getColorStr(QColor color)  const
{
	return QString("#%1%2%3").arg(color.red()/16,0,16).arg(color.green()/16,0,16).arg(color.blue()/16,0,16);
}