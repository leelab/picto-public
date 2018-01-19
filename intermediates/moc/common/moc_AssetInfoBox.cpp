/****************************************************************************
** Meta object code from reading C++ file 'AssetInfoBox.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.5.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../source/common/designer/AssetInfoBox.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#include <QtCore/QSharedPointer>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'AssetInfoBox.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.5.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_AssetInfoBox_t {
    QByteArrayData data[8];
    char stringdata0[100];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_AssetInfoBox_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_AssetInfoBox_t qt_meta_stringdata_AssetInfoBox = {
    {
QT_MOC_LITERAL(0, 0, 12), // "AssetInfoBox"
QT_MOC_LITERAL(1, 13, 13), // "assetSelected"
QT_MOC_LITERAL(2, 27, 0), // ""
QT_MOC_LITERAL(3, 28, 21), // "QSharedPointer<Asset>"
QT_MOC_LITERAL(4, 50, 5), // "asset"
QT_MOC_LITERAL(5, 56, 15), // "searchRequested"
QT_MOC_LITERAL(6, 72, 13), // "SearchRequest"
QT_MOC_LITERAL(7, 86, 13) // "searchRequest"

    },
    "AssetInfoBox\0assetSelected\0\0"
    "QSharedPointer<Asset>\0asset\0searchRequested\0"
    "SearchRequest\0searchRequest"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_AssetInfoBox[] = {

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
       1,    1,   24,    2, 0x0a /* Public */,
       5,    1,   27,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void, 0x80000000 | 3,    4,
    QMetaType::Void, 0x80000000 | 6,    7,

       0        // eod
};

void AssetInfoBox::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        AssetInfoBox *_t = static_cast<AssetInfoBox *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->assetSelected((*reinterpret_cast< QSharedPointer<Asset>(*)>(_a[1]))); break;
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
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QSharedPointer<Asset> >(); break;
            }
            break;
        }
    }
}

const QMetaObject AssetInfoBox::staticMetaObject = {
    { &SearchableTextEdit::staticMetaObject, qt_meta_stringdata_AssetInfoBox.data,
      qt_meta_data_AssetInfoBox,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *AssetInfoBox::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *AssetInfoBox::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_AssetInfoBox.stringdata0))
        return static_cast<void*>(const_cast< AssetInfoBox*>(this));
    return SearchableTextEdit::qt_metacast(_clname);
}

int AssetInfoBox::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = SearchableTextEdit::qt_metacall(_c, _id, _a);
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
