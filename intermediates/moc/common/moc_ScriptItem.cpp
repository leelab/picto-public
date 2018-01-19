/****************************************************************************
** Meta object code from reading C++ file 'ScriptItem.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.5.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../source/common/designer/ScriptItem.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'ScriptItem.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.5.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_ScriptItem_t {
    QByteArrayData data[9];
    char stringdata0[104];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_ScriptItem_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_ScriptItem_t qt_meta_stringdata_ScriptItem = {
    {
QT_MOC_LITERAL(0, 0, 10), // "ScriptItem"
QT_MOC_LITERAL(1, 11, 16), // "scriptPropEdited"
QT_MOC_LITERAL(2, 28, 0), // ""
QT_MOC_LITERAL(3, 29, 9), // "Property*"
QT_MOC_LITERAL(4, 39, 11), // "changedProp"
QT_MOC_LITERAL(5, 51, 8), // "newValue"
QT_MOC_LITERAL(6, 60, 15), // "searchRequested"
QT_MOC_LITERAL(7, 76, 13), // "SearchRequest"
QT_MOC_LITERAL(8, 90, 13) // "searchRequest"

    },
    "ScriptItem\0scriptPropEdited\0\0Property*\0"
    "changedProp\0newValue\0searchRequested\0"
    "SearchRequest\0searchRequest"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_ScriptItem[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       2,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    2,   24,    2, 0x08 /* Private */,
       6,    1,   29,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void, 0x80000000 | 3, QMetaType::QVariant,    4,    5,
    QMetaType::Void, 0x80000000 | 7,    8,

       0        // eod
};

void ScriptItem::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        ScriptItem *_t = static_cast<ScriptItem *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->scriptPropEdited((*reinterpret_cast< Property*(*)>(_a[1])),(*reinterpret_cast< QVariant(*)>(_a[2]))); break;
        case 1: _t->searchRequested((*reinterpret_cast< SearchRequest(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<int*>(_a[0]) = -1; break;
        case 0:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< Property* >(); break;
            }
            break;
        }
    }
}

const QMetaObject ScriptItem::staticMetaObject = {
    { &DiagramItem::staticMetaObject, qt_meta_stringdata_ScriptItem.data,
      qt_meta_data_ScriptItem,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *ScriptItem::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *ScriptItem::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_ScriptItem.stringdata0))
        return static_cast<void*>(const_cast< ScriptItem*>(this));
    return DiagramItem::qt_metacast(_clname);
}

int ScriptItem::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = DiagramItem::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 2)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 2;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 2)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 2;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
