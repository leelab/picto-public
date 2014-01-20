#ifndef _ERROR_LIST_H_
#define _ERROR_LIST_H_

#include <QWidget>

/*!	\brief A simple list for holding error messages and their types.
 *	\details This is a very simple object.  Errors are added to the
 *	list with addError(), they are accessed with getAllErrors() and 
 *	the list is cleared with clear().
 *	\author Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2013
 */
class ErrorList : public QWidget
{
	Q_OBJECT
public:
	ErrorList(QWidget *parent = 0);
	virtual ~ErrorList(){};
	QStringList getAllErrors();

public slots:
	void clear();
	void addError(QString errorType, QString errorText);

private:
	QStringList errorTypes_;
	QStringList errorTexts_;

};

#endif