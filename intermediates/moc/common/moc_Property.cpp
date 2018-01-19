/****************************************************************************
** Meta object code from reading C++ file 'Property.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.5.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../source/common/property/Property.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'Property.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.5.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_Picto__Property_t {
    QByteArrayData data[11];
    char stringdata0[120];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Picto__Property_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Picto__Property_t qt_meta_stringdata_Picto__Property = {
    {
QT_MOC_LITERAL(0, 0, 15), // "Picto::Property"
QT_MOC_LITERAL(1, 16, 16), // "initValueChanged"
QT_MOC_LITERAL(2, 33, 0), // ""
QT_MOC_LITERAL(3, 34, 9), // "Property*"
QT_MOC_LITERAL(4, 44, 11), // "changedProp"
QT_MOC_LITERAL(5, 56, 12), // "newInitValue"
QT_MOC_LITERAL(6, 69, 12), // "valueChanged"
QT_MOC_LITERAL(7, 82, 8), // "newValue"
QT_MOC_LITERAL(8, 91, 12), // "setInitValue"
QT_MOC_LITERAL(9, 104, 6), // "_value"
QT_MOC_LITERAL(10, 111, 8) // "setValue"

    },
    "Picto::Property\0initValueChanged\0\0"
    "Property*\0changedProp\0newInitValue\0"
    "valueChanged\0newValue\0setInitValue\0"
    "_value\0setValue"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Picto__Property[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    2,   34,    2, 0x06 /* Public */,
       6,    2,   39,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       8,    1,   44,    2, 0x0a /* Public */,
      10,    1,   47,    2, 0x0a /* Public */,

 // signals: parameters
    QMetaType::Void, 0x80000000 | 3, QMetaType::QVariant,    4,    5,
    QMetaType::Void, 0x80000000 | 3, QMetaType::QVariant,    4,    7,

 // slots: parameters
    QMetaType::Void, QMetaType::QVariant,    9,
    QMetaType::Void, QMetaType::QVariant,    9,

       0        // eod
};

void Picto::Property::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Property *_t = static_cast<Property *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->initValueChanged((*reinterpret_cast< Property*(*)>(_a[1])),(*reinterpret_cast< QVariant(*)>(_a[2]))); break;
        case 1: _t->valueChanged((*reinterpret_cast< Property*(*)>(_a[1])),(*reinterpret_cast< QVariant(*)>(_a[2]))); break;
        case 2: _t->setInitValue((*reinterpret_cast< QVariant(*)>(_a[1]))); break;
        case 3: _t->setValue((*reinterpret_cast< QVariant(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<int*>(_a[0]) = -1; break;
        case 0:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< Property* >(); break;
            }
            break;
        case 1:
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
            typedef void (Property::*_t)(Property * , QVariant );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&Property::initValueChanged)) {
                *result = 0;
            }
        }
        {
            typedef void (Property::*_t)(Property * , QVariant );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&Property::valueChanged)) {
                *result = 1;
            }
        }
    }
}

const QMetaObject Picto::Property::staticMetaObject = {
    { &Asset::staticMetaObject, qt_meta_stringdata_Picto__Property.data,
      qt_meta_data_Picto__Property,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *Picto::Property::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Picto::Property::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_Picto__Property.stringdata0))
        return static_cast<void*>(const_cast< Property*>(this));
    return Asset::qt_metacast(_clname);
}

int Picto::Property::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = Asset::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 4)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 4;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 4)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 4;
    }
    return _id;
}

// SIGNAL 0
void Picto::Property::initValueChanged(Property * _t1, QVariant _t2)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void Picto::Property::valueChanged(Property * _t1, QVariant _t2)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}
QT_END_MOC_NAMESPACE
