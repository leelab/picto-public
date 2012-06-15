#ifndef FLUSHDURMODE_H
#define FLUSHDURMODE_H

#include <QList>
#include <QSharedPointer>
#include "NumberEditMode.h"

/*!	\brief The front panel's FlushDurMode interface
 *
 *	The front panel's FlushDurMode interface is defined here.
 */
class FlushDurMode : public NumberEditMode
{
	Q_OBJECT

public:
	FlushDurMode(QSharedPointer<DirectorInterface> directorIf,FrontPanelInfo *panelInfo);
	virtual ~FlushDurMode();

protected:
	virtual int getEditableNumber();
	virtual PanelInfo::DisplayModeType myModeType(){return PanelInfo::FlushDurModeType;};
	virtual bool setValueToDirector(int val);
private:
	QSharedPointer<DirectorInterface> directorIf_;
	FrontPanelInfo *panelInfo_;
};


#endif

