#ifndef CHANGENAMEMODE_H
#define CHANGENAMEMODE_H

#include <QList>
#include <QSharedPointer>
#include "TextEditMode.h"

/*!	\brief The front panel's ChangeNameMode interface
 *
 *	The front panel's ChangeNameMode interface is defined here.
 */
class ChangeNameMode : public TextEditMode
{
	Q_OBJECT

public:
	ChangeNameMode(QSharedPointer<DirectorInterface> directorIf);
	virtual ~ChangeNameMode();

protected:
	virtual QString getEditableText();
	virtual PanelInfo::DisplayModeType myModeType(){return PanelInfo::ChangeNameModeType;};
	virtual bool setValueToDirector(QString text);
private:
	QSharedPointer<DirectorInterface> directorIf_;
};


#endif

