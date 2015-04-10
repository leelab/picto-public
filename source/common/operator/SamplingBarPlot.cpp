#include "SamplingBarPlot.h"

#include "../memleakdetect.h"

namespace Picto {

const QString SamplingBarPlot::type = "Sampling Bar Plot";

SamplingBarPlot::SamplingBarPlot()
{

}

/*!	\brief Constructs and returns a shared pointer to a new SamplingBarPlot
 */
QSharedPointer<Asset> SamplingBarPlot::Create()
{
	return QSharedPointer<Asset>(new SamplingBarPlot());
}

}; //namespace Picto
