#ifndef PROPERTYFRAME_H
#define PROPERTYFRAME_H
#include <QtAbstractPropertyBrowser>
#include <QtVariantPropertyManager>
#include <QtGroupBoxPropertyBrowser>
#include <qtbuttonPropertyBrowser>
#include <qttreePropertyBrowser>
#include "statemachineeditor/PropertyEditorFactory.h"
#include "../common/experiment/experiment.h"
#include <QScrollArea>
#include <QSqlDatabase>
using namespace Picto;

QT_BEGIN_NAMESPACE
class QWidget;
class QtProperty;
QT_END_NAMESPACE

//! [0]
class PropertyFrame : public QScrollArea
{
    Q_OBJECT

public:
   PropertyFrame(QWidget *parent=0);
   virtual ~PropertyFrame(){};
   void clearProperties(){setTopLevelDataStore(QSharedPointer<DataStore>());};
   void setTopLevelDataStore(QSharedPointer<DataStore> dataStore);
   void updatePropertiesFromFile(QString filename);
signals:
   void parameterMessageReady(QSharedPointer<Property> changedProp);
private:
	QSharedPointer<PropertyEditorFactory> propertyFactory_;
	QWidget* mainWidget_;
	QSqlDatabase propValsSession_;
	QHash<QString,QSharedPointer<Property>> pathMap_;
private slots:
	void propertyEdited(QSharedPointer<Property> prop);
};
//! [0]
#endif
