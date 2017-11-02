/****************************************************************************
** Meta object code from reading C++ file 'RemoteStateUpdater.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.5.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../source/workstation/RemoteViewer/RemoteStateUpdater.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'RemoteStateUpdater.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.5.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_Picto__RemoteStateUpdater_t {
    QByteArrayData data[5];
    char stringdata0[73];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Picto__RemoteStateUpdater_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Picto__RemoteStateUpdater_t qt_meta_stringdata_Picto__RemoteStateUpdater = {
    {
QT_MOC_LITERAL(0, 0, 25), // "Picto::RemoteStateUpdater"
QT_MOC_LITERAL(1, 26, 18), // "updateCurrUnitTime"
QT_MOC_LITERAL(2, 45, 0), // ""
QT_MOC_LITERAL(3, 46, 4), // "time"
QT_MOC_LITERAL(4, 51, 21) // "prepareToProcessQueue"

    },
    "Picto::RemoteStateUpdater\0updateCurrUnitTime\0"
    "\0time\0prepareToProcessQueue"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Picto__RemoteStateUpdater[] = {

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
       4,    0,   27,    2, 0x0a /* Public */,

 // slots: parameters
    QMetaType::Void, QMetaType::QString,    3,
    QMetaType::Void,

       0        // eod
};

void Picto::RemoteStateUpdater::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        RemoteStateUpdater *_t = static_cast<RemoteStateUpdater *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->updateCurrUnitTime((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 1: _t->prepareToProcessQueue(); break;
        default: ;
        }
    }
}

const QMetaObject Picto::RemoteStateUpdater::staticMetaObject = {
    { &StateUpdater::staticMetaObject, qt_meta_stringdata_Picto__RemoteStateUpdater.data,
      qt_meta_data_Picto__RemoteStateUpdater,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *Picto::RemoteStateUpdater::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Picto::RemoteStateUpdater::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_Picto__RemoteStateUpdater.stringdata0))
        return static_cast<void*>(const_cast< RemoteStateUpdater*>(this));
    return StateUpdater::qt_metacast(_clname);
}

int Picto::RemoteStateUpdater::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = StateUpdater::qt_metacall(_c, _id, _a);
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
