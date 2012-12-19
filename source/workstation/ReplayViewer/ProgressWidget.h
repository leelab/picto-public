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
	double getHighlightMin(int index);
	void setHighlightMin(int index,double progress);
	double getHighlightMax(int index);
	void setHighlightMax(int index,double progress);
	void setHighlightRange(int index,double minProgress,double maxProgress);
	QColor getHighlightColor(int index);
	void setHighlightColor(int index, QColor color);
	double getSliderProgress();
	void setSliderProgress(double progress);

signals:
	void userAction(bool mouseDown);
	void valueRequested(double progress);

private:
	void verifyHighlightIndex(int index);
	void updateHighlights();
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