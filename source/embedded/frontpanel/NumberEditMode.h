#ifndef NUMBEREDITMODE_H
#define NUMBEREDITMODE_H

#include <QList>
#include <QSharedPointer>
#include "DisplayMode.h"

/*!	\brief The front panel's NumberEditMode interface
 *
 *	The front panel's NumberEditMode interface is defined here.
 */
class NumberEditMode : public DisplayMode
{
	Q_OBJECT

public:
	NumberEditMode(int maxValue,int minValue,QString valName,int fastIncrement,int slowIncrement);
	virtual ~NumberEditMode();
	virtual void initMode();

protected:
	virtual PanelInfo::DisplayModeType handleUserInput(PanelInfo::InputType type);
	virtual int getEditableNumber()=0;
	virtual bool setValueToDirector(int val)=0;

private:
	void drawDisplay();
	QSharedPointer<DirectorInterface> directorIf_;
	int currNum_;
	int maxDigits_;
	int maxValue_;
	int minValue_;
	int fastInc_;
	int slowInc_;
	int numAtInit_;
	QString valName_;
};


#endif

