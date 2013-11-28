#ifndef _ANALYSIS_OUTPUT_WIDGET_H_
#define _ANALYSIS_OUTPUT_WIDGET_H_

#include <QWidget>
#include <QDir>
#include "../common.h"

namespace Picto {

/*! \brief The base class of widgets that display a representation of the output of an AnalysisOutput object.
 *	\details These widgets are displayed in an AnalysisOutputWidgetContainer for monitoring of Analysis outputs
 *	from within the Picto application.  Different AnalysisOutput classes will be associated with different 
 *	AnalysisWidgets, for example, an AnalysisFileOutput will be associated with an AnalysisFileOutputWidget, whereas
 *	an AnalysisRasterOutput might still save its data to file, but be viewable using an AnalysisRasterPlotWidget.
 *	\author Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2013
 */
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
	/*! \brief Returns true if the data represented in this widget is saveable to disk, false otherwise.*/
	virtual bool isSaveable(){return false;};
	/*! \brief Saves the data represnted in this widget to the input directory, returns true if successful, false otherwise.
	 */
	virtual bool saveOutputTo(QDir directory){return false;};
	/*! \brief Returns the name of this widget.
	 *	\details This will be the name used as its title in a tabbed layout.*/
	virtual QString getName(){return objectName();};
}; //namespace Picto
};
#endif