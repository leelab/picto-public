/****************************************************************************
** Meta object code from reading C++ file 'AssetItem.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.5.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../source/common/designer/AssetItem.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'AssetItem.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.5.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_AssetItem_t {
    QByteArrayData data[6];
    char stringdata0[67];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_AssetItem_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_AssetItem_t qt_meta_stringdata_AssetItem = {
    {
QT_MOC_LITERAL(0, 0, 9), // "AssetItem"
QT_MOC_LITERAL(1, 10, 11), // "assetEdited"
QT_MOC_LITERAL(2, 22, 0), // ""
QT_MOC_LITERAL(3, 23, 15), // "searchRequested"
QT_MOC_LITERAL(4, 39, 13), // "SearchRequest"
QT_MOC_LITERAL(5, 53, 13) // "searchRequest"

    },
    "AssetItem\0assetEdited\0\0searchRequested\0"
    "SearchRequest\0searchRequest"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_AssetItem[] = {

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
       1,    0,   24,    2, 0x0a /* Public */,
       3,    1,   25,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 4,    5,

       0        // eod
};

void AssetItem::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        AssetItem *_t = static_cast<AssetItem *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->assetEdited(); break;
        case 1: _t->searchRequested((*reinterpret_cast< SearchRequest(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObject AssetItem::staticMetaObject = {
    { &DiagramItem::staticMetaObject, qt_meta_stringdata_AssetItem.data,
      qt_meta_data_AssetItem,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *AssetItem::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *AssetItem::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_AssetItem.stringdata0))
        return static_cast<void*>(const_cast< AssetItem*>(this));
    return DiagramItem::qt_metacast(_clname);
}

int AssetItem::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
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
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 2;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
