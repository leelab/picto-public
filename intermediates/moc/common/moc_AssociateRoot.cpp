/****************************************************************************
** Meta object code from reading C++ file 'AssociateRoot.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.5.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../source/common/parameter/AssociateRoot.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#include <QtCore/QSharedPointer>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'AssociateRoot.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.5.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_Picto__AssociateRoot_t {
    QByteArrayData data[9];
    char stringdata0[134];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Picto__AssociateRoot_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Picto__AssociateRoot_t qt_meta_stringdata_Picto__AssociateRoot = {
    {
QT_MOC_LITERAL(0, 0, 20), // "Picto::AssociateRoot"
QT_MOC_LITERAL(1, 21, 25), // "linkedAssetPropertyEdited"
QT_MOC_LITERAL(2, 47, 0), // ""
QT_MOC_LITERAL(3, 48, 9), // "Property*"
QT_MOC_LITERAL(4, 58, 11), // "changedProp"
QT_MOC_LITERAL(5, 70, 8), // "newValue"
QT_MOC_LITERAL(6, 79, 26), // "AssociateRootChildWasAdded"
QT_MOC_LITERAL(7, 106, 21), // "QSharedPointer<Asset>"
QT_MOC_LITERAL(8, 128, 5) // "child"

    },
    "Picto::AssociateRoot\0linkedAssetPropertyEdited\0"
    "\0Property*\0changedProp\0newValue\0"
    "AssociateRootChildWasAdded\0"
    "QSharedPointer<Asset>\0child"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Picto__AssociateRoot[] = {

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

void Picto::AssociateRoot::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        AssociateRoot *_t = static_cast<AssociateRoot *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->linkedAssetPropertyEdited((*reinterpret_cast< Property*(*)>(_a[1])),(*reinterpret_cast< QVariant(*)>(_a[2]))); break;
        case 1: _t->AssociateRootChildWasAdded((*reinterpret_cast< QSharedPointer<Asset>(*)>(_a[1]))); break;
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
        case 1:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QSharedPointer<Asset> >(); break;
            }
            break;
        }
    }
}

const QMetaObject Picto::AssociateRoot::staticMetaObject = {
    { &UIEnabled::staticMetaObject, qt_meta_stringdata_Picto__AssociateRoot.data,
      qt_meta_data_Picto__AssociateRoot,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *Picto::AssociateRoot::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Picto::AssociateRoot::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_Picto__AssociateRoot.stringdata0))
        return static_cast<void*>(const_cast< AssociateRoot*>(this));
    return UIEnabled::qt_metacast(_clname);
}

int Picto::AssociateRoot::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = UIEnabled::qt_metacall(_c, _id, _a);
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
