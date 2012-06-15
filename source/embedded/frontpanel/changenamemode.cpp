
#include "ChangeNameMode.h"
#include "../../common/protocol/protocolcommand.h"
#include "../../common/protocol/protocolresponse.h"

ChangeNameMode::ChangeNameMode(QSharedPointer<DirectorInterface> directorIf) :
TextEditMode(15,"Name",1,3),
directorIf_(directorIf)
{
	Q_ASSERT(directorIf);
}

ChangeNameMode::~ChangeNameMode()
{
}

QString ChangeNameMode::getEditableText()
{
	return directorIf_->getName();
}

bool ChangeNameMode::setValueToDirector(QString text)
{
	return directorIf_->setName(text);
}