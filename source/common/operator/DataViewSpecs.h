#ifndef _DATA_VIEW_SPECS_H_
#define _DATA_VIEW_SPECS_H_

#include "../common.h"

namespace Picto {
namespace DataViewSize {
	/*!	\brief Enumeration of the various sizes Views can take.
	*/
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
	enum ColumnType : int
	{
		COLUMN_OUTLINE = 0, //!< Histogram Outline
		COLUMN_FLAT_COLUMN,
		COLUMN_RAISED_COLUMN,
		COLUMN_LINES,
		COLUMN_MAX
	};
}; //namespace ColumnType
}; //namespace Picto

#endif
