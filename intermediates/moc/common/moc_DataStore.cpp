/****************************************************************************
** Meta object code from reading C++ file 'DataStore.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.5.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../source/common/storage/DataStore.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#include <QtCore/QSharedPointer>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'DataStore.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.5.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_Picto__DataStore_t {
    QByteArrayData data[7];
    char stringdata0[109];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Picto__DataStore_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Picto__DataStore_t qt_meta_stringdata_Picto__DataStore = {
    {
QT_MOC_LITERAL(0, 0, 16), // "Picto::DataStore"
QT_MOC_LITERAL(1, 17, 26), // "childAddedAfterDeserialize"
QT_MOC_LITERAL(2, 44, 0), // ""
QT_MOC_LITERAL(3, 45, 21), // "QSharedPointer<Asset>"
QT_MOC_LITERAL(4, 67, 8), // "newChild"
QT_MOC_LITERAL(5, 76, 20), // "associateChildEdited"
QT_MOC_LITERAL(6, 97, 11) // "childEdited"

    },
    "Picto::DataStore\0childAddedAfterDeserialize\0"
    "\0QSharedPointer<Asset>\0newChild\0"
    "associateChildEdited\0childEdited"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Picto__DataStore[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       3,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   29,    2, 0x06 /* Public */,
       5,    0,   32,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       6,    0,   33,    2, 0x0a /* Public */,

 // signals: parameters
    QMetaType::Void, 0x80000000 | 3,    4,
    QMetaType::Void,

 // slots: parameters
    QMetaType::Void,

       0        // eod
};

void Picto::DataStore::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        DataStore *_t = static_cast<DataStore *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->childAddedAfterDeserialize((*reinterpret_cast< QSharedPointer<Asset>(*)>(_a[1]))); break;
        case 1: _t->associateChildEdited(); break;
        case 2: _t->childEdited(); break;
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
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (DataStore::*_t)(QSharedPointer<Asset> );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&DataStore::childAddedAfterDeserialize)) {
                *result = 0;
            }
        }
        {
            typedef void (DataStore::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&DataStore::associateChildEdited)) {
                *result = 1;
            }
        }
    }
}

const QMetaObject Picto::DataStore::staticMetaObject = {
    { &Asset::staticMetaObject, qt_meta_stringdata_Picto__DataStore.data,
      qt_meta_data_Picto__DataStore,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *Picto::DataStore::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Picto::DataStore::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_Picto__DataStore.stringdata0))
        return static_cast<void*>(const_cast< DataStore*>(this));
    return Asset::qt_metacast(_clname);
}

int Picto::DataStore::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = Asset::qt_metacall(_c, _id, _a);
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

// SIGNAL 0
void Picto::DataStore::childAddedAfterDeserialize(QSharedPointer<Asset> _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void Picto::DataStore::associateChildEdited()
{
    QMetaObject::activate(this, &staticMetaObject, 1, Q_NULLPTR);
}
QT_END_MOC_NAMESPACE
