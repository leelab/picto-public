/****************************************************************************
** Meta object code from reading C++ file 'Reward.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.5.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../source/common/statemachine/Reward.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'Reward.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.5.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_Picto__Reward_t {
    QByteArrayData data[5];
    char stringdata0[55];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Picto__Reward_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Picto__Reward_t qt_meta_stringdata_Picto__Reward = {
    {
QT_MOC_LITERAL(0, 0, 13), // "Picto::Reward"
QT_MOC_LITERAL(1, 14, 6), // "number"
QT_MOC_LITERAL(2, 21, 12), // "unitQuantity"
QT_MOC_LITERAL(3, 34, 15), // "minRewardPeriod"
QT_MOC_LITERAL(4, 50, 4) // "chan"

    },
    "Picto::Reward\0number\0unitQuantity\0"
    "minRewardPeriod\0chan"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Picto__Reward[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       0,    0, // methods
       4,   14, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // properties: name, type, flags
       1, QMetaType::Int, 0x00095103,
       2, QMetaType::Int, 0x00095103,
       3, QMetaType::Int, 0x00095103,
       4, QMetaType::Int, 0x00095003,

       0        // eod
};

void Picto::Reward::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{

#ifndef QT_NO_PROPERTIES
    if (_c == QMetaObject::ReadProperty) {
        Reward *_t = static_cast<Reward *>(_o);
        Q_UNUSED(_t)
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< int*>(_v) = _t->getNumber(); break;
        case 1: *reinterpret_cast< int*>(_v) = _t->getUnitQuantity(); break;
        case 2: *reinterpret_cast< int*>(_v) = _t->getMinRewardPeriod(); break;
        case 3: *reinterpret_cast< int*>(_v) = _t->getRewardChan(); break;
        default: break;
        }
    } else if (_c == QMetaObject::WriteProperty) {
        Reward *_t = static_cast<Reward *>(_o);
        Q_UNUSED(_t)
        void *_v = _a[0];
        switch (_id) {
        case 0: _t->setNumber(*reinterpret_cast< int*>(_v)); break;
        case 1: _t->setUnitQuantity(*reinterpret_cast< int*>(_v)); break;
        case 2: _t->setMinRewardPeriod(*reinterpret_cast< int*>(_v)); break;
        case 3: _t->setRewardChan(*reinterpret_cast< int*>(_v)); break;
        default: break;
        }
    } else if (_c == QMetaObject::ResetProperty) {
    }
#endif // QT_NO_PROPERTIES
    Q_UNUSED(_o);
    Q_UNUSED(_id);
    Q_UNUSED(_c);
    Q_UNUSED(_a);
}

const QMetaObject Picto::Reward::staticMetaObject = {
    { &StateMachineElement::staticMetaObject, qt_meta_stringdata_Picto__Reward.data,
      qt_meta_data_Picto__Reward,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *Picto::Reward::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Picto::Reward::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_Picto__Reward.stringdata0))
        return static_cast<void*>(const_cast< Reward*>(this));
    return StateMachineElement::qt_metacast(_clname);
}

int Picto::Reward::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = StateMachineElement::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    
#ifndef QT_NO_PROPERTIES
   if (_c == QMetaObject::ReadProperty || _c == QMetaObject::WriteProperty
            || _c == QMetaObject::ResetProperty || _c == QMetaObject::RegisterPropertyMetaType) {
        qt_static_metacall(this, _c, _id, _a);
        _id -= 4;
    } else if (_c == QMetaObject::QueryPropertyDesignable) {
        _id -= 4;
    } else if (_c == QMetaObject::QueryPropertyScriptable) {
        _id -= 4;
    } else if (_c == QMetaObject::QueryPropertyStored) {
        _id -= 4;
    } else if (_c == QMetaObject::QueryPropertyEditable) {
        _id -= 4;
    } else if (_c == QMetaObject::QueryPropertyUser) {
        _id -= 4;
    }
#endif // QT_NO_PROPERTIES
    return _id;
}
QT_END_MOC_NAMESPACE
