#ifndef _PROGRESSWIDGET_H_
#define _PROGRESSWIDGET_H_

#include <QFrame>
#include <QColor>
#include <QHash>
#include <QTime>

class QSlider;
class QLCDNumber;
class QLabel;
class HighlightData;
/*!	\brief	A Widget that represents some kind of progress along with a slider that can be moved to
 *	request changes in the current progress position (like the youtube time bar).
 *	\details This widget includes a progress bar with a slider on it and an LCD display on the end.
 *	The units that are used to refer to the position of the slider bar are "progress" units.  The
 *	bars max and min progresses can be set, and then the current progress can be set, causing the 
 *	slider to move along the bar accordingly.  The current slider progress is always displayed on
 *	the LCD display.
 *
 *	This ProgressWidget also supports a highlighting system.  Multiple highlight bars can be configured
 *	to appear along the progress bar at particular positions.  It is useful to have these separate means
 *	of displaying a progress value along the same bar as the slider.  The slider can represent a current
 *	selected time and highlights can represent the times that are loaded (again, like youtube).
 *	\author Trevor Stavropoulos, Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2015
 */
class ProgressWidget : public QFrame
{
	Q_OBJECT
public:
	ProgressWidget(QWidget *parent=0);
	virtual ~ProgressWidget();
	void setMaximum(double max);
	double getMaximum();
	double getHighlightMin(int index);
	void setHighlightMin(int index,double progress,bool forceUpdate = false);
	double getHighlightMax(int index);
	void setHighlightMax(int index,double progress,bool forceUpdate = false);
	void setHighlightRange(int index,double minProgress,double maxProgress,bool forceUpdate = false);
	QColor getHighlightColor(int index);
	void setHighlightColor(int index, QColor color,bool forceUpdate = false);
	double getSliderProgress();
	void setSliderProgress(double progress);
	void jumpToEnd();

signals:
	/*! \brief Emitted when the user presses or releases the slider.  mouseDown indicates if a press or release is happening.
	*/
	void userAction(bool mouseDown);
	/*! \brief Emitted when the user uses the slider to request that the system using the slider
	 *	move to a new "progress".
	 */
	void valueRequested(double progress);

private:
	void verifyHighlightIndex(int index);
	void updateHighlights(bool forceUpdate = false);
	int progressToSlider(double progress);
	double sliderToProgress(int sliderVal);
	QSlider* progSlider_;
	QLCDNumber* currProgress_;
	QLabel* maxProgress_;
	double ticksPerUnitProg_;
	double max_;
	QHash<int,HighlightData> highlights_;
	QTime frameTimer_;
private slots:
	void sliderPressed();
	void sliderReleased();
	void sliderValueChanged(int value);
};

class HighlightData
{
public:
	HighlightData();
	void setColor(QColor color);
	QColor getColor()  const;
	QString getMaxColorStr()  const;
	QString getMinColorStr()  const;
	void setMin(double min);
	double getMin() const;
	void setMax(double max);
	double getMax()  const;

	bool operator<(const HighlightData& rhs)  const;


private:
	QString getColorStr(QColor color)  const;
	double min_;
	double max_;
	QColor color_;
};

#endif