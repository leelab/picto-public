#ifndef REWARDDURMODE_H
#define REWARDDURMODE_H

#include <QList>
#include <QSharedPointer>
#include "NumberEditMode.h"

/*!	\brief A NumberEditMode including functionality for the user to change the the duration
 * of the default PictoBox reward.
 * \author Trevor Stavropoulos, Joey Schnurr, Mark Hammond, Matt Gay
 * \date 2009-2015
 */
class RewardDurMode : public NumberEditMode
{
	Q_OBJECT

public:
	RewardDurMode(QSharedPointer<DirectorInterface> directorIf,FrontPanelInfo *panelInfo);
	virtual ~RewardDurMode();

protected:
	virtual int getEditableNumber();
	virtual PanelInfo::DisplayModeType myModeType(){return PanelInfo::RewardDurModeType;};
	virtual bool setValueToDirector(int val);
private:
	QSharedPointer<DirectorInterface> directorIf_;
	FrontPanelInfo *panelInfo_;
};


#endif

