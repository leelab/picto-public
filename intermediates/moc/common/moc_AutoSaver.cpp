/****************************************************************************
** Meta object code from reading C++ file 'AutoSaver.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.5.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../source/common/designer/AutoSaver.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#include <QtCore/QSharedPointer>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'AutoSaver.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.5.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_Picto__AutoSaver_t {
    QByteArrayData data[9];
    char stringdata0[138];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Picto__AutoSaver_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Picto__AutoSaver_t qt_meta_stringdata_Picto__AutoSaver = {
    {
QT_MOC_LITERAL(0, 0, 16), // "Picto::AutoSaver"
QT_MOC_LITERAL(1, 17, 10), // "openDesign"
QT_MOC_LITERAL(2, 28, 0), // ""
QT_MOC_LITERAL(3, 29, 26), // "QSharedPointer<DesignRoot>"
QT_MOC_LITERAL(4, 56, 10), // "designRoot"
QT_MOC_LITERAL(5, 67, 16), // "saveDesignToFile"
QT_MOC_LITERAL(6, 84, 26), // "deleteRestorableDesignFile"
QT_MOC_LITERAL(7, 111, 8), // "filePath"
QT_MOC_LITERAL(8, 120, 17) // "restoreDesignFile"

    },
    "Picto::AutoSaver\0openDesign\0\0"
    "QSharedPointer<DesignRoot>\0designRoot\0"
    "saveDesignToFile\0deleteRestorableDesignFile\0"
    "filePath\0restoreDesignFile"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Picto__AutoSaver[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   34,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       5,    0,   37,    2, 0x08 /* Private */,
       6,    1,   38,    2, 0x08 /* Private */,
       8,    1,   41,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void, 0x80000000 | 3,    4,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,    7,
    QMetaType::Void, QMetaType::QString,    7,

       0        // eod
};

void Picto::AutoSaver::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        AutoSaver *_t = static_cast<AutoSaver *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->openDesign((*reinterpret_cast< QSharedPointer<DesignRoot>(*)>(_a[1]))); break;
        case 1: _t->saveDesignToFile(); break;
        case 2: _t->deleteRestorableDesignFile((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 3: _t->restoreDesignFile((*reinterpret_cast< QString(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<int*>(_a[0]) = -1; break;
        case 0:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QSharedPointer<DesignRoot> >(); break;
            }
            break;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (AutoSaver::*_t)(QSharedPointer<DesignRoot> );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&AutoSaver::openDesign)) {
                *result = 0;
            }
        }
    }
}

const QMetaObject Picto::AutoSaver::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_Picto__AutoSaver.data,
      qt_meta_data_Picto__AutoSaver,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *Picto::AutoSaver::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Picto::AutoSaver::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_Picto__AutoSaver.stringdata0))
        return static_cast<void*>(const_cast< AutoSaver*>(this));
    return QObject::qt_metacast(_clname);
}

int Picto::AutoSaver::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
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
            qt_static_metacall(this, _c, _id, _a);
        _id -= 4;
    }
    return _id;
}

// SIGNAL 0
void Picto::AutoSaver::openDesign(QSharedPointer<DesignRoot> _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_END_MOC_NAMESPACE
