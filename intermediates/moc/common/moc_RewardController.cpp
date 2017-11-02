/****************************************************************************
** Meta object code from reading C++ file 'RewardController.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.5.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../source/common/iodevices/RewardController.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'RewardController.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.5.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_Picto__RewardController_t {
    QByteArrayData data[15];
    char stringdata0[176];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Picto__RewardController_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Picto__RewardController_t qt_meta_stringdata_Picto__RewardController = {
    {
QT_MOC_LITERAL(0, 0, 23), // "Picto::RewardController"
QT_MOC_LITERAL(1, 24, 8), // "rewarded"
QT_MOC_LITERAL(2, 33, 0), // ""
QT_MOC_LITERAL(3, 34, 8), // "quantity"
QT_MOC_LITERAL(4, 43, 9), // "addReward"
QT_MOC_LITERAL(5, 53, 7), // "channel"
QT_MOC_LITERAL(6, 61, 15), // "minRewardPeriod"
QT_MOC_LITERAL(7, 77, 14), // "triggerRewards"
QT_MOC_LITERAL(8, 92, 12), // "appendToList"
QT_MOC_LITERAL(9, 105, 16), // "rewardInProgress"
QT_MOC_LITERAL(10, 122, 17), // "hasPendingRewards"
QT_MOC_LITERAL(11, 140, 5), // "flush"
QT_MOC_LITERAL(12, 146, 7), // "seconds"
QT_MOC_LITERAL(13, 154, 10), // "isFlushing"
QT_MOC_LITERAL(14, 165, 10) // "abortFlush"

    },
    "Picto::RewardController\0rewarded\0\0"
    "quantity\0addReward\0channel\0minRewardPeriod\0"
    "triggerRewards\0appendToList\0"
    "rewardInProgress\0hasPendingRewards\0"
    "flush\0seconds\0isFlushing\0abortFlush"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Picto__RewardController[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       9,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   59,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       4,    3,   62,    2, 0x0a /* Public */,
       7,    1,   69,    2, 0x0a /* Public */,
       9,    1,   72,    2, 0x0a /* Public */,
      10,    0,   75,    2, 0x0a /* Public */,
      10,    1,   76,    2, 0x0a /* Public */,
      11,    2,   79,    2, 0x0a /* Public */,
      13,    1,   84,    2, 0x0a /* Public */,
      14,    1,   87,    2, 0x0a /* Public */,

 // signals: parameters
    QMetaType::Void, QMetaType::Int,    3,

 // slots: parameters
    QMetaType::Void, QMetaType::UInt, QMetaType::Int, QMetaType::Int,    5,    3,    6,
    QMetaType::Void, QMetaType::Bool,    8,
    QMetaType::Bool, QMetaType::UInt,    5,
    QMetaType::Bool,
    QMetaType::Bool, QMetaType::UInt,    5,
    QMetaType::Void, QMetaType::UInt, QMetaType::Int,    5,   12,
    QMetaType::Bool, QMetaType::UInt,    5,
    QMetaType::Void, QMetaType::UInt,    5,

       0        // eod
};

void Picto::RewardController::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        RewardController *_t = static_cast<RewardController *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->rewarded((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 1: _t->addReward((*reinterpret_cast< uint(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3]))); break;
        case 2: _t->triggerRewards((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 3: { bool _r = _t->rewardInProgress((*reinterpret_cast< uint(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = _r; }  break;
        case 4: { bool _r = _t->hasPendingRewards();
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = _r; }  break;
        case 5: { bool _r = _t->hasPendingRewards((*reinterpret_cast< uint(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = _r; }  break;
        case 6: _t->flush((*reinterpret_cast< uint(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 7: { bool _r = _t->isFlushing((*reinterpret_cast< uint(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = _r; }  break;
        case 8: _t->abortFlush((*reinterpret_cast< uint(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (RewardController::*_t)(int );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&RewardController::rewarded)) {
                *result = 0;
            }
        }
    }
}

const QMetaObject Picto::RewardController::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_Picto__RewardController.data,
      qt_meta_data_Picto__RewardController,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *Picto::RewardController::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Picto::RewardController::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_Picto__RewardController.stringdata0))
        return static_cast<void*>(const_cast< RewardController*>(this));
    return QObject::qt_metacast(_clname);
}

int Picto::RewardController::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 9)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 9;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 9)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 9;
    }
    return _id;
}

// SIGNAL 0
void Picto::RewardController::rewarded(int _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_END_MOC_NAMESPACE
