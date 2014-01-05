#include "namedefs.h"
#include "memleakdetect.h"

namespace Picto {

/*! \brief Defines the names of the difference Picto apps for the various purposes (mostly client identification in network communication).
*/
TranslatedNames::TranslatedNames()
{
	serverAppName = tr("PictoServer");
	proxyServerAppName = tr("PictoProxyClient");
	configAppName = tr("PictoConfig");
	directorAppName = tr("PictoDirector");
	frontPanelAppName = tr("PictoFrontPanel");
	directorHWName = tr("PictoBox");
	workstationAppName = tr("PictoWorkstation");
}

}; //namespace Picto
