/****************************************************************************
** Meta object code from reading C++ file 'FileSessionLoader.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.5.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../source/common/playback/FileSessionLoader.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'FileSessionLoader.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.5.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_Picto__FileSessionLoader_t {
    QByteArrayData data[4];
    char stringdata0[48];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Picto__FileSessionLoader_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Picto__FileSessionLoader_t qt_meta_stringdata_Picto__FileSessionLoader = {
    {
QT_MOC_LITERAL(0, 0, 24), // "Picto::FileSessionLoader"
QT_MOC_LITERAL(1, 25, 13), // "percentLoaded"
QT_MOC_LITERAL(2, 39, 0), // ""
QT_MOC_LITERAL(3, 40, 7) // "percent"

    },
    "Picto::FileSessionLoader\0percentLoaded\0"
    "\0percent"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Picto__FileSessionLoader[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   19,    2, 0x06 /* Public */,

 // signals: parameters
    QMetaType::Void, QMetaType::Double,    3,

       0        // eod
};

void Picto::FileSessionLoader::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        FileSessionLoader *_t = static_cast<FileSessionLoader *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->percentLoaded((*reinterpret_cast< double(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (FileSessionLoader::*_t)(double );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&FileSessionLoader::percentLoaded)) {
                *result = 0;
            }
        }
    }
}

const QMetaObject Picto::FileSessionLoader::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_Picto__FileSessionLoader.data,
      qt_meta_data_Picto__FileSessionLoader,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *Picto::FileSessionLoader::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Picto::FileSessionLoader::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_Picto__FileSessionLoader.stringdata0))
        return static_cast<void*>(const_cast< FileSessionLoader*>(this));
    return QObject::qt_metacast(_clname);
}

int Picto::FileSessionLoader::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 1)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 1;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 1)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 1;
    }
    return _id;
}

// SIGNAL 0
void Picto::FileSessionLoader::percentLoaded(double _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_END_MOC_NAMESPACE
