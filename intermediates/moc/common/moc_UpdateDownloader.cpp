/****************************************************************************
** Meta object code from reading C++ file 'UpdateDownloader.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.5.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../source/common/update/UpdateDownloader.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'UpdateDownloader.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.5.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_UpdateDownloader_t {
    QByteArrayData data[13];
    char stringdata0[228];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_UpdateDownloader_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_UpdateDownloader_t qt_meta_stringdata_UpdateDownloader = {
    {
QT_MOC_LITERAL(0, 0, 16), // "UpdateDownloader"
QT_MOC_LITERAL(1, 17, 12), // "updateFailed"
QT_MOC_LITERAL(2, 30, 0), // ""
QT_MOC_LITERAL(3, 31, 14), // "closingProcess"
QT_MOC_LITERAL(4, 46, 16), // "serverDiscovered"
QT_MOC_LITERAL(5, 63, 12), // "QHostAddress"
QT_MOC_LITERAL(6, 76, 18), // "pictoServerAddress"
QT_MOC_LITERAL(7, 95, 15), // "pictoServerPort"
QT_MOC_LITERAL(8, 111, 25), // "requestServerPictoVersion"
QT_MOC_LITERAL(9, 137, 29), // "serverVersionResponseReceived"
QT_MOC_LITERAL(10, 167, 21), // "noNewVersionAvailable"
QT_MOC_LITERAL(11, 189, 19), // "newVersionAvailable"
QT_MOC_LITERAL(12, 209, 18) // "reattemptDiscovery"

    },
    "UpdateDownloader\0updateFailed\0\0"
    "closingProcess\0serverDiscovered\0"
    "QHostAddress\0pictoServerAddress\0"
    "pictoServerPort\0requestServerPictoVersion\0"
    "serverVersionResponseReceived\0"
    "noNewVersionAvailable\0newVersionAvailable\0"
    "reattemptDiscovery"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_UpdateDownloader[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       8,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    0,   54,    2, 0x06 /* Public */,
       3,    0,   55,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       4,    2,   56,    2, 0x08 /* Private */,
       8,    0,   61,    2, 0x08 /* Private */,
       9,    0,   62,    2, 0x08 /* Private */,
      10,    0,   63,    2, 0x08 /* Private */,
      11,    0,   64,    2, 0x08 /* Private */,
      12,    0,   65,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void,
    QMetaType::Void,

 // slots: parameters
    QMetaType::Void, 0x80000000 | 5, QMetaType::UShort,    6,    7,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void UpdateDownloader::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        UpdateDownloader *_t = static_cast<UpdateDownloader *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->updateFailed(); break;
        case 1: _t->closingProcess(); break;
        case 2: _t->serverDiscovered((*reinterpret_cast< QHostAddress(*)>(_a[1])),(*reinterpret_cast< quint16(*)>(_a[2]))); break;
        case 3: _t->requestServerPictoVersion(); break;
        case 4: _t->serverVersionResponseReceived(); break;
        case 5: _t->noNewVersionAvailable(); break;
        case 6: _t->newVersionAvailable(); break;
        case 7: _t->reattemptDiscovery(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (UpdateDownloader::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&UpdateDownloader::updateFailed)) {
                *result = 0;
            }
        }
        {
            typedef void (UpdateDownloader::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&UpdateDownloader::closingProcess)) {
                *result = 1;
            }
        }
    }
}

const QMetaObject UpdateDownloader::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_UpdateDownloader.data,
      qt_meta_data_UpdateDownloader,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *UpdateDownloader::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *UpdateDownloader::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_UpdateDownloader.stringdata0))
        return static_cast<void*>(const_cast< UpdateDownloader*>(this));
    return QObject::qt_metacast(_clname);
}

int UpdateDownloader::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 8)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 8;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 8)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 8;
    }
    return _id;
}

// SIGNAL 0
void UpdateDownloader::updateFailed()
{
    QMetaObject::activate(this, &staticMetaObject, 0, Q_NULLPTR);
}

// SIGNAL 1
void UpdateDownloader::closingProcess()
{
    QMetaObject::activate(this, &staticMetaObject, 1, Q_NULLPTR);
}
QT_END_MOC_NAMESPACE
