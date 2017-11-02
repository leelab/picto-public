/****************************************************************************
** Meta object code from reading C++ file 'displaymode.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.5.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../source/embedded/frontpanel/displaymode.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'displaymode.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.5.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_DisplayMode_t {
    QByteArrayData data[12];
    char stringdata0[137];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_DisplayMode_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_DisplayMode_t qt_meta_stringdata_DisplayMode = {
    {
QT_MOC_LITERAL(0, 0, 11), // "DisplayMode"
QT_MOC_LITERAL(1, 12, 9), // "updateLCD"
QT_MOC_LITERAL(2, 22, 0), // ""
QT_MOC_LITERAL(3, 23, 4), // "line"
QT_MOC_LITERAL(4, 28, 4), // "text"
QT_MOC_LITERAL(5, 33, 15), // "toggleBacklight"
QT_MOC_LITERAL(6, 49, 15), // "turnOnBacklight"
QT_MOC_LITERAL(7, 65, 13), // "userInputSlot"
QT_MOC_LITERAL(8, 79, 26), // "PanelInfo::DisplayModeType"
QT_MOC_LITERAL(9, 106, 9), // "InputType"
QT_MOC_LITERAL(10, 116, 4), // "type"
QT_MOC_LITERAL(11, 121, 15) // "messageTimedOut"

    },
    "DisplayMode\0updateLCD\0\0line\0text\0"
    "toggleBacklight\0turnOnBacklight\0"
    "userInputSlot\0PanelInfo::DisplayModeType\0"
    "InputType\0type\0messageTimedOut"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_DisplayMode[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       5,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       3,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    2,   39,    2, 0x06 /* Public */,
       5,    0,   44,    2, 0x06 /* Public */,
       6,    0,   45,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       7,    1,   46,    2, 0x0a /* Public */,
      11,    0,   49,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void, QMetaType::Int, QMetaType::QString,    3,    4,
    QMetaType::Void,
    QMetaType::Void,

 // slots: parameters
    0x80000000 | 8, 0x80000000 | 9,   10,
    QMetaType::Void,

       0        // eod
};

void DisplayMode::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        DisplayMode *_t = static_cast<DisplayMode *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->updateLCD((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2]))); break;
        case 1: _t->toggleBacklight(); break;
        case 2: _t->turnOnBacklight(); break;
        case 3: { PanelInfo::DisplayModeType _r = _t->userInputSlot((*reinterpret_cast< InputType(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< PanelInfo::DisplayModeType*>(_a[0]) = _r; }  break;
        case 4: _t->messageTimedOut(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (DisplayMode::*_t)(int , QString );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&DisplayMode::updateLCD)) {
                *result = 0;
            }
        }
        {
            typedef void (DisplayMode::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&DisplayMode::toggleBacklight)) {
                *result = 1;
            }
        }
        {
            typedef void (DisplayMode::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&DisplayMode::turnOnBacklight)) {
                *result = 2;
            }
        }
    }
}

const QMetaObject DisplayMode::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_DisplayMode.data,
      qt_meta_data_DisplayMode,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *DisplayMode::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *DisplayMode::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_DisplayMode.stringdata0))
        return static_cast<void*>(const_cast< DisplayMode*>(this));
    return QObject::qt_metacast(_clname);
}

int DisplayMode::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 5)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 5;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 5)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 5;
    }
    return _id;
}

// SIGNAL 0
void DisplayMode::updateLCD(int _t1, QString _t2)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void DisplayMode::toggleBacklight()
{
    QMetaObject::activate(this, &staticMetaObject, 1, Q_NULLPTR);
}

// SIGNAL 2
void DisplayMode::turnOnBacklight()
{
    QMetaObject::activate(this, &staticMetaObject, 2, Q_NULLPTR);
}
QT_END_MOC_NAMESPACE
