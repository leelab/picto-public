#include <QDir>
#include <QMessageBox>
#include <QDebug>

#include "AnalysisPlotOutput.h"
#include "AnalysisBinaryOutputWidget.h"
#include "Analysis.h"
#include "../operator/OperatorPlot.h"
#include "../statemachine/statemachine.h"

#include "../memleakdetect.h"

namespace Picto {

QHash<QString, int> AnalysisPlotOutput::filenameMap_;

/*! \brief Creates a new AnalysisPlotOutput object.
 *	\details 
 *		- Adds a FileType list property to hold the type of the file.
 */
AnalysisPlotOutput::AnalysisPlotOutput()
	: AnalysisOutput(), fileName_(""), valid_(false)
{
	AddDefinableProperty("OperatorPlot", "");

	exportTypeList_ << "PDF" << "PostScript" << "PNG" << "BMP";
	AddDefinableProperty(PropertyContainer::enumTypeId(), "FileType", 2, "enumNames", exportTypeList_);

	exportSizeList_ << "Small" << "Medium" << "Large" << "Huge";
	AddDefinableProperty(PropertyContainer::enumTypeId(), "ExportSize", 0, "enumNames", exportSizeList_);
}

/*! \brief Creates a new AnalysisPlotOutput and returns a shared Asset pointer to it.*/
QSharedPointer<Asset> AnalysisPlotOutput::Create()
{
	return QSharedPointer<Asset>(new AnalysisPlotOutput());
}

void AnalysisPlotOutput::postDeserialize()
{
	AnalysisOutput::postDeserialize();

	connect(propertyContainer_->getProperty("OperatorPlot").data(), SIGNAL(edited()),
		this, SLOT(operatorPlotNameEdited()));

}

bool AnalysisPlotOutput::validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	bool result = true;

	result &= AnalysisOutput::validateObject(xmlStreamReader);

	if (propertyContainer_->getPropertyValue("OperatorPlot").toString().isEmpty())
	{
		addError("AnalysisPlotOutput cannot have an empty 'OperatorPlot' property.");
		result = false;
	}

	return result;
}

/*! \brief Creates a new AnalysisBinaryOutputWidget, sets its underlying file and returns an AnalysisOutputWidget pointer
 *	to it.
 *	\details It is the callers responsibility to delete the widget returned from this function.
 */
AnalysisOutputWidget* AnalysisPlotOutput::createWidget()
{
	if (!valid_)
		setupFile();

	AnalysisBinaryOutputWidget* outputWidget(new AnalysisBinaryOutputWidget());
	outputWidget->setFile(fileName_);
	return outputWidget;
}

void AnalysisPlotOutput::setupFile()
{
	if (valid_)
		return;

	QString outputDir = getTempOutputDir();
	if (!QFile::exists(outputDir + "/" + getRunName()))
	{	//The directory doesn't exist yet.  Make it.
		QDir dir;
		dir.mkpath(outputDir + "/" + getRunName());
	}

	QString fileName = operatorPlot_->getTitle();
	fileName.replace(' ', '_');

	switch (getExportType())
	{
	case ExportType::EXPORT_PDF:
		fileName += ".pdf";
		break;
	case ExportType::EXPORT_PNG:
		fileName += ".png";
		break;
	case ExportType::EXPORT_BMP:
		fileName += ".bmp";
		break;
	case ExportType::EXPORT_POSTSCRIPT:
		fileName += ".ps";
		break;
	default:
		qDebug() << "Unexpected Export Type";
		return;
	}

	fileName_ = outputDir + "/" + getRunName() + "/" + getRunName() + "." + fileName;
	
	if (filenameMap_.contains(fileName_))
	{
		int version = filenameMap_[fileName_]++;

		fileName_ = outputDir + "/" + getRunName() + "/" + getRunName() + "." + QString("%1").arg(version).rightJustified(2,'0') + "." + fileName;
	}
	else
	{
		filenameMap_[fileName_] = 1;
	}

	valid_ = true;
}

void AnalysisPlotOutput::exportPlot()
{
	if (!operatorPlot_)
	{
		qDebug() << "Tried to output an unlinked Plot";
		return;
	}

	if (!valid_)
		setupFile();

	emit operatorPlot_->exportPlot((int)getExportType(), (int)getExportSize(), fileName_);
}


ExportType::ExportType AnalysisPlotOutput::getExportType() const
{
	return (ExportType::ExportType)(propertyContainer_->getPropertyValue("FileType").toInt());
}

DataViewSize::ViewSize AnalysisPlotOutput::getExportSize() const
{
	return (DataViewSize::ViewSize)(propertyContainer_->getPropertyValue("ExportSize").toInt() + 1);
}

/*! \brief Connects this object to its attached OperatorPlot
 */
void AnalysisPlotOutput::postLinkUpdate()
{
	AnalysisOutput::postLinkUpdate();

	linkToPlot();
}

//! Slot when the property for operatorPlot is edited
void AnalysisPlotOutput::operatorPlotNameEdited()
{
	linkToPlot();
}

//! Helper function to find and connect this AnalysisPlotOutput to its Plot
void AnalysisPlotOutput::linkToPlot()
{
	QSharedPointer<Analysis> analysis = getParentAsset().objectCast<Analysis>();
	QString targetName = propertyContainer_->getPropertyValue("OperatorPlot").toString();
	foreach(QString childTag, analysis->getValidChildTags())
	{
		foreach(QSharedPointer<Asset> child, analysis->getGeneratedChildren(childTag))
		{
			if (child.isNull())
				continue;
			if (child->getName() == targetName && child->inherits("Picto::OperatorPlot"))
			{
				operatorPlot_ = child.objectCast<OperatorPlot>();
				break;
			}
		}
	}
}

void AnalysisPlotOutput::finishUp()
{
	valid_ = false;
	filenameMap_.clear();
}

}; //namespace Picto
