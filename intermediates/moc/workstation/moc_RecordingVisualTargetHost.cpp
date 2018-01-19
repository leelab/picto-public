/****************************************************************************
** Meta object code from reading C++ file 'RecordingVisualTargetHost.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.5.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../source/workstation/ReplayViewer/RecordingVisualTargetHost.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'RecordingVisualTargetHost.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.5.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_Picto__RecordingVisualTargetHost_t {
    QByteArrayData data[7];
    char stringdata0[112];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Picto__RecordingVisualTargetHost_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Picto__RecordingVisualTargetHost_t qt_meta_stringdata_Picto__RecordingVisualTargetHost = {
    {
QT_MOC_LITERAL(0, 0, 32), // "Picto::RecordingVisualTargetHost"
QT_MOC_LITERAL(1, 33, 19), // "updateRecordingTime"
QT_MOC_LITERAL(2, 53, 0), // ""
QT_MOC_LITERAL(3, 54, 8), // "currTime"
QT_MOC_LITERAL(4, 63, 15), // "toggleRecording"
QT_MOC_LITERAL(5, 79, 16), // "restartRecording"
QT_MOC_LITERAL(6, 96, 15) // "updateFrameTime"

    },
    "Picto::RecordingVisualTargetHost\0"
    "updateRecordingTime\0\0currTime\0"
    "toggleRecording\0restartRecording\0"
    "updateFrameTime"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Picto__RecordingVisualTargetHost[] = {

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
       4,    0,   37,    2, 0x0a /* Public */,
       5,    0,   38,    2, 0x0a /* Public */,
       6,    1,   39,    2, 0x0a /* Public */,

 // signals: parameters
    QMetaType::Void, QMetaType::Double,    3,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Double,    2,

       0        // eod
};

void Picto::RecordingVisualTargetHost::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        RecordingVisualTargetHost *_t = static_cast<RecordingVisualTargetHost *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->updateRecordingTime((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 1: _t->toggleRecording(); break;
        case 2: _t->restartRecording(); break;
        case 3: _t->updateFrameTime((*reinterpret_cast< double(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (RecordingVisualTargetHost::*_t)(double );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&RecordingVisualTargetHost::updateRecordingTime)) {
                *result = 0;
            }
        }
    }
}

const QMetaObject Picto::RecordingVisualTargetHost::staticMetaObject = {
    { &VisualTargetHost::staticMetaObject, qt_meta_stringdata_Picto__RecordingVisualTargetHost.data,
      qt_meta_data_Picto__RecordingVisualTargetHost,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *Picto::RecordingVisualTargetHost::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Picto::RecordingVisualTargetHost::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_Picto__RecordingVisualTargetHost.stringdata0))
        return static_cast<void*>(const_cast< RecordingVisualTargetHost*>(this));
    return VisualTargetHost::qt_metacast(_clname);
}

int Picto::RecordingVisualTargetHost::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = VisualTargetHost::qt_metacall(_c, _id, _a);
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
void Picto::RecordingVisualTargetHost::updateRecordingTime(double _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_END_MOC_NAMESPACE
