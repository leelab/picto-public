/****************************************************************************
** Meta object code from reading C++ file 'ColorParameter.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.5.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../source/common/parameter/ColorParameter.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'ColorParameter.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.5.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_Picto__ColorParameter_t {
    QByteArrayData data[12];
    char stringdata0[67];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Picto__ColorParameter_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Picto__ColorParameter_t qt_meta_stringdata_Picto__ColorParameter = {
    {
QT_MOC_LITERAL(0, 0, 21), // "Picto::ColorParameter"
QT_MOC_LITERAL(1, 22, 8), // "setColor"
QT_MOC_LITERAL(2, 31, 0), // ""
QT_MOC_LITERAL(3, 32, 1), // "r"
QT_MOC_LITERAL(4, 34, 1), // "g"
QT_MOC_LITERAL(5, 36, 1), // "b"
QT_MOC_LITERAL(6, 38, 1), // "a"
QT_MOC_LITERAL(7, 40, 3), // "red"
QT_MOC_LITERAL(8, 44, 5), // "green"
QT_MOC_LITERAL(9, 50, 4), // "blue"
QT_MOC_LITERAL(10, 55, 5), // "alpha"
QT_MOC_LITERAL(11, 61, 5) // "value"

    },
    "Picto::ColorParameter\0setColor\0\0r\0g\0"
    "b\0a\0red\0green\0blue\0alpha\0value"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Picto__ColorParameter[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       2,   14, // methods
       5,   40, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    4,   24,    2, 0x0a /* Public */,
       1,    3,   33,    2, 0x2a /* Public | MethodCloned */,

 // slots: parameters
    QMetaType::Void, QMetaType::Int, QMetaType::Int, QMetaType::Int, QMetaType::Int,    3,    4,    5,    6,
    QMetaType::Void, QMetaType::Int, QMetaType::Int, QMetaType::Int,    3,    4,    5,

 // properties: name, type, flags
       7, QMetaType::Int, 0x00095103,
       8, QMetaType::Int, 0x00095103,
       9, QMetaType::Int, 0x00095103,
      10, QMetaType::Int, 0x00095103,
      11, QMetaType::QVariant, 0x00095003,

       0        // eod
};

void Picto::ColorParameter::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        ColorParameter *_t = static_cast<ColorParameter *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->setColor((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3])),(*reinterpret_cast< int(*)>(_a[4]))); break;
        case 1: _t->setColor((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3]))); break;
        default: ;
        }
    }
#ifndef QT_NO_PROPERTIES
    else if (_c == QMetaObject::ReadProperty) {
        ColorParameter *_t = static_cast<ColorParameter *>(_o);
        Q_UNUSED(_t)
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< int*>(_v) = _t->getRed(); break;
        case 1: *reinterpret_cast< int*>(_v) = _t->getGreen(); break;
        case 2: *reinterpret_cast< int*>(_v) = _t->getBlue(); break;
        case 3: *reinterpret_cast< int*>(_v) = _t->getAlpha(); break;
        case 4: *reinterpret_cast< QVariant*>(_v) = _t->getColor(); break;
        default: break;
        }
    } else if (_c == QMetaObject::WriteProperty) {
        ColorParameter *_t = static_cast<ColorParameter *>(_o);
        Q_UNUSED(_t)
        void *_v = _a[0];
        switch (_id) {
        case 0: _t->setRed(*reinterpret_cast< int*>(_v)); break;
        case 1: _t->setGreen(*reinterpret_cast< int*>(_v)); break;
        case 2: _t->setBlue(*reinterpret_cast< int*>(_v)); break;
        case 3: _t->setAlpha(*reinterpret_cast< int*>(_v)); break;
        case 4: _t->setColor(*reinterpret_cast< QVariant*>(_v)); break;
        default: break;
        }
    } else if (_c == QMetaObject::ResetProperty) {
    }
#endif // QT_NO_PROPERTIES
}

const QMetaObject Picto::ColorParameter::staticMetaObject = {
    { &Parameter::staticMetaObject, qt_meta_stringdata_Picto__ColorParameter.data,
      qt_meta_data_Picto__ColorParameter,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *Picto::ColorParameter::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Picto::ColorParameter::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_Picto__ColorParameter.stringdata0))
        return static_cast<void*>(const_cast< ColorParameter*>(this));
    return Parameter::qt_metacast(_clname);
}

int Picto::ColorParameter::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = Parameter::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 2)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 2;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 2)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 2;
    }
#ifndef QT_NO_PROPERTIES
   else if (_c == QMetaObject::ReadProperty || _c == QMetaObject::WriteProperty
            || _c == QMetaObject::ResetProperty || _c == QMetaObject::RegisterPropertyMetaType) {
        qt_static_metacall(this, _c, _id, _a);
        _id -= 5;
    } else if (_c == QMetaObject::QueryPropertyDesignable) {
        _id -= 5;
    } else if (_c == QMetaObject::QueryPropertyScriptable) {
        _id -= 5;
    } else if (_c == QMetaObject::QueryPropertyStored) {
        _id -= 5;
    } else if (_c == QMetaObject::QueryPropertyEditable) {
        _id -= 5;
    } else if (_c == QMetaObject::QueryPropertyUser) {
        _id -= 5;
    }
#endif // QT_NO_PROPERTIES
    return _id;
}
QT_END_MOC_NAMESPACE
