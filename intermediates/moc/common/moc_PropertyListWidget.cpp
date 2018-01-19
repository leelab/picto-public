/****************************************************************************
** Meta object code from reading C++ file 'PropertyListWidget.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.5.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../source/common/designer/PropertyListWidget.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#include <QtCore/QSharedPointer>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'PropertyListWidget.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.5.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_PropertyListWidget_t {
    QByteArrayData data[9];
    char stringdata0[125];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_PropertyListWidget_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_PropertyListWidget_t qt_meta_stringdata_PropertyListWidget = {
    {
QT_MOC_LITERAL(0, 0, 18), // "PropertyListWidget"
QT_MOC_LITERAL(1, 19, 14), // "propertyEdited"
QT_MOC_LITERAL(2, 34, 0), // ""
QT_MOC_LITERAL(3, 35, 24), // "QSharedPointer<Property>"
QT_MOC_LITERAL(4, 60, 4), // "prop"
QT_MOC_LITERAL(5, 65, 3), // "val"
QT_MOC_LITERAL(6, 69, 17), // "propertyWasEdited"
QT_MOC_LITERAL(7, 87, 27), // "propertyWasEditedExternally"
QT_MOC_LITERAL(8, 115, 9) // "Property*"

    },
    "PropertyListWidget\0propertyEdited\0\0"
    "QSharedPointer<Property>\0prop\0val\0"
    "propertyWasEdited\0propertyWasEditedExternally\0"
    "Property*"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_PropertyListWidget[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       3,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    2,   29,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       6,    2,   34,    2, 0x08 /* Private */,
       7,    2,   39,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void, 0x80000000 | 3, QMetaType::QVariant,    4,    5,

 // slots: parameters
    QMetaType::Void, 0x80000000 | 3, QMetaType::QVariant,    4,    5,
    QMetaType::Void, 0x80000000 | 8, QMetaType::QVariant,    4,    5,

       0        // eod
};

void PropertyListWidget::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        PropertyListWidget *_t = static_cast<PropertyListWidget *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->propertyEdited((*reinterpret_cast< QSharedPointer<Property>(*)>(_a[1])),(*reinterpret_cast< QVariant(*)>(_a[2]))); break;
        case 1: _t->propertyWasEdited((*reinterpret_cast< QSharedPointer<Property>(*)>(_a[1])),(*reinterpret_cast< QVariant(*)>(_a[2]))); break;
        case 2: _t->propertyWasEditedExternally((*reinterpret_cast< Property*(*)>(_a[1])),(*reinterpret_cast< QVariant(*)>(_a[2]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<int*>(_a[0]) = -1; break;
        case 0:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QSharedPointer<Property> >(); break;
            }
            break;
        case 1:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QSharedPointer<Property> >(); break;
            }
            break;
        case 2:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< Property* >(); break;
            }
            break;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (PropertyListWidget::*_t)(QSharedPointer<Property> , QVariant );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&PropertyListWidget::propertyEdited)) {
                *result = 0;
            }
        }
    }
}

const QMetaObject PropertyListWidget::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_PropertyListWidget.data,
      qt_meta_data_PropertyListWidget,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *PropertyListWidget::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *PropertyListWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_PropertyListWidget.stringdata0))
        return static_cast<void*>(const_cast< PropertyListWidget*>(this));
    return QWidget::qt_metacast(_clname);
}

int PropertyListWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 3)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 3;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 3)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 3;
    }
    return _id;
}

// SIGNAL 0
void PropertyListWidget::propertyEdited(QSharedPointer<Property> _t1, QVariant _t2)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_END_MOC_NAMESPACE
