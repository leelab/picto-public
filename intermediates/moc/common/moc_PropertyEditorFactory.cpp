/****************************************************************************
** Meta object code from reading C++ file 'PropertyEditorFactory.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.5.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../source/common/designer/PropertyEditorFactory.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#include <QtCore/QSharedPointer>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'PropertyEditorFactory.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.5.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_PropertyEditorFactory_t {
    QByteArrayData data[10];
    char stringdata0[118];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_PropertyEditorFactory_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_PropertyEditorFactory_t qt_meta_stringdata_PropertyEditorFactory = {
    {
QT_MOC_LITERAL(0, 0, 21), // "PropertyEditorFactory"
QT_MOC_LITERAL(1, 22, 14), // "propertyEdited"
QT_MOC_LITERAL(2, 37, 0), // ""
QT_MOC_LITERAL(3, 38, 24), // "QSharedPointer<Property>"
QT_MOC_LITERAL(4, 63, 4), // "prop"
QT_MOC_LITERAL(5, 68, 3), // "val"
QT_MOC_LITERAL(6, 72, 18), // "qtPropValueChanged"
QT_MOC_LITERAL(7, 91, 11), // "QtProperty*"
QT_MOC_LITERAL(8, 103, 8), // "property"
QT_MOC_LITERAL(9, 112, 5) // "value"

    },
    "PropertyEditorFactory\0propertyEdited\0"
    "\0QSharedPointer<Property>\0prop\0val\0"
    "qtPropValueChanged\0QtProperty*\0property\0"
    "value"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_PropertyEditorFactory[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       2,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    2,   24,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       6,    2,   29,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void, 0x80000000 | 3, QMetaType::QVariant,    4,    5,

 // slots: parameters
    QMetaType::Void, 0x80000000 | 7, QMetaType::QVariant,    8,    9,

       0        // eod
};

void PropertyEditorFactory::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        PropertyEditorFactory *_t = static_cast<PropertyEditorFactory *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->propertyEdited((*reinterpret_cast< QSharedPointer<Property>(*)>(_a[1])),(*reinterpret_cast< QVariant(*)>(_a[2]))); break;
        case 1: _t->qtPropValueChanged((*reinterpret_cast< QtProperty*(*)>(_a[1])),(*reinterpret_cast< const QVariant(*)>(_a[2]))); break;
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
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (PropertyEditorFactory::*_t)(QSharedPointer<Property> , QVariant );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&PropertyEditorFactory::propertyEdited)) {
                *result = 0;
            }
        }
    }
}

const QMetaObject PropertyEditorFactory::staticMetaObject = {
    { &QtVariantEditorFactory::staticMetaObject, qt_meta_stringdata_PropertyEditorFactory.data,
      qt_meta_data_PropertyEditorFactory,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *PropertyEditorFactory::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *PropertyEditorFactory::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_PropertyEditorFactory.stringdata0))
        return static_cast<void*>(const_cast< PropertyEditorFactory*>(this));
    return QtVariantEditorFactory::qt_metacast(_clname);
}

int PropertyEditorFactory::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QtVariantEditorFactory::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 2)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 2;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 2)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 2;
    }
    return _id;
}

// SIGNAL 0
void PropertyEditorFactory::propertyEdited(QSharedPointer<Property> _t1, QVariant _t2)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_END_MOC_NAMESPACE
