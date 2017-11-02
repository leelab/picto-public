/****************************************************************************
** Meta object code from reading C++ file 'neuralDataViewer.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.5.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../source/workstation/RemoteViewer/neuralDataViewer.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'neuralDataViewer.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.5.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_NeuralDataViewer_t {
    QByteArrayData data[7];
    char stringdata0[82];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_NeuralDataViewer_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_NeuralDataViewer_t qt_meta_stringdata_NeuralDataViewer = {
    {
QT_MOC_LITERAL(0, 0, 16), // "NeuralDataViewer"
QT_MOC_LITERAL(1, 17, 17), // "setBehavioralTime"
QT_MOC_LITERAL(2, 35, 0), // ""
QT_MOC_LITERAL(3, 36, 4), // "time"
QT_MOC_LITERAL(4, 41, 18), // "plotChannelChanged"
QT_MOC_LITERAL(5, 60, 5), // "index"
QT_MOC_LITERAL(6, 66, 15) // "plotUnitChanged"

    },
    "NeuralDataViewer\0setBehavioralTime\0\0"
    "time\0plotChannelChanged\0index\0"
    "plotUnitChanged"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_NeuralDataViewer[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       3,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    1,   29,    2, 0x0a /* Public */,
       4,    1,   32,    2, 0x08 /* Private */,
       6,    1,   35,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void, QMetaType::Double,    3,
    QMetaType::Void, QMetaType::Int,    5,
    QMetaType::Void, QMetaType::Int,    5,

       0        // eod
};

void NeuralDataViewer::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        NeuralDataViewer *_t = static_cast<NeuralDataViewer *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->setBehavioralTime((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 1: _t->plotChannelChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 2: _t->plotUnitChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObject NeuralDataViewer::staticMetaObject = {
    { &QScrollArea::staticMetaObject, qt_meta_stringdata_NeuralDataViewer.data,
      qt_meta_data_NeuralDataViewer,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *NeuralDataViewer::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *NeuralDataViewer::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_NeuralDataViewer.stringdata0))
        return static_cast<void*>(const_cast< NeuralDataViewer*>(this));
    return QScrollArea::qt_metacast(_clname);
}

int NeuralDataViewer::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QScrollArea::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 3)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 3;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 3)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 3;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
