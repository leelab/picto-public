#ifndef REWARDCONTROLLERMODE_H
#define REWARDCONTROLLERMODE_H

#include <QList>
#include <QSharedPointer>
#include "NumberEditMode.h"

/*!	\brief A NumberEditMode including functionality for the user to change the current Reward Controller.
 * \author Trevor Stavropoulos, Joey Schnurr, Mark Hammond, Matt Gay
 * \date 2009-2015
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

