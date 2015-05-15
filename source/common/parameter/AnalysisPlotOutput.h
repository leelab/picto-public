#ifndef _ANALYSIS_PLOT_OUTPUT_H_
#define _ANALYSIS_PLOT_OUTPUT_H_

#include "../common.h"

#include "../operator/DataViewSpecs.h"
#include "AnalysisOutput.h"

namespace Picto {
	class OperatorPlot;

/*! \brief An AnalysisOutput object used for writing Plots to file.
 *	\details This object must be associated with an existing AnalysisPlot in order to function.
 *	
 *	For in Picto viewing of output data, this object returns an AnalysisBinaryOutputWidget from createWidget.
 *	\sa AnalysisBinaryOutputWidget, AnalysisOutputWidgetContainer
 *	\author Trevor Stavropoulos, Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2015
 */
#if defined WIN32 || defined WINCE
	class PICTOLIB_API AnalysisPlotOutput : public AnalysisOutput
#else
	class AnalysisPlotOutput : public AnalysisOutput
#endif
{
	Q_OBJECT

public:
	AnalysisPlotOutput();
	virtual ~AnalysisPlotOutput(){};

	static QSharedPointer<Asset> Create();

	virtual QString friendlyTypeName(){return "Plot Output";};

	virtual void postLinkUpdate();

public slots:
	void exportPlot();

protected:
	virtual void postDeserialize();
	virtual bool validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader);

	virtual AnalysisOutputWidget* createWidget();

private slots:
	void operatorPlotNameEdited();

private:
	//! Returns the chosen export type
	ExportType::ExportType getExportType() const;
	void setupFile();

	void linkToPlot();

	QStringList typeList_; 
	bool valid_;

	QString fileName_;

	QSharedPointer<OperatorPlot> operatorPlot_;

	//! Holds the names of the various export types
	QStringList exportTypeList_;
};


}; //namespace Picto

#endif
