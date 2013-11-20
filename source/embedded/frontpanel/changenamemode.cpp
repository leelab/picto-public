
#include "ChangeNameMode.h"
#include "../../common/protocol/protocolcommand.h"
#include "../../common/protocol/protocolresponse.h"

/*!
 * Constructs a new ChangeNameMode object, taking in a DirectorInterface as a parameter that will be
 * used to poll/push the PictoBox name from/to the director application.
 */
ChangeNameMode::ChangeNameMode(QSharedPointer<DirectorInterface> directorIf) :
TextEditMode(15,"Name",1,3),
directorIf_(directorIf)
{
	Q_ASSERT(directorIf);
}

ChangeNameMode::~ChangeNameMode()
{
}

/*! Returns the Pictobox name as received from the Director application.
*/
QString ChangeNameMode::getEditableText()
{
	return directorIf_->getName();
}

/*! Sets the latest updated Pictobox name to the director application
 * though the DirectorInterface.
 */
bool ChangeNameMode::setValueToDirector(QString text)
{
	return directorIf_->setName(text);
}