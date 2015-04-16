#ifndef _DATA_VIEW_SPECS_H_
#define _DATA_VIEW_SPECS_H_

#include "../common.h"

namespace Picto {
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

namespace QwtFactoryRequest {
	/*! \brief Enumeration of the various widgets to request from the QwtFactory
	 *	\note We have to use a factory to construct our widgets so they belong to the UI thread
	 */
	enum QwtWidgetType : int
	{
		QWT_HISTOGRAM_PLOT = 0,		//!< A QwtPlotHistogram object
		QWT_WIDGET_MAX
	};
}; //namespace QwtFactoryRequest

}; //namespace Picto

#endif
