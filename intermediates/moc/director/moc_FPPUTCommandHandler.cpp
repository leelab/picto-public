/****************************************************************************
** Meta object code from reading C++ file 'FPPUTCommandHandler.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.5.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../source/director/frontpanel/FPPUTCommandHandler.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'FPPUTCommandHandler.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.5.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_FPPUTCommandHandler_t {
    QByteArrayData data[8];
    char stringdata0[89];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_FPPUTCommandHandler_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_FPPUTCommandHandler_t qt_meta_stringdata_FPPUTCommandHandler = {
    {
QT_MOC_LITERAL(0, 0, 19), // "FPPUTCommandHandler"
QT_MOC_LITERAL(1, 20, 7), // "setName"
QT_MOC_LITERAL(2, 28, 0), // ""
QT_MOC_LITERAL(3, 29, 4), // "name"
QT_MOC_LITERAL(4, 34, 17), // "setRewardDuration"
QT_MOC_LITERAL(5, 52, 10), // "controller"
QT_MOC_LITERAL(6, 63, 8), // "duration"
QT_MOC_LITERAL(7, 72, 16) // "setFlushDuration"

    },
    "FPPUTCommandHandler\0setName\0\0name\0"
    "setRewardDuration\0controller\0duration\0"
    "setFlushDuration"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_FPPUTCommandHandler[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       3,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       3,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   29,    2, 0x06 /* Public */,
       4,    2,   32,    2, 0x06 /* Public */,
       7,    2,   37,    2, 0x06 /* Public */,

 // signals: parameters
    QMetaType::Void, QMetaType::QString,    3,
    QMetaType::Void, QMetaType::Int, QMetaType::Int,    5,    6,
    QMetaType::Void, QMetaType::Int, QMetaType::Int,    5,    6,

       0        // eod
};

void FPPUTCommandHandler::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        FPPUTCommandHandler *_t = static_cast<FPPUTCommandHandler *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->setName((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 1: _t->setRewardDuration((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 2: _t->setFlushDuration((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (FPPUTCommandHandler::*_t)(QString );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&FPPUTCommandHandler::setName)) {
                *result = 0;
            }
        }
        {
            typedef void (FPPUTCommandHandler::*_t)(int , int );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&FPPUTCommandHandler::setRewardDuration)) {
                *result = 1;
            }
        }
        {
            typedef void (FPPUTCommandHandler::*_t)(int , int );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&FPPUTCommandHandler::setFlushDuration)) {
                *result = 2;
            }
        }
    }
}

const QMetaObject FPPUTCommandHandler::staticMetaObject = {
    { &Picto::ProtocolCommandHandler::staticMetaObject, qt_meta_stringdata_FPPUTCommandHandler.data,
      qt_meta_data_FPPUTCommandHandler,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *FPPUTCommandHandler::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *FPPUTCommandHandler::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_FPPUTCommandHandler.stringdata0))
        return static_cast<void*>(const_cast< FPPUTCommandHandler*>(this));
    return Picto::ProtocolCommandHandler::qt_metacast(_clname);
}

int FPPUTCommandHandler::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = Picto::ProtocolCommandHandler::qt_metacall(_c, _id, _a);
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
    return _id;
}

// SIGNAL 0
void FPPUTCommandHandler::setName(QString _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void FPPUTCommandHandler::setRewardDuration(int _t1, int _t2)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void FPPUTCommandHandler::setFlushDuration(int _t1, int _t2)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}
QT_END_MOC_NAMESPACE
