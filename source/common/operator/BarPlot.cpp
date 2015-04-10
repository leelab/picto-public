#include "BarPlot.h"

#include "../memleakdetect.h"

namespace Picto {

const QString BarPlot::type = "Bar Plot";

BarPlot::BarPlot()
{
	
}

/*!	\brief Constructs and returns a shared pointer to a new BarPlot
 */
QSharedPointer<Asset> BarPlot::Create()
{
	return QSharedPointer<Asset>(new BarPlot());
}

}; //namespace Picto
