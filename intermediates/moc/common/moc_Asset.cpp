/****************************************************************************
** Meta object code from reading C++ file 'Asset.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.5.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../source/common/storage/Asset.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'Asset.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.5.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_Picto__Asset_t {
    QByteArrayData data[7];
    char stringdata0[86];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Picto__Asset_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Picto__Asset_t qt_meta_stringdata_Picto__Asset = {
    {
QT_MOC_LITERAL(0, 0, 12), // "Picto::Asset"
QT_MOC_LITERAL(1, 13, 6), // "edited"
QT_MOC_LITERAL(2, 20, 0), // ""
QT_MOC_LITERAL(3, 21, 7), // "deleted"
QT_MOC_LITERAL(4, 29, 13), // "assetIdEdited"
QT_MOC_LITERAL(5, 43, 20), // "receivedEditedSignal"
QT_MOC_LITERAL(6, 64, 21) // "receivedDeletedSignal"

    },
    "Picto::Asset\0edited\0\0deleted\0assetIdEdited\0"
    "receivedEditedSignal\0receivedDeletedSignal"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Picto__Asset[] = {

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
       1,    0,   39,    2, 0x06 /* Public */,
       3,    0,   40,    2, 0x06 /* Public */,
       4,    0,   41,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       5,    0,   42,    2, 0x08 /* Private */,
       6,    0,   43,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void Picto::Asset::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Asset *_t = static_cast<Asset *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->edited(); break;
        case 1: _t->deleted(); break;
        case 2: _t->assetIdEdited(); break;
        case 3: _t->receivedEditedSignal(); break;
        case 4: _t->receivedDeletedSignal(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (Asset::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&Asset::edited)) {
                *result = 0;
            }
        }
        {
            typedef void (Asset::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&Asset::deleted)) {
                *result = 1;
            }
        }
        {
            typedef void (Asset::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&Asset::assetIdEdited)) {
                *result = 2;
            }
        }
    }
    Q_UNUSED(_a);
}

const QMetaObject Picto::Asset::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_Picto__Asset.data,
      qt_meta_data_Picto__Asset,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *Picto::Asset::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Picto::Asset::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_Picto__Asset.stringdata0))
        return static_cast<void*>(const_cast< Asset*>(this));
    if (!strcmp(_clname, "Serializable"))
        return static_cast< Serializable*>(const_cast< Asset*>(this));
    return QObject::qt_metacast(_clname);
}

int Picto::Asset::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
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
void Picto::Asset::edited()
{
    QMetaObject::activate(this, &staticMetaObject, 0, Q_NULLPTR);
}

// SIGNAL 1
void Picto::Asset::deleted()
{
    QMetaObject::activate(this, &staticMetaObject, 1, Q_NULLPTR);
}

// SIGNAL 2
void Picto::Asset::assetIdEdited()
{
    QMetaObject::activate(this, &staticMetaObject, 2, Q_NULLPTR);
}
QT_END_MOC_NAMESPACE
