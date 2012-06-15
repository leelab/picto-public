#ifndef REWARDDURMODE_H
#define REWARDDURMODE_H

#include <QList>
#include <QSharedPointer>
#include "NumberEditMode.h"

/*!	\brief The front panel's RewardDurMode interface
 *
 *	The front panel's RewardDurMode interface is defined here.
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

