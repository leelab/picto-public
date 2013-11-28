#ifndef TOOLGROUP_H
#define TOOLGROUP_H
#include <QWidget>
#include <QList>
#include <QMap>
#include <QString>
#include <QButtonGroup>
#include "EditorState.h"
QT_BEGIN_NAMESPACE
class QButtonGroup;
class QToolButton;
class QAbstractButton;
class QGridLayout;
class QIcon;
class QWidget;
QT_END_NAMESPACE

/*! \brief A group of buttons of a particular type that have some affect on activity in the designer.
 *	\details Each of the subcategories in the Toolbox on the left hand side of the designer window is a ToolGroup.
 *	Different types of ToolGroups are allowed; however, currently we are only using AssetToolGroup objects.
 *	\author Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2013
 */
class ToolGroup : public QWidget
{
    Q_OBJECT

public:
	ToolGroup(QSharedPointer<EditorState> editorState, QWidget *parent=0);
	virtual ~ToolGroup(){};
	int numButtons();
	/*! \brief Returns the name of the currently selected button.*/
	QString getSelectedButton(){return selectedButton_;};

protected:
	void AddButton(const QString &label, QIcon icon, bool enabled = true);
	void clearButtons();
	/*! \brief Returns the EditorState referenced by this ToolGroup.*/
	QSharedPointer<EditorState> getEditorState(){return editorState_;};
	QPixmap getButtonPixmap(int buttonId);
	/*! \brief Called when the a button is pressed so that the button can do whatever it's supposed to do.*/
	virtual void doButtonAction(int buttonId) = 0;
	/*! \brief Called when all buttons are unpressed and similar situations to disable all button actions that may have been
	 *	started during a previous doButtonAction() call.
	 *	\details For example, as AssetToolGroup might have set a new insertion item to be added to the design when the user
	 *	clicks the canvas.  When this function is called, it sets the insertion item back to NULL.
	 */
	virtual void disableButtonActions(){};
	/*! \brief Returns true if the button at the input button id should be enabled in the widget and false if it
	 *	should appear grayed out.
	 *	\details For example, if the number of child assets of a certain type is limited in a particular window, this
	 *	would return false for that asset's button once the limit is reached.
	 */
	virtual bool isEnabled(int buttonId) = 0;
private slots:
    void buttonGroupClicked(int id);
	void disableAllButtons();

private:
	QSharedPointer<EditorState> editorState_;
	QSharedPointer<QButtonGroup> buttonGroup_;
	QGridLayout* layout_;
	QList<QWidget*> widgets_;
	QMap<int,QString> nameMap_;
	QString selectedButton_;
	int row_;
	int col_;
};

#endif
