/****************************************************************************
** Meta object code from reading C++ file 'ControlPanelInterface.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.5.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../source/common/engine/ControlPanelInterface.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'ControlPanelInterface.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.5.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_ControlPanelInterface_t {
    QByteArrayData data[23];
    char stringdata0[329];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_ControlPanelInterface_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_ControlPanelInterface_t qt_meta_stringdata_ControlPanelInterface = {
    {
QT_MOC_LITERAL(0, 0, 21), // "ControlPanelInterface"
QT_MOC_LITERAL(1, 22, 17), // "nameChangeRequest"
QT_MOC_LITERAL(2, 40, 0), // ""
QT_MOC_LITERAL(3, 41, 4), // "name"
QT_MOC_LITERAL(4, 46, 27), // "rewardDurationChangeRequest"
QT_MOC_LITERAL(5, 74, 10), // "controller"
QT_MOC_LITERAL(6, 85, 8), // "duration"
QT_MOC_LITERAL(7, 94, 26), // "flushDurationChangeRequest"
QT_MOC_LITERAL(8, 121, 12), // "flushRequest"
QT_MOC_LITERAL(9, 134, 17), // "giveRewardRequest"
QT_MOC_LITERAL(10, 152, 11), // "nameChanged"
QT_MOC_LITERAL(11, 164, 16), // "ipAddressChanged"
QT_MOC_LITERAL(12, 181, 4), // "addr"
QT_MOC_LITERAL(13, 186, 21), // "rewardDurationChanged"
QT_MOC_LITERAL(14, 208, 20), // "flushDurationChanged"
QT_MOC_LITERAL(15, 229, 13), // "statusChanged"
QT_MOC_LITERAL(16, 243, 6), // "status"
QT_MOC_LITERAL(17, 250, 18), // "doIncomingCommands"
QT_MOC_LITERAL(18, 269, 7), // "setName"
QT_MOC_LITERAL(19, 277, 17), // "setRewardDuration"
QT_MOC_LITERAL(20, 295, 16), // "setFlushDuration"
QT_MOC_LITERAL(21, 312, 5), // "flush"
QT_MOC_LITERAL(22, 318, 10) // "giveReward"

    },
    "ControlPanelInterface\0nameChangeRequest\0"
    "\0name\0rewardDurationChangeRequest\0"
    "controller\0duration\0flushDurationChangeRequest\0"
    "flushRequest\0giveRewardRequest\0"
    "nameChanged\0ipAddressChanged\0addr\0"
    "rewardDurationChanged\0flushDurationChanged\0"
    "statusChanged\0status\0doIncomingCommands\0"
    "setName\0setRewardDuration\0setFlushDuration\0"
    "flush\0giveReward"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_ControlPanelInterface[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
      16,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       5,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   94,    2, 0x06 /* Public */,
       4,    2,   97,    2, 0x06 /* Public */,
       7,    2,  102,    2, 0x06 /* Public */,
       8,    1,  107,    2, 0x06 /* Public */,
       9,    1,  110,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
      10,    1,  113,    2, 0x0a /* Public */,
      11,    1,  116,    2, 0x0a /* Public */,
      13,    2,  119,    2, 0x0a /* Public */,
      14,    2,  124,    2, 0x0a /* Public */,
      15,    1,  129,    2, 0x0a /* Public */,
      17,    0,  132,    2, 0x0a /* Public */,
      18,    1,  133,    2, 0x09 /* Protected */,
      19,    2,  136,    2, 0x09 /* Protected */,
      20,    2,  141,    2, 0x09 /* Protected */,
      21,    1,  146,    2, 0x09 /* Protected */,
      22,    1,  149,    2, 0x09 /* Protected */,

 // signals: parameters
    QMetaType::Void, QMetaType::QString,    3,
    QMetaType::Void, QMetaType::Int, QMetaType::Int,    5,    6,
    QMetaType::Void, QMetaType::Int, QMetaType::Int,    5,    6,
    QMetaType::Void, QMetaType::Int,    5,
    QMetaType::Void, QMetaType::Int,    5,

 // slots: parameters
    QMetaType::Void, QMetaType::QString,    3,
    QMetaType::Void, QMetaType::QString,   12,
    QMetaType::Void, QMetaType::Int, QMetaType::Int,    5,    6,
    QMetaType::Void, QMetaType::Int, QMetaType::Int,    5,    6,
    QMetaType::Void, QMetaType::QString,   16,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,    3,
    QMetaType::Void, QMetaType::Int, QMetaType::Int,    5,    6,
    QMetaType::Void, QMetaType::Int, QMetaType::Int,    5,    6,
    QMetaType::Void, QMetaType::Int,    5,
    QMetaType::Void, QMetaType::Int,    5,

       0        // eod
};

void ControlPanelInterface::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        ControlPanelInterface *_t = static_cast<ControlPanelInterface *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->nameChangeRequest((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 1: _t->rewardDurationChangeRequest((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 2: _t->flushDurationChangeRequest((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 3: _t->flushRequest((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 4: _t->giveRewardRequest((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 5: _t->nameChanged((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 6: _t->ipAddressChanged((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 7: _t->rewardDurationChanged((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 8: _t->flushDurationChanged((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 9: _t->statusChanged((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 10: _t->doIncomingCommands(); break;
        case 11: _t->setName((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 12: _t->setRewardDuration((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 13: _t->setFlushDuration((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 14: _t->flush((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 15: _t->giveReward((*reinterpret_cast< int(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (ControlPanelInterface::*_t)(QString );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&ControlPanelInterface::nameChangeRequest)) {
                *result = 0;
            }
        }
        {
            typedef void (ControlPanelInterface::*_t)(int , int );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&ControlPanelInterface::rewardDurationChangeRequest)) {
                *result = 1;
            }
        }
        {
            typedef void (ControlPanelInterface::*_t)(int , int );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&ControlPanelInterface::flushDurationChangeRequest)) {
                *result = 2;
            }
        }
        {
            typedef void (ControlPanelInterface::*_t)(int );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&ControlPanelInterface::flushRequest)) {
                *result = 3;
            }
        }
        {
            typedef void (ControlPanelInterface::*_t)(int );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&ControlPanelInterface::giveRewardRequest)) {
                *result = 4;
            }
        }
    }
}

const QMetaObject ControlPanelInterface::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_ControlPanelInterface.data,
      qt_meta_data_ControlPanelInterface,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *ControlPanelInterface::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *ControlPanelInterface::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_ControlPanelInterface.stringdata0))
        return static_cast<void*>(const_cast< ControlPanelInterface*>(this));
    return QObject::qt_metacast(_clname);
}

int ControlPanelInterface::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 16)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 16;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 16)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 16;
    }
    return _id;
}

// SIGNAL 0
void ControlPanelInterface::nameChangeRequest(QString _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void ControlPanelInterface::rewardDurationChangeRequest(int _t1, int _t2)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void ControlPanelInterface::flushDurationChangeRequest(int _t1, int _t2)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void ControlPanelInterface::flushRequest(int _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}

// SIGNAL 4
void ControlPanelInterface::giveRewardRequest(int _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 4, _a);
}
QT_END_MOC_NAMESPACE
