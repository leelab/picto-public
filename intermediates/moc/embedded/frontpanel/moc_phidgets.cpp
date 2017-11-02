/****************************************************************************
** Meta object code from reading C++ file 'phidgets.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.5.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../source/embedded/frontpanel/phidgets.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'phidgets.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.5.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_Phidgets_t {
    QByteArrayData data[11];
    char stringdata0[117];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Phidgets_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Phidgets_t qt_meta_stringdata_Phidgets = {
    {
QT_MOC_LITERAL(0, 0, 8), // "Phidgets"
QT_MOC_LITERAL(1, 9, 15), // "userInputSignal"
QT_MOC_LITERAL(2, 25, 0), // ""
QT_MOC_LITERAL(3, 26, 4), // "type"
QT_MOC_LITERAL(4, 31, 9), // "updateLCD"
QT_MOC_LITERAL(5, 41, 4), // "line"
QT_MOC_LITERAL(6, 46, 4), // "text"
QT_MOC_LITERAL(7, 51, 15), // "toggleBacklight"
QT_MOC_LITERAL(8, 67, 15), // "turnOnBacklight"
QT_MOC_LITERAL(9, 83, 16), // "turnOffBacklight"
QT_MOC_LITERAL(10, 100, 16) // "turnWasTriggered"

    },
    "Phidgets\0userInputSignal\0\0type\0updateLCD\0"
    "line\0text\0toggleBacklight\0turnOnBacklight\0"
    "turnOffBacklight\0turnWasTriggered"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Phidgets[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       6,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   44,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       4,    2,   47,    2, 0x0a /* Public */,
       7,    0,   52,    2, 0x0a /* Public */,
       8,    0,   53,    2, 0x0a /* Public */,
       9,    0,   54,    2, 0x0a /* Public */,
      10,    1,   55,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void, QMetaType::Int,    3,

 // slots: parameters
    QMetaType::Void, QMetaType::Int, QMetaType::QString,    5,    6,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,    2,

       0        // eod
};

void Phidgets::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Phidgets *_t = static_cast<Phidgets *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->userInputSignal((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 1: _t->updateLCD((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2]))); break;
        case 2: _t->toggleBacklight(); break;
        case 3: _t->turnOnBacklight(); break;
        case 4: _t->turnOffBacklight(); break;
        case 5: _t->turnWasTriggered((*reinterpret_cast< int(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (Phidgets::*_t)(int );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&Phidgets::userInputSignal)) {
                *result = 0;
            }
        }
    }
}

const QMetaObject Phidgets::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_Phidgets.data,
      qt_meta_data_Phidgets,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *Phidgets::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Phidgets::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_Phidgets.stringdata0))
        return static_cast<void*>(const_cast< Phidgets*>(this));
    return QObject::qt_metacast(_clname);
}

int Phidgets::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 6)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 6;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 6)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 6;
    }
    return _id;
}

// SIGNAL 0
void Phidgets::userInputSignal(int _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_END_MOC_NAMESPACE
