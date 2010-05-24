/*!	\brief The error handling tool for Workstation
 *
 *	Eventually, this thing will be a full bown widget with a list of current
 *	errors.  However, since I'm still putting things together, it's going to 
 *	start off as a really simple list of errors, with basic access functions
 */

#ifndef _ERROR_LIST_H_
#define _ERROR_LIST_H_

#include <QWidget>

class ErrorList : public QWidget
{
	Q_OBJECT
public:
	ErrorList(QWidget *parent = 0);
	QStringList getAllErrors();

public slots:
	void clear();
	void addError(QString errorType, QString errorText);

private:
	QStringList errorTypes_;
	QStringList errorTexts_;

};

#endif