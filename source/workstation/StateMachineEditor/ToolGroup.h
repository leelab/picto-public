#ifndef TOOLGROUP_H
#define TOOLGROUP_H
#include <QWidget>
#include <QList>
#include <QMap>
#include <QString>
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
	ToolGroup(QWidget *parent=0);
	QString getSelectedButton(){return selectedButton_;};
signals:
	void insertionItemSelected(QString itemName);
protected:
	void AddButton(const QString &label, QIcon icon);
	void clearButtons();
private slots:
    void buttonGroupClicked(int id);
	void buttonGroupReleased(int id);

private:
	QButtonGroup* buttonGroup_;
	QGridLayout* layout_;
	QList<QWidget*> widgets_;
	QMap<int,QString> nameMap_;
	QString selectedButton_;
	int row_;
	int col_;
};

#endif
