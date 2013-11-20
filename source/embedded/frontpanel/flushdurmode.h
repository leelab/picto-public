#ifndef FLUSHDURMODE_H
#define FLUSHDURMODE_H

#include <QList>
#include <QSharedPointer>
#include "NumberEditMode.h"

/*!	\brief A NumberEditMode including functionality for the user to change the the duration
 * of the default PictoBox flush.
 * \author Joey Schnurr, Mark Hammond, Matt Gay
 * \date 2009-2013
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

