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

class ToolGroup : public QWidget
{
    Q_OBJECT

public:
	ToolGroup(QSharedPointer<EditorState> editorState, QWidget *parent=0);
	virtual ~ToolGroup(){};
	QString getSelectedButton(){return selectedButton_;};

protected:
	void AddButton(const QString &label, QIcon icon, bool enabled = true);
	void clearButtons();
	QSharedPointer<EditorState> getEditorState(){return editorState_;};

	virtual void doButtonAction(int buttonId) = 0;
	virtual void disableButtonActions(){};
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
