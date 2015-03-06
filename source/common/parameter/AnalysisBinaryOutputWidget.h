#ifndef _BINARY_OUTPUT_WIDGET_H_
#define _BINARY_OUTPUT_WIDGET_H_

#include <QSharedPointer>
#include <QFile>
#include <QWidget>
#include <QTextEdit>
#include <QLabel>
#include "AnalysisOutputWidget.h"

namespace Picto {

/*! \brief An AnalysisOutputWidget for displaying binary files.
 *	\details At some point we may fill this in with something more meaningful.  Currently,
 *	this simply shows a text box that says "Preview is not supported for binary files."
 *	\sa AnalysisFileOutputWidget
 *	\author Trevor Stavropoulos, Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2015
 */
class AnalysisBinaryOutputWidget : public AnalysisOutputWidget
{
	Q_OBJECT
public:
	AnalysisBinaryOutputWidget(QWidget* parent = 0);
	virtual ~AnalysisBinaryOutputWidget();

	bool setFile(QString filename);
	virtual bool isSaveable();
	virtual bool saveOutputTo(QDir directory);
private:
	QSharedPointer<QFile> file_;
	QTextEdit* textEdit_;
};
}; //namespace Picto
#endif