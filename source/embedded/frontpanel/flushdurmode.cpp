
#include "FlushDurMode.h"
#include "../../common/protocol/protocolcommand.h"
#include "../../common/protocol/protocolresponse.h"

FlushDurMode::FlushDurMode(QSharedPointer<DirectorInterface> directorIf,FrontPanelInfo *panelInfo) :
NumberEditMode(999,1,"Flush","s",10,1),
directorIf_(directorIf),
panelInfo_(panelInfo)
{
	Q_ASSERT(directorIf);
}

FlushDurMode::~FlushDurMode()
{
}

int FlushDurMode::getEditableNumber()
{
	return directorIf_->getFlushDuration(panelInfo_->getRewardController());
}

bool FlushDurMode::setValueToDirector(int val)
{
	return directorIf_->setFlushDuration(val,panelInfo_->getRewardController());
}