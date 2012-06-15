#ifndef TextEditMode_H
#define TextEditMode_H

#include <QList>
#include <QSharedPointer>
#include "DisplayMode.h"

/*!	\brief The front panel's TextEditMode interface
 *
 *	The front panel's TextEditMode interface is defined here.
 */
class TextEditMode : public DisplayMode
{
	Q_OBJECT

public:
	TextEditMode(int maxChars,QString valName,int slowInc,int fastInc);
	virtual ~TextEditMode();
	virtual void initMode();

protected:
	virtual PanelInfo::DisplayModeType handleUserInput(PanelInfo::InputType type);
	virtual QString getEditableText()=0;
	virtual bool setValueToDirector(QString text)=0;

private:
	void drawDisplay();
	QSharedPointer<DirectorInterface> directorIf_;
	QString currText_;
	QString textAtInit_;
	int currTextChar_;
	int maxChars_;
	QString valName_;
	int slowInc_;
	int fastInc_;
};


#endif

