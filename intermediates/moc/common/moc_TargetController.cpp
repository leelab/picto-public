/****************************************************************************
** Meta object code from reading C++ file 'TargetController.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.5.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../source/common/controlelements/TargetController.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'TargetController.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.5.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_Picto__TargetController_t {
    QByteArrayData data[12];
    char stringdata0[200];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Picto__TargetController_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Picto__TargetController_t qt_meta_stringdata_Picto__TargetController = {
    {
QT_MOC_LITERAL(0, 0, 23), // "Picto::TargetController"
QT_MOC_LITERAL(1, 24, 12), // "userOnTarget"
QT_MOC_LITERAL(2, 37, 0), // ""
QT_MOC_LITERAL(3, 38, 17), // "userEnteredTarget"
QT_MOC_LITERAL(4, 56, 16), // "userExitedTarget"
QT_MOC_LITERAL(5, 73, 23), // "controlTargetNameEdited"
QT_MOC_LITERAL(6, 97, 12), // "fixationTime"
QT_MOC_LITERAL(7, 110, 9), // "totalTime"
QT_MOC_LITERAL(8, 120, 18), // "minAcquisitionTime"
QT_MOC_LITERAL(9, 139, 18), // "maxAcquisitionTime"
QT_MOC_LITERAL(10, 158, 20), // "maxReacquisitionTime"
QT_MOC_LITERAL(11, 179, 20) // "reacquisitionAllowed"

    },
    "Picto::TargetController\0userOnTarget\0"
    "\0userEnteredTarget\0userExitedTarget\0"
    "controlTargetNameEdited\0fixationTime\0"
    "totalTime\0minAcquisitionTime\0"
    "maxAcquisitionTime\0maxReacquisitionTime\0"
    "reacquisitionAllowed"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Picto__TargetController[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   14, // methods
       6,   38, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,   34,    2, 0x0a /* Public */,
       3,    0,   35,    2, 0x0a /* Public */,
       4,    0,   36,    2, 0x0a /* Public */,
       5,    0,   37,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Bool,
    QMetaType::Bool,
    QMetaType::Bool,
    QMetaType::Void,

 // properties: name, type, flags
       6, QMetaType::Int, 0x00095103,
       7, QMetaType::Int, 0x00095103,
       8, QMetaType::Int, 0x00095003,
       9, QMetaType::Int, 0x00095003,
      10, QMetaType::Int, 0x00095003,
      11, QMetaType::Bool, 0x00095003,

       0        // eod
};

void Picto::TargetController::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        TargetController *_t = static_cast<TargetController *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: { bool _r = _t->userOnTarget();
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = _r; }  break;
        case 1: { bool _r = _t->userEnteredTarget();
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = _r; }  break;
        case 2: { bool _r = _t->userExitedTarget();
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = _r; }  break;
        case 3: _t->controlTargetNameEdited(); break;
        default: ;
        }
    }
#ifndef QT_NO_PROPERTIES
    else if (_c == QMetaObject::ReadProperty) {
        TargetController *_t = static_cast<TargetController *>(_o);
        Q_UNUSED(_t)
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< int*>(_v) = _t->getFixationTime(); break;
        case 1: *reinterpret_cast< int*>(_v) = _t->getTotalTime(); break;
        case 2: *reinterpret_cast< int*>(_v) = _t->getMinAcqTime(); break;
        case 3: *reinterpret_cast< int*>(_v) = _t->getMaxAcqTime(); break;
        case 4: *reinterpret_cast< int*>(_v) = _t->getMaxReacqTime(); break;
        case 5: *reinterpret_cast< bool*>(_v) = _t->getReacqAllowed(); break;
        default: break;
        }
    } else if (_c == QMetaObject::WriteProperty) {
        TargetController *_t = static_cast<TargetController *>(_o);
        Q_UNUSED(_t)
        void *_v = _a[0];
        switch (_id) {
        case 0: _t->setFixationTime(*reinterpret_cast< int*>(_v)); break;
        case 1: _t->setTotalTime(*reinterpret_cast< int*>(_v)); break;
        case 2: _t->setMinAcqTime(*reinterpret_cast< int*>(_v)); break;
        case 3: _t->setMaxAcqTime(*reinterpret_cast< int*>(_v)); break;
        case 4: _t->setMaxReacqTime(*reinterpret_cast< int*>(_v)); break;
        case 5: _t->setReacqAllowed(*reinterpret_cast< bool*>(_v)); break;
        default: break;
        }
    } else if (_c == QMetaObject::ResetProperty) {
    }
#endif // QT_NO_PROPERTIES
}

const QMetaObject Picto::TargetController::staticMetaObject = {
    { &ControlElement::staticMetaObject, qt_meta_stringdata_Picto__TargetController.data,
      qt_meta_data_Picto__TargetController,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *Picto::TargetController::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Picto::TargetController::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_Picto__TargetController.stringdata0))
        return static_cast<void*>(const_cast< TargetController*>(this));
    return ControlElement::qt_metacast(_clname);
}

int Picto::TargetController::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = ControlElement::qt_metacall(_c, _id, _a);
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
#ifndef QT_NO_PROPERTIES
   else if (_c == QMetaObject::ReadProperty || _c == QMetaObject::WriteProperty
            || _c == QMetaObject::ResetProperty || _c == QMetaObject::RegisterPropertyMetaType) {
        qt_static_metacall(this, _c, _id, _a);
        _id -= 6;
    } else if (_c == QMetaObject::QueryPropertyDesignable) {
        _id -= 6;
    } else if (_c == QMetaObject::QueryPropertyScriptable) {
        _id -= 6;
    } else if (_c == QMetaObject::QueryPropertyStored) {
        _id -= 6;
    } else if (_c == QMetaObject::QueryPropertyEditable) {
        _id -= 6;
    } else if (_c == QMetaObject::QueryPropertyUser) {
        _id -= 6;
    }
#endif // QT_NO_PROPERTIES
    return _id;
}
QT_END_MOC_NAMESPACE
