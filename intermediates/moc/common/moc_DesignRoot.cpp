/****************************************************************************
** Meta object code from reading C++ file 'DesignRoot.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.5.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../source/common/design/DesignRoot.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'DesignRoot.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.5.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_Picto__DesignRoot_t {
    QByteArrayData data[7];
    char stringdata0[87];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Picto__DesignRoot_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Picto__DesignRoot_t qt_meta_stringdata_Picto__DesignRoot = {
    {
QT_MOC_LITERAL(0, 0, 17), // "Picto::DesignRoot"
QT_MOC_LITERAL(1, 18, 13), // "undoAvailable"
QT_MOC_LITERAL(2, 32, 0), // ""
QT_MOC_LITERAL(3, 33, 9), // "available"
QT_MOC_LITERAL(4, 43, 13), // "redoAvailable"
QT_MOC_LITERAL(5, 57, 16), // "refreshedFromXml"
QT_MOC_LITERAL(6, 74, 12) // "designEdited"

    },
    "Picto::DesignRoot\0undoAvailable\0\0"
    "available\0redoAvailable\0refreshedFromXml\0"
    "designEdited"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Picto__DesignRoot[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       3,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   34,    2, 0x06 /* Public */,
       4,    1,   37,    2, 0x06 /* Public */,
       5,    0,   40,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       6,    0,   41,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void, QMetaType::Bool,    3,
    QMetaType::Void, QMetaType::Bool,    3,
    QMetaType::Void,

 // slots: parameters
    QMetaType::Void,

       0        // eod
};

void Picto::DesignRoot::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        DesignRoot *_t = static_cast<DesignRoot *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->undoAvailable((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 1: _t->redoAvailable((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 2: _t->refreshedFromXml(); break;
        case 3: _t->designEdited(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (DesignRoot::*_t)(bool );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&DesignRoot::undoAvailable)) {
                *result = 0;
            }
        }
        {
            typedef void (DesignRoot::*_t)(bool );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&DesignRoot::redoAvailable)) {
                *result = 1;
            }
        }
        {
            typedef void (DesignRoot::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&DesignRoot::refreshedFromXml)) {
                *result = 2;
            }
        }
    }
}

const QMetaObject Picto::DesignRoot::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_Picto__DesignRoot.data,
      qt_meta_data_Picto__DesignRoot,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *Picto::DesignRoot::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Picto::DesignRoot::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_Picto__DesignRoot.stringdata0))
        return static_cast<void*>(const_cast< DesignRoot*>(this));
    return QObject::qt_metacast(_clname);
}

int Picto::DesignRoot::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 4)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 4;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 4)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 4;
    }
    return _id;
}

// SIGNAL 0
void Picto::DesignRoot::undoAvailable(bool _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void Picto::DesignRoot::redoAvailable(bool _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void Picto::DesignRoot::refreshedFromXml()
{
    QMetaObject::activate(this, &staticMetaObject, 2, Q_NULLPTR);
}
QT_END_MOC_NAMESPACE
