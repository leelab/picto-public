#ifndef CHANGENAMEMODE_H
#define CHANGENAMEMODE_H

#include <QList>
#include <QSharedPointer>
#include "TextEditMode.h"

/*!	\brief A TextEditMode including functionality for the user to change the PictoBox's name
 * \author Trevor Stavropoulos, Joey Schnurr, Mark Hammond, Matt Gay
 * \date 2009-2015
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

