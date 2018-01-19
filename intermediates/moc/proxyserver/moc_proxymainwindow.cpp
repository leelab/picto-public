/****************************************************************************
** Meta object code from reading C++ file 'proxymainwindow.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.5.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../source/proxyserver/proxymainwindow.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'proxymainwindow.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.5.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_ProxyMainWindow_t {
    QByteArrayData data[21];
    char stringdata0[281];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_ProxyMainWindow_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_ProxyMainWindow_t qt_meta_stringdata_ProxyMainWindow = {
    {
QT_MOC_LITERAL(0, 0, 15), // "ProxyMainWindow"
QT_MOC_LITERAL(1, 16, 11), // "updateState"
QT_MOC_LITERAL(2, 28, 0), // ""
QT_MOC_LITERAL(3, 29, 10), // "enterState"
QT_MOC_LITERAL(4, 40, 8), // "runState"
QT_MOC_LITERAL(5, 49, 8), // "endState"
QT_MOC_LITERAL(6, 58, 15), // "startConnecting"
QT_MOC_LITERAL(7, 74, 30), // "setNeuralDataAcquisitionDevice"
QT_MOC_LITERAL(8, 105, 5), // "index"
QT_MOC_LITERAL(9, 111, 15), // "deviceIsRunning"
QT_MOC_LITERAL(10, 127, 13), // "deviceHasData"
QT_MOC_LITERAL(11, 141, 11), // "startDevice"
QT_MOC_LITERAL(12, 153, 10), // "stopDevice"
QT_MOC_LITERAL(13, 164, 17), // "isServerConnected"
QT_MOC_LITERAL(14, 182, 15), // "isSessionActive"
QT_MOC_LITERAL(15, 198, 10), // "closeEvent"
QT_MOC_LITERAL(16, 209, 12), // "QCloseEvent*"
QT_MOC_LITERAL(17, 222, 5), // "event"
QT_MOC_LITERAL(18, 228, 18), // "pluginIndexChanged"
QT_MOC_LITERAL(19, 247, 19), // "systemNumberChanged"
QT_MOC_LITERAL(20, 267, 13) // "writeSettings"

    },
    "ProxyMainWindow\0updateState\0\0enterState\0"
    "runState\0endState\0startConnecting\0"
    "setNeuralDataAcquisitionDevice\0index\0"
    "deviceIsRunning\0deviceHasData\0startDevice\0"
    "stopDevice\0isServerConnected\0"
    "isSessionActive\0closeEvent\0QCloseEvent*\0"
    "event\0pluginIndexChanged\0systemNumberChanged\0"
    "writeSettings"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_ProxyMainWindow[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
      16,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,   94,    2, 0x08 /* Private */,
       3,    0,   95,    2, 0x08 /* Private */,
       4,    0,   96,    2, 0x08 /* Private */,
       5,    0,   97,    2, 0x08 /* Private */,
       6,    0,   98,    2, 0x08 /* Private */,
       7,    1,   99,    2, 0x08 /* Private */,
       9,    0,  102,    2, 0x08 /* Private */,
      10,    0,  103,    2, 0x08 /* Private */,
      11,    0,  104,    2, 0x08 /* Private */,
      12,    0,  105,    2, 0x08 /* Private */,
      13,    0,  106,    2, 0x08 /* Private */,
      14,    0,  107,    2, 0x08 /* Private */,
      15,    1,  108,    2, 0x08 /* Private */,
      18,    1,  111,    2, 0x08 /* Private */,
      19,    1,  114,    2, 0x08 /* Private */,
      20,    0,  117,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,    8,
    QMetaType::Bool,
    QMetaType::Bool,
    QMetaType::Bool,
    QMetaType::Void,
    QMetaType::Bool,
    QMetaType::Bool,
    QMetaType::Void, 0x80000000 | 16,   17,
    QMetaType::Void, QMetaType::Int,    8,
    QMetaType::Void, QMetaType::Int,    8,
    QMetaType::Void,

       0        // eod
};

void ProxyMainWindow::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        ProxyMainWindow *_t = static_cast<ProxyMainWindow *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->updateState(); break;
        case 1: _t->enterState(); break;
        case 2: _t->runState(); break;
        case 3: _t->endState(); break;
        case 4: _t->startConnecting(); break;
        case 5: _t->setNeuralDataAcquisitionDevice((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 6: { bool _r = _t->deviceIsRunning();
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = _r; }  break;
        case 7: { bool _r = _t->deviceHasData();
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = _r; }  break;
        case 8: { bool _r = _t->startDevice();
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = _r; }  break;
        case 9: _t->stopDevice(); break;
        case 10: { bool _r = _t->isServerConnected();
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = _r; }  break;
        case 11: { bool _r = _t->isSessionActive();
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = _r; }  break;
        case 12: _t->closeEvent((*reinterpret_cast< QCloseEvent*(*)>(_a[1]))); break;
        case 13: _t->pluginIndexChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 14: _t->systemNumberChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 15: _t->writeSettings(); break;
        default: ;
        }
    }
}

const QMetaObject ProxyMainWindow::staticMetaObject = {
    { &QMainWindow::staticMetaObject, qt_meta_stringdata_ProxyMainWindow.data,
      qt_meta_data_ProxyMainWindow,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *ProxyMainWindow::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *ProxyMainWindow::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_ProxyMainWindow.stringdata0))
        return static_cast<void*>(const_cast< ProxyMainWindow*>(this));
    if (!strcmp(_clname, "ComponentInterface"))
        return static_cast< ComponentInterface*>(const_cast< ProxyMainWindow*>(this));
    return QMainWindow::qt_metacast(_clname);
}

int ProxyMainWindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 16)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 16;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 16)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 16;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
