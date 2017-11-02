/****************************************************************************
** Meta object code from reading C++ file 'AssociateHostLink.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.5.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../source/common/parameter/AssociateHostLink.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#include <QtCore/QSharedPointer>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'AssociateHostLink.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.5.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_Picto__AssociateHostLink_t {
    QByteArrayData data[6];
    char stringdata0[96];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Picto__AssociateHostLink_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Picto__AssociateHostLink_t qt_meta_stringdata_Picto__AssociateHostLink = {
    {
QT_MOC_LITERAL(0, 0, 24), // "Picto::AssociateHostLink"
QT_MOC_LITERAL(1, 25, 13), // "linkedToAsset"
QT_MOC_LITERAL(2, 39, 0), // ""
QT_MOC_LITERAL(3, 40, 21), // "QSharedPointer<Asset>"
QT_MOC_LITERAL(4, 62, 5), // "asset"
QT_MOC_LITERAL(5, 68, 27) // "updateLinkedAssetProperties"

    },
    "Picto::AssociateHostLink\0linkedToAsset\0"
    "\0QSharedPointer<Asset>\0asset\0"
    "updateLinkedAssetProperties"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Picto__AssociateHostLink[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       2,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   24,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       5,    0,   27,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void, 0x80000000 | 3,    4,

 // slots: parameters
    QMetaType::Void,

       0        // eod
};

void Picto::AssociateHostLink::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        AssociateHostLink *_t = static_cast<AssociateHostLink *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->linkedToAsset((*reinterpret_cast< QSharedPointer<Asset>(*)>(_a[1]))); break;
        case 1: _t->updateLinkedAssetProperties(); break;
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
            typedef void (AssociateHostLink::*_t)(QSharedPointer<Asset> );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&AssociateHostLink::linkedToAsset)) {
                *result = 0;
            }
        }
    }
}

const QMetaObject Picto::AssociateHostLink::staticMetaObject = {
    { &DataStore::staticMetaObject, qt_meta_stringdata_Picto__AssociateHostLink.data,
      qt_meta_data_Picto__AssociateHostLink,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *Picto::AssociateHostLink::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Picto::AssociateHostLink::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_Picto__AssociateHostLink.stringdata0))
        return static_cast<void*>(const_cast< AssociateHostLink*>(this));
    return DataStore::qt_metacast(_clname);
}

int Picto::AssociateHostLink::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = DataStore::qt_metacall(_c, _id, _a);
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

// SIGNAL 0
void Picto::AssociateHostLink::linkedToAsset(QSharedPointer<Asset> _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_END_MOC_NAMESPACE
