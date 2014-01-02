#include "BinaryDataOutput.h"
#include "../memleakdetect.h"

namespace Picto {

/*! \brief I believe that this is no longer used.  It sbould probably be deleted.*/
const QString BinaryDataOutput::type = "Binary Data Output";

/*! \brief Constructs a BinaryDataOutput object.
 *	\details Adds a Value Property to store an integer value that will be output in binary form over the output port.
 */
BinaryDataOutput::BinaryDataOutput()
: OutputSignal()
{
	AddDefinableProperty(QVariant::Int,"Value",0);
}

/*! \brief Creates a new BinaryDataOutput object and returns a shared Asset pointer to it.*/
QSharedPointer<Asset> BinaryDataOutput::Create()
{
	return QSharedPointer<Asset>(new BinaryDataOutput());
}

void BinaryDataOutput::postDeserialize()
{
	OutputSignal::postDeserialize();
}

bool BinaryDataOutput::validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	if(!OutputSignal::validateObject(xmlStreamReader))
		return false;
	return true;
}

}; //namespace Picto
