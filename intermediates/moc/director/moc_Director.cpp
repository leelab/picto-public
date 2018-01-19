/****************************************************************************
** Meta object code from reading C++ file 'Director.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.5.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../source/director/Director.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'Director.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.5.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_Director_t {
    QByteArrayData data[8];
    char stringdata0[87];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Director_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Director_t qt_meta_stringdata_Director = {
    {
QT_MOC_LITERAL(0, 0, 8), // "Director"
QT_MOC_LITERAL(1, 9, 10), // "changeName"
QT_MOC_LITERAL(2, 20, 0), // ""
QT_MOC_LITERAL(3, 21, 4), // "name"
QT_MOC_LITERAL(4, 26, 20), // "changeRewardDuration"
QT_MOC_LITERAL(5, 47, 10), // "controller"
QT_MOC_LITERAL(6, 58, 8), // "duration"
QT_MOC_LITERAL(7, 67, 19) // "changeFlushDuration"

    },
    "Director\0changeName\0\0name\0"
    "changeRewardDuration\0controller\0"
    "duration\0changeFlushDuration"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Director[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       3,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    1,   29,    2, 0x08 /* Private */,
       4,    2,   32,    2, 0x08 /* Private */,
       7,    2,   37,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void, QMetaType::QString,    3,
    QMetaType::Void, QMetaType::Int, QMetaType::Int,    5,    6,
    QMetaType::Void, QMetaType::Int, QMetaType::Int,    5,    6,

       0        // eod
};

void Director::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Director *_t = static_cast<Director *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->changeName((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 1: _t->changeRewardDuration((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 2: _t->changeFlushDuration((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        default: ;
        }
    }
}

const QMetaObject Director::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_Director.data,
      qt_meta_data_Director,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *Director::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Director::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_Director.stringdata0))
        return static_cast<void*>(const_cast< Director*>(this));
    if (!strcmp(_clname, "ComponentInterface"))
        return static_cast< ComponentInterface*>(const_cast< Director*>(this));
    return QObject::qt_metacast(_clname);
}

int Director::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
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
QT_END_MOC_NAMESPACE
