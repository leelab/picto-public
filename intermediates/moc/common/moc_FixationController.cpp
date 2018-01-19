/****************************************************************************
** Meta object code from reading C++ file 'FixationController.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.5.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../source/common/controlelements/FixationController.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'FixationController.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.5.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_Picto__FixationController_t {
    QByteArrayData data[10];
    char stringdata0[171];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Picto__FixationController_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Picto__FixationController_t qt_meta_stringdata_Picto__FixationController = {
    {
QT_MOC_LITERAL(0, 0, 25), // "Picto::FixationController"
QT_MOC_LITERAL(1, 26, 12), // "userOnTarget"
QT_MOC_LITERAL(2, 39, 0), // ""
QT_MOC_LITERAL(3, 40, 17), // "userEnteredTarget"
QT_MOC_LITERAL(4, 58, 16), // "userExitedTarget"
QT_MOC_LITERAL(5, 75, 23), // "controlTargetNameEdited"
QT_MOC_LITERAL(6, 99, 12), // "fixationTime"
QT_MOC_LITERAL(7, 112, 18), // "minAcquisitionTime"
QT_MOC_LITERAL(8, 131, 18), // "maxAcquisitionTime"
QT_MOC_LITERAL(9, 150, 20) // "maxReacquisitionTime"

    },
    "Picto::FixationController\0userOnTarget\0"
    "\0userEnteredTarget\0userExitedTarget\0"
    "controlTargetNameEdited\0fixationTime\0"
    "minAcquisitionTime\0maxAcquisitionTime\0"
    "maxReacquisitionTime"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Picto__FixationController[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   14, // methods
       4,   38, // properties
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
       7, QMetaType::Int, 0x00095003,
       8, QMetaType::Int, 0x00095003,
       9, QMetaType::Int, 0x00095003,

       0        // eod
};

void Picto::FixationController::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        FixationController *_t = static_cast<FixationController *>(_o);
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
        FixationController *_t = static_cast<FixationController *>(_o);
        Q_UNUSED(_t)
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< int*>(_v) = _t->getFixationTime(); break;
        case 1: *reinterpret_cast< int*>(_v) = _t->getMinAcqTime(); break;
        case 2: *reinterpret_cast< int*>(_v) = _t->getMaxAcqTime(); break;
        case 3: *reinterpret_cast< int*>(_v) = _t->getMaxReacqTime(); break;
        default: break;
        }
    } else if (_c == QMetaObject::WriteProperty) {
        FixationController *_t = static_cast<FixationController *>(_o);
        Q_UNUSED(_t)
        void *_v = _a[0];
        switch (_id) {
        case 0: _t->setFixationTime(*reinterpret_cast< int*>(_v)); break;
        case 1: _t->setMinAcqTime(*reinterpret_cast< int*>(_v)); break;
        case 2: _t->setMaxAcqTime(*reinterpret_cast< int*>(_v)); break;
        case 3: _t->setMaxReacqTime(*reinterpret_cast< int*>(_v)); break;
        default: break;
        }
    } else if (_c == QMetaObject::ResetProperty) {
    }
#endif // QT_NO_PROPERTIES
}

const QMetaObject Picto::FixationController::staticMetaObject = {
    { &ControlElement::staticMetaObject, qt_meta_stringdata_Picto__FixationController.data,
      qt_meta_data_Picto__FixationController,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *Picto::FixationController::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Picto::FixationController::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_Picto__FixationController.stringdata0))
        return static_cast<void*>(const_cast< FixationController*>(this));
    return ControlElement::qt_metacast(_clname);
}

int Picto::FixationController::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
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
        _id -= 4;
    } else if (_c == QMetaObject::QueryPropertyDesignable) {
        _id -= 4;
    } else if (_c == QMetaObject::QueryPropertyScriptable) {
        _id -= 4;
    } else if (_c == QMetaObject::QueryPropertyStored) {
        _id -= 4;
    } else if (_c == QMetaObject::QueryPropertyEditable) {
        _id -= 4;
    } else if (_c == QMetaObject::QueryPropertyUser) {
        _id -= 4;
    }
#endif // QT_NO_PROPERTIES
    return _id;
}
QT_END_MOC_NAMESPACE
