/****************************************************************************
** Meta object code from reading C++ file 'menu.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.5.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../source/embedded/frontpanel/menu.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'menu.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.5.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_Menu_t {
    QByteArrayData data[12];
    char stringdata0[121];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Menu_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Menu_t qt_meta_stringdata_Menu = {
    {
QT_MOC_LITERAL(0, 0, 4), // "Menu"
QT_MOC_LITERAL(1, 5, 9), // "updateLCD"
QT_MOC_LITERAL(2, 15, 0), // ""
QT_MOC_LITERAL(3, 16, 4), // "line"
QT_MOC_LITERAL(4, 21, 4), // "text"
QT_MOC_LITERAL(5, 26, 15), // "toggleBacklight"
QT_MOC_LITERAL(6, 42, 15), // "turnOnBacklight"
QT_MOC_LITERAL(7, 58, 16), // "turnOffBacklight"
QT_MOC_LITERAL(8, 75, 13), // "userInputSlot"
QT_MOC_LITERAL(9, 89, 4), // "type"
QT_MOC_LITERAL(10, 94, 11), // "aboutToQuit"
QT_MOC_LITERAL(11, 106, 14) // "returnToStatus"

    },
    "Menu\0updateLCD\0\0line\0text\0toggleBacklight\0"
    "turnOnBacklight\0turnOffBacklight\0"
    "userInputSlot\0type\0aboutToQuit\0"
    "returnToStatus"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Menu[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       7,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       4,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    2,   49,    2, 0x06 /* Public */,
       5,    0,   54,    2, 0x06 /* Public */,
       6,    0,   55,    2, 0x06 /* Public */,
       7,    0,   56,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       8,    1,   57,    2, 0x0a /* Public */,
      10,    0,   60,    2, 0x0a /* Public */,
      11,    0,   61,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void, QMetaType::Int, QMetaType::QString,    3,    4,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

 // slots: parameters
    QMetaType::Void, QMetaType::Int,    9,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void Menu::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Menu *_t = static_cast<Menu *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->updateLCD((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2]))); break;
        case 1: _t->toggleBacklight(); break;
        case 2: _t->turnOnBacklight(); break;
        case 3: _t->turnOffBacklight(); break;
        case 4: _t->userInputSlot((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 5: _t->aboutToQuit(); break;
        case 6: _t->returnToStatus(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (Menu::*_t)(int , QString );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&Menu::updateLCD)) {
                *result = 0;
            }
        }
        {
            typedef void (Menu::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&Menu::toggleBacklight)) {
                *result = 1;
            }
        }
        {
            typedef void (Menu::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&Menu::turnOnBacklight)) {
                *result = 2;
            }
        }
        {
            typedef void (Menu::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&Menu::turnOffBacklight)) {
                *result = 3;
            }
        }
    }
}

const QMetaObject Menu::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_Menu.data,
      qt_meta_data_Menu,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *Menu::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Menu::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_Menu.stringdata0))
        return static_cast<void*>(const_cast< Menu*>(this));
    return QObject::qt_metacast(_clname);
}

int Menu::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 7)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 7;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 7)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 7;
    }
    return _id;
}

// SIGNAL 0
void Menu::updateLCD(int _t1, QString _t2)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void Menu::toggleBacklight()
{
    QMetaObject::activate(this, &staticMetaObject, 1, Q_NULLPTR);
}

// SIGNAL 2
void Menu::turnOnBacklight()
{
    QMetaObject::activate(this, &staticMetaObject, 2, Q_NULLPTR);
}

// SIGNAL 3
void Menu::turnOffBacklight()
{
    QMetaObject::activate(this, &staticMetaObject, 3, Q_NULLPTR);
}
QT_END_MOC_NAMESPACE
