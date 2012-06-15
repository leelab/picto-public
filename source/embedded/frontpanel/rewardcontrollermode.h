#ifndef REWARDCONTROLLERMODE_H
#define REWARDCONTROLLERMODE_H

#include <QList>
#include <QSharedPointer>
#include "NumberEditMode.h"

/*!	\brief The front panel's RewardControllerMode interface
 *
 *	The front panel's RewardControllerMode interface is defined here.
 */
class RewardControllerMode : public NumberEditMode
{
	Q_OBJECT

public:
	RewardControllerMode(FrontPanelInfo *panelInfo);
	virtual ~RewardControllerMode();

protected:
	virtual int getEditableNumber();
	virtual PanelInfo::DisplayModeType myModeType(){return PanelInfo::RewardControllerModeType;};
	virtual bool setValueToDirector(int val);
private:
	FrontPanelInfo *panelInfo_;
};


#endif

