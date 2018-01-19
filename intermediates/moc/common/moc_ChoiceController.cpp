/****************************************************************************
** Meta object code from reading C++ file 'ChoiceController.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.5.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../source/common/controlelements/ChoiceController.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'ChoiceController.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.5.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_Picto__ChoiceController_t {
    QByteArrayData data[7];
    char stringdata0[96];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Picto__ChoiceController_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Picto__ChoiceController_t qt_meta_stringdata_Picto__ChoiceController = {
    {
QT_MOC_LITERAL(0, 0, 23), // "Picto::ChoiceController"
QT_MOC_LITERAL(1, 24, 12), // "userOnTarget"
QT_MOC_LITERAL(2, 37, 0), // ""
QT_MOC_LITERAL(3, 38, 17), // "userEnteredTarget"
QT_MOC_LITERAL(4, 56, 16), // "userExitedTarget"
QT_MOC_LITERAL(5, 73, 12), // "fixationTime"
QT_MOC_LITERAL(6, 86, 9) // "totalTime"

    },
    "Picto::ChoiceController\0userOnTarget\0"
    "\0userEnteredTarget\0userExitedTarget\0"
    "fixationTime\0totalTime"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Picto__ChoiceController[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       3,   14, // methods
       2,   32, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,   29,    2, 0x0a /* Public */,
       3,    0,   30,    2, 0x0a /* Public */,
       4,    0,   31,    2, 0x0a /* Public */,

 // slots: parameters
    QMetaType::Bool,
    QMetaType::Bool,
    QMetaType::Bool,

 // properties: name, type, flags
       5, QMetaType::Int, 0x00095103,
       6, QMetaType::Int, 0x00095103,

       0        // eod
};

void Picto::ChoiceController::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        ChoiceController *_t = static_cast<ChoiceController *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: { bool _r = _t->userOnTarget();
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = _r; }  break;
        case 1: { bool _r = _t->userEnteredTarget();
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = _r; }  break;
        case 2: { bool _r = _t->userExitedTarget();
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = _r; }  break;
        default: ;
        }
    }
#ifndef QT_NO_PROPERTIES
    else if (_c == QMetaObject::ReadProperty) {
        ChoiceController *_t = static_cast<ChoiceController *>(_o);
        Q_UNUSED(_t)
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< int*>(_v) = _t->getFixationTime(); break;
        case 1: *reinterpret_cast< int*>(_v) = _t->getTotalTime(); break;
        default: break;
        }
    } else if (_c == QMetaObject::WriteProperty) {
        ChoiceController *_t = static_cast<ChoiceController *>(_o);
        Q_UNUSED(_t)
        void *_v = _a[0];
        switch (_id) {
        case 0: _t->setFixationTime(*reinterpret_cast< int*>(_v)); break;
        case 1: _t->setTotalTime(*reinterpret_cast< int*>(_v)); break;
        default: break;
        }
    } else if (_c == QMetaObject::ResetProperty) {
    }
#endif // QT_NO_PROPERTIES
}

const QMetaObject Picto::ChoiceController::staticMetaObject = {
    { &ControlElement::staticMetaObject, qt_meta_stringdata_Picto__ChoiceController.data,
      qt_meta_data_Picto__ChoiceController,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *Picto::ChoiceController::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Picto::ChoiceController::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_Picto__ChoiceController.stringdata0))
        return static_cast<void*>(const_cast< ChoiceController*>(this));
    return ControlElement::qt_metacast(_clname);
}

int Picto::ChoiceController::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = ControlElement::qt_metacall(_c, _id, _a);
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
#ifndef QT_NO_PROPERTIES
   else if (_c == QMetaObject::ReadProperty || _c == QMetaObject::WriteProperty
            || _c == QMetaObject::ResetProperty || _c == QMetaObject::RegisterPropertyMetaType) {
        qt_static_metacall(this, _c, _id, _a);
        _id -= 2;
    } else if (_c == QMetaObject::QueryPropertyDesignable) {
        _id -= 2;
    } else if (_c == QMetaObject::QueryPropertyScriptable) {
        _id -= 2;
    } else if (_c == QMetaObject::QueryPropertyStored) {
        _id -= 2;
    } else if (_c == QMetaObject::QueryPropertyEditable) {
        _id -= 2;
    } else if (_c == QMetaObject::QueryPropertyUser) {
        _id -= 2;
    }
#endif // QT_NO_PROPERTIES
    return _id;
}
QT_END_MOC_NAMESPACE
