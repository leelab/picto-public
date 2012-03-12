#ifndef _ANALYSIS_PARAMETER_SELECTOR_H_
#define _ANALYSIS_PARAMETER_SELECTOR_H_

#include <QWidget>
#include <QLineEdit>
#include <QComboBox>
#include <QLabel>
#include <QSqlDatabase>

/*!	\brief	This views and controls a remotely running experiment
 *
 *	This is the viewer that will be used while an experiment is running.  It has the
 *	following features:
 *		Allows starting, stopping, and pausing of remotely running experiments
 *		Displays behavioral input (eye tracker) as a moving cursor
 *		Displays current state of remote experiment
 */
class AnalysisParameterSelector : public QWidget
{
	Q_OBJECT
public:
	AnalysisParameterSelector(QWidget *parent=0);
	virtual ~AnalysisParameterSelector();
	void setSession(QSqlDatabase session){resetQueryParameterData();session_ = session;};
	bool paramsAreValid(){getQueryParameters();return true;};
	int getPrintAssetId(){return printAssetId_;};
	int getWhenAssetId(){return whenAssetId_;};
	enum eventType{CHANGES, STARTS, ENDS};
	int getWhenEventType(){return eventType_->itemData(eventType_->currentIndex()).toInt();};
	bool whenAssetIsProperty(){return whenAssetIsProperty_;};
	int getTimeSinceEventType(){return timeSinceEventType_->itemData(timeSinceEventType_->currentIndex()).toInt();};
	bool timeSinceIsProperty(){return timeSinceIsProperty_;};
	QString getWhenDestTransitions(){return whenDestTransitions_;};
	QString getWhenSourceTransitions(){return whenSourceTransitions_;};
	QString getTimeSinceDestTransitions(){return timeSinceDestTransitions_;};
	QString getTimeSinceSourceTransitions(){return timeSinceSourceTransitions_;};
	bool shouldStampTime(){return shouldStampTime_->currentIndex();};

private:

	void setupUi();
	int getElementId(QString path);
	int getPropertyId(int parentId, QString name);
	void resetQueryParameterData();
	void getQueryParameters();

	QString getTransitionIds(int parentId, QString name, bool asSource);

	QLineEdit *printPath_;
	QLineEdit *whenPath_;
	QComboBox *eventType_;
	QComboBox *shouldStampTime_;
	QLabel *timeSinceLabel_;
	QLineEdit *timeSince_;
	QComboBox *timeSinceEventType_;
	int printAssetId_;
	bool printAssetIsProperty_;
	QString lastPrintAssetPath_;
	int whenAssetId_;
	QString lastWhenAssetPath_;
	QString whenSourceTransitions_;
	QString whenDestTransitions_;
	bool whenAssetIsProperty_;
	int timeSinceAssetId_;
	QString lastTimeSinceAssetPath_;
	bool timeSinceIsProperty_;
	QString timeSinceSourceTransitions_;
	QString timeSinceDestTransitions_;

	QSqlDatabase session_;

private slots:
	void updateUI();
};

#endif