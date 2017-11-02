/****************************************************************************
** Meta object code from reading C++ file 'PseudorandomIntParameter.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.5.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../source/common/parameter/PseudorandomIntParameter.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'PseudorandomIntParameter.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.5.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_Picto__PseudorandomIntParameter_t {
    QByteArrayData data[8];
    char stringdata0[82];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Picto__PseudorandomIntParameter_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Picto__PseudorandomIntParameter_t qt_meta_stringdata_Picto__PseudorandomIntParameter = {
    {
QT_MOC_LITERAL(0, 0, 31), // "Picto::PseudorandomIntParameter"
QT_MOC_LITERAL(1, 32, 9), // "randomize"
QT_MOC_LITERAL(2, 42, 0), // ""
QT_MOC_LITERAL(3, 43, 18), // "reshuffleLastValue"
QT_MOC_LITERAL(4, 62, 5), // "reset"
QT_MOC_LITERAL(5, 68, 5), // "value"
QT_MOC_LITERAL(6, 74, 3), // "min"
QT_MOC_LITERAL(7, 78, 3) // "max"

    },
    "Picto::PseudorandomIntParameter\0"
    "randomize\0\0reshuffleLastValue\0reset\0"
    "value\0min\0max"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Picto__PseudorandomIntParameter[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       3,   14, // methods
       3,   32, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,   29,    2, 0x0a /* Public */,
       3,    0,   30,    2, 0x0a /* Public */,
       4,    0,   31,    2, 0x0a /* Public */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

 // properties: name, type, flags
       5, QMetaType::Int, 0x00095103,
       6, QMetaType::Int, 0x00095103,
       7, QMetaType::Int, 0x00095103,

       0        // eod
};

void Picto::PseudorandomIntParameter::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        PseudorandomIntParameter *_t = static_cast<PseudorandomIntParameter *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->randomize(); break;
        case 1: _t->reshuffleLastValue(); break;
        case 2: _t->reset(); break;
        default: ;
        }
    }
#ifndef QT_NO_PROPERTIES
    else if (_c == QMetaObject::ReadProperty) {
        PseudorandomIntParameter *_t = static_cast<PseudorandomIntParameter *>(_o);
        Q_UNUSED(_t)
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< int*>(_v) = _t->getValue(); break;
        case 1: *reinterpret_cast< int*>(_v) = _t->getMin(); break;
        case 2: *reinterpret_cast< int*>(_v) = _t->getMax(); break;
        default: break;
        }
    } else if (_c == QMetaObject::WriteProperty) {
        PseudorandomIntParameter *_t = static_cast<PseudorandomIntParameter *>(_o);
        Q_UNUSED(_t)
        void *_v = _a[0];
        switch (_id) {
        case 0: _t->setValue(*reinterpret_cast< int*>(_v)); break;
        case 1: _t->setMin(*reinterpret_cast< int*>(_v)); break;
        case 2: _t->setMax(*reinterpret_cast< int*>(_v)); break;
        default: break;
        }
    } else if (_c == QMetaObject::ResetProperty) {
    }
#endif // QT_NO_PROPERTIES
    Q_UNUSED(_a);
}

const QMetaObject Picto::PseudorandomIntParameter::staticMetaObject = {
    { &Parameter::staticMetaObject, qt_meta_stringdata_Picto__PseudorandomIntParameter.data,
      qt_meta_data_Picto__PseudorandomIntParameter,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *Picto::PseudorandomIntParameter::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Picto::PseudorandomIntParameter::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_Picto__PseudorandomIntParameter.stringdata0))
        return static_cast<void*>(const_cast< PseudorandomIntParameter*>(this));
    return Parameter::qt_metacast(_clname);
}

int Picto::PseudorandomIntParameter::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = Parameter::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 3)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 3;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 3)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 3;
    }
#ifndef QT_NO_PROPERTIES
   else if (_c == QMetaObject::ReadProperty || _c == QMetaObject::WriteProperty
            || _c == QMetaObject::ResetProperty || _c == QMetaObject::RegisterPropertyMetaType) {
        qt_static_metacall(this, _c, _id, _a);
        _id -= 3;
    } else if (_c == QMetaObject::QueryPropertyDesignable) {
        _id -= 3;
    } else if (_c == QMetaObject::QueryPropertyScriptable) {
        _id -= 3;
    } else if (_c == QMetaObject::QueryPropertyStored) {
        _id -= 3;
    } else if (_c == QMetaObject::QueryPropertyEditable) {
        _id -= 3;
    } else if (_c == QMetaObject::QueryPropertyUser) {
        _id -= 3;
    }
#endif // QT_NO_PROPERTIES
    return _id;
}
QT_END_MOC_NAMESPACE
