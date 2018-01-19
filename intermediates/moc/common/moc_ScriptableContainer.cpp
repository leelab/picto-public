/****************************************************************************
** Meta object code from reading C++ file 'ScriptableContainer.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.5.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../source/common/statemachine/ScriptableContainer.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#include <QtCore/QSharedPointer>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'ScriptableContainer.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.5.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_Picto__ScriptableContainer_t {
    QByteArrayData data[9];
    char stringdata0[118];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Picto__ScriptableContainer_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Picto__ScriptableContainer_t qt_meta_stringdata_Picto__ScriptableContainer = {
    {
QT_MOC_LITERAL(0, 0, 26), // "Picto::ScriptableContainer"
QT_MOC_LITERAL(1, 27, 15), // "deinitScripting"
QT_MOC_LITERAL(2, 43, 0), // ""
QT_MOC_LITERAL(3, 44, 9), // "Property*"
QT_MOC_LITERAL(4, 54, 4), // "prop"
QT_MOC_LITERAL(5, 59, 5), // "value"
QT_MOC_LITERAL(6, 65, 21), // "addChildToScriptLists"
QT_MOC_LITERAL(7, 87, 21), // "QSharedPointer<Asset>"
QT_MOC_LITERAL(8, 109, 8) // "newChild"

    },
    "Picto::ScriptableContainer\0deinitScripting\0"
    "\0Property*\0prop\0value\0addChildToScriptLists\0"
    "QSharedPointer<Asset>\0newChild"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Picto__ScriptableContainer[] = {

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
       1,    0,   29,    2, 0x08 /* Private */,
       1,    2,   30,    2, 0x08 /* Private */,
       6,    1,   35,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 3, QMetaType::QVariant,    4,    5,
    QMetaType::Void, 0x80000000 | 7,    8,

       0        // eod
};

void Picto::ScriptableContainer::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        ScriptableContainer *_t = static_cast<ScriptableContainer *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->deinitScripting(); break;
        case 1: _t->deinitScripting((*reinterpret_cast< Property*(*)>(_a[1])),(*reinterpret_cast< QVariant(*)>(_a[2]))); break;
        case 2: _t->addChildToScriptLists((*reinterpret_cast< QSharedPointer<Asset>(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<int*>(_a[0]) = -1; break;
        case 1:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< Property* >(); break;
            }
            break;
        case 2:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QSharedPointer<Asset> >(); break;
            }
            break;
        }
    }
}

const QMetaObject Picto::ScriptableContainer::staticMetaObject = {
    { &Scriptable::staticMetaObject, qt_meta_stringdata_Picto__ScriptableContainer.data,
      qt_meta_data_Picto__ScriptableContainer,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *Picto::ScriptableContainer::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Picto::ScriptableContainer::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_Picto__ScriptableContainer.stringdata0))
        return static_cast<void*>(const_cast< ScriptableContainer*>(this));
    return Scriptable::qt_metacast(_clname);
}

int Picto::ScriptableContainer::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = Scriptable::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 3)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 3;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 3)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 3;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
