#ifndef _DATA_VIEW_SPECS_H_
#define _DATA_VIEW_SPECS_H_

#include <QMetaType>

#include "../common.h"

//! The Number of view slots horizontally on the Data View Tab
#define VIEWGRIDWIDTH 4
//! The Number of view slots vertically on the Data View Tab
#define VIEWGRIDHEIGHT 4

namespace Picto {
	//!	Namespace for the enumeration of the various sizes Views can take.
namespace DataViewSize {
	//!	Enumeration of the various sizes Views can take.
	enum ViewSize : int
	{
		VIEW_SIZE_1x1 = 1,	//!< A 1x1 view
		VIEW_SIZE_2x2,		//!< A 2x2 view
		VIEW_SIZE_3x3,		//!< A 3x3 view
		VIEW_SIZE_4x4,		//!< A 4x4 view
		VIEW_SIZE_MAX		//!< An invalid value
	};
}; //namespace DataViewSize

//!	Namespace for the enumeration of the various forms columns can have.
namespace ColumnType {
	//!	Enumeration of the various forms columns can have.
	enum ColumnType : int
	{
		COLUMN_OUTLINE = 0,		//!< Histogram Outline
		COLUMN_FLAT_COLUMN,		//!< Flat Bar Plot Symbols
		COLUMN_RAISED_COLUMN,	//!< 3D Bar Plot Symbols
		COLUMN_LINES,			//!< Simple lines at the specified values
		COLUMN_MAX				//!< An invalid value
	};
}; //namespace ColumnType

//!	Namespace for the enumeration of the widgets to request from the QwtFactory
namespace QwtFactoryRequest {
	/*! \brief Enumeration of the various widgets to request from the QwtFactory
	 *	\note We have to use a factory to construct our widgets so they belong to the UI thread
	 */
	enum QwtWidgetType : int
	{
		QWT_HISTOGRAM_PLOT = 0,		//!< A QwtPlotHistogram object
		QWT_WIDGET_MAX				//!< An invalid value
	};
}; //namespace QwtFactoryRequest

//!	Namespace for the enumeration of the various formats that are supported by the export function
namespace ExportType{
	//!	Enumeration of the various formats that are supported by the export function
	enum ExportType : int
	{
		EXPORT_PDF = 0,			//!< Export to PDF
		EXPORT_POSTSCRIPT,		//!< Export to PostScript
		EXPORT_PNG,				//!< Export to PNG
		EXPORT_BMP,				//!< Export to BMP
		EXPORT_MAX				//!< An invalid value
	};

}; //namespace ExportType
//!	A struct to keep track of the location of widgets within the viewer.
struct WidgetCoords
{
	//! Construct a new WidgetCoords struct.
	WidgetCoords(int x, int y)
	{
		x_ = x;
		y_ = y;
	}
	int x_;		//!< X grid coordinate
	int y_;		//!< Y grid coordinate
};

class ViewProperties{
public:
	ViewProperties() : size_(DataViewSize::VIEW_SIZE_1x1), x_(0), y_(0) {};
	ViewProperties(DataViewSize::ViewSize size, int x, int y) : size_(size), x_(x), y_(y) {};
	DataViewSize::ViewSize size_;
	int x_;
	int y_;
};

//!	Namespace for the enumeration of the various Align Methods that Rasters and PSTHs can use
namespace AlignType{
	//!	Enumeration of the various Align Methods that Rasters and PSTHs can use
	enum AlignType : int
	{
		ALIGN_BEGIN = 0,			//!< Align to Beginning of Window
		ALIGN_FLAG,					//!< Align to Flag
		ALIGN_END,					//!< Align to End of Window
		ALIGN_MAX					//!< An invalid value
	};

}; //namespace ExportType


}; //namespace Picto

Q_DECLARE_METATYPE(Picto::ViewProperties);

#endif
