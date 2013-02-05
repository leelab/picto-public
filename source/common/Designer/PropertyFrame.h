#ifndef PROPERTYFRAME_H
#define PROPERTYFRAME_H
#include <QtPropertyBrowser.h>
#include <QtVariantProperty.h>
#include <QtGroupBoxPropertyBrowser.h>
#include <qtbuttonPropertyBrowser.h>
#include <qttreePropertyBrowser.h>
#include "../common/designer/PropertyEditorFactory.h"
#include "../common/experiment/experiment.h"
#include <QScrollArea>
#include <QSqlDatabase>
using namespace Picto;

QT_BEGIN_NAMESPACE
class QWidget;
class QtProperty;
QT_END_NAMESPACE

//! [0]
#if defined WIN32 || defined WINCE
class PICTOLIB_API PropertyFrame : public QScrollArea
#else
class PropertyFrame : public QScrollArea
#endif
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
	QVector<QSharedPointer<QtVariantPropertyManager>> propManagers_;
	QWidget* mainWidget_;
	QSqlDatabase propValsSession_;
	QHash<QString,QSharedPointer<Property>> pathMap_;
private slots:
	void propertyEdited(QSharedPointer<Property> prop,QVariant val);
};
//! [0]
#endif
