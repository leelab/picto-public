/****************************************************************************
** Meta object code from reading C++ file 'CommandChannel.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.5.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../source/common/network/CommandChannel.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'CommandChannel.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.5.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_Picto__CommandChannel_t {
    QByteArrayData data[6];
    char stringdata0[98];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Picto__CommandChannel_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Picto__CommandChannel_t qt_meta_stringdata_Picto__CommandChannel = {
    {
QT_MOC_LITERAL(0, 0, 21), // "Picto::CommandChannel"
QT_MOC_LITERAL(1, 22, 20), // "connectAttemptFailed"
QT_MOC_LITERAL(2, 43, 0), // ""
QT_MOC_LITERAL(3, 44, 19), // "channelDisconnected"
QT_MOC_LITERAL(4, 64, 15), // "connectToServer"
QT_MOC_LITERAL(5, 80, 17) // "disconnectHandler"

    },
    "Picto::CommandChannel\0connectAttemptFailed\0"
    "\0channelDisconnected\0connectToServer\0"
    "disconnectHandler"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Picto__CommandChannel[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    0,   34,    2, 0x06 /* Public */,
       3,    0,   35,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       4,    0,   36,    2, 0x0a /* Public */,
       5,    0,   37,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void,
    QMetaType::Void,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void Picto::CommandChannel::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        CommandChannel *_t = static_cast<CommandChannel *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->connectAttemptFailed(); break;
        case 1: _t->channelDisconnected(); break;
        case 2: _t->connectToServer(); break;
        case 3: _t->disconnectHandler(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (CommandChannel::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&CommandChannel::connectAttemptFailed)) {
                *result = 0;
            }
        }
        {
            typedef void (CommandChannel::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&CommandChannel::channelDisconnected)) {
                *result = 1;
            }
        }
    }
    Q_UNUSED(_a);
}

const QMetaObject Picto::CommandChannel::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_Picto__CommandChannel.data,
      qt_meta_data_Picto__CommandChannel,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *Picto::CommandChannel::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Picto::CommandChannel::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_Picto__CommandChannel.stringdata0))
        return static_cast<void*>(const_cast< CommandChannel*>(this));
    return QObject::qt_metacast(_clname);
}

int Picto::CommandChannel::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
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
void Picto::CommandChannel::connectAttemptFailed()
{
    QMetaObject::activate(this, &staticMetaObject, 0, Q_NULLPTR);
}

// SIGNAL 1
void Picto::CommandChannel::channelDisconnected()
{
    QMetaObject::activate(this, &staticMetaObject, 1, Q_NULLPTR);
}
QT_END_MOC_NAMESPACE
