/****************************************************************************
** Meta object code from reading C++ file 'Toolbox.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.5.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../source/common/designer/Toolbox.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#include <QtCore/QSharedPointer>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'Toolbox.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.5.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_Toolbox_t {
    QByteArrayData data[6];
    char stringdata0[58];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Toolbox_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Toolbox_t qt_meta_stringdata_Toolbox = {
    {
QT_MOC_LITERAL(0, 0, 7), // "Toolbox"
QT_MOC_LITERAL(1, 8, 8), // "setAsset"
QT_MOC_LITERAL(2, 17, 0), // ""
QT_MOC_LITERAL(3, 18, 21), // "QSharedPointer<Asset>"
QT_MOC_LITERAL(4, 40, 5), // "asset"
QT_MOC_LITERAL(5, 46, 11) // "clearAssets"

    },
    "Toolbox\0setAsset\0\0QSharedPointer<Asset>\0"
    "asset\0clearAssets"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Toolbox[] = {

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
       5,    0,   27,    2, 0x0a /* Public */,

 // slots: parameters
    QMetaType::Void, 0x80000000 | 3,    4,
    QMetaType::Void,

       0        // eod
};

void Toolbox::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Toolbox *_t = static_cast<Toolbox *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->setAsset((*reinterpret_cast< QSharedPointer<Asset>(*)>(_a[1]))); break;
        case 1: _t->clearAssets(); break;
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

const QMetaObject Toolbox::staticMetaObject = {
    { &QToolBox::staticMetaObject, qt_meta_stringdata_Toolbox.data,
      qt_meta_data_Toolbox,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *Toolbox::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Toolbox::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_Toolbox.stringdata0))
        return static_cast<void*>(const_cast< Toolbox*>(this));
    return QToolBox::qt_metacast(_clname);
}

int Toolbox::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QToolBox::qt_metacall(_c, _id, _a);
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
