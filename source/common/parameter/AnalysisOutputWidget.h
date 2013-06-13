#ifndef _ANALYSIS_OUTPUT_WIDGET_H_
#define _ANALYSIS_OUTPUT_WIDGET_H_

#include <QWidget>
#include <QDir>
#include "../common.h"

namespace Picto {

#if defined WIN32 || defined WINCE
class PICTOLIB_API AnalysisOutputWidget : public QWidget
#else
class AnalysisOutputWidget : public QWidget
#endif
{
	Q_OBJECT
public:
	AnalysisOutputWidget(QWidget* parent = 0);
	virtual ~AnalysisOutputWidget();
	virtual bool isSaveable(){return false;};
	virtual bool saveOutputTo(QDir directory){return false;};
	virtual QString getName(){return objectName();};
}; //namespace Picto
};
#endif