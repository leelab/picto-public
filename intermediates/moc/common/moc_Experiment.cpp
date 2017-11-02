/****************************************************************************
** Meta object code from reading C++ file 'Experiment.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.5.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../source/common/experiment/Experiment.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#include <QtCore/QSharedPointer>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'Experiment.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.5.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_Picto__Experiment_t {
    QByteArrayData data[16];
    char stringdata0[184];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Picto__Experiment_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Picto__Experiment_t qt_meta_stringdata_Picto__Experiment = {
    {
QT_MOC_LITERAL(0, 0, 17), // "Picto::Experiment"
QT_MOC_LITERAL(1, 18, 9), // "taskAdded"
QT_MOC_LITERAL(2, 28, 0), // ""
QT_MOC_LITERAL(3, 29, 12), // "changeHostId"
QT_MOC_LITERAL(4, 42, 24), // "updateSignalCoefficients"
QT_MOC_LITERAL(5, 67, 9), // "Property*"
QT_MOC_LITERAL(6, 77, 11), // "changedProp"
QT_MOC_LITERAL(7, 89, 3), // "var"
QT_MOC_LITERAL(8, 93, 17), // "sortTasksIntoList"
QT_MOC_LITERAL(9, 111, 21), // "QSharedPointer<Asset>"
QT_MOC_LITERAL(10, 133, 8), // "newChild"
QT_MOC_LITERAL(11, 142, 7), // "xOffset"
QT_MOC_LITERAL(12, 150, 7), // "yOffset"
QT_MOC_LITERAL(13, 158, 5), // "xGain"
QT_MOC_LITERAL(14, 164, 5), // "yGain"
QT_MOC_LITERAL(15, 170, 13) // "xySignalShear"

    },
    "Picto::Experiment\0taskAdded\0\0changeHostId\0"
    "updateSignalCoefficients\0Property*\0"
    "changedProp\0var\0sortTasksIntoList\0"
    "QSharedPointer<Asset>\0newChild\0xOffset\0"
    "yOffset\0xGain\0yGain\0xySignalShear"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Picto__Experiment[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   14, // methods
       5,   44, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    0,   34,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       3,    0,   35,    2, 0x08 /* Private */,
       4,    2,   36,    2, 0x08 /* Private */,
       8,    1,   41,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 5, QMetaType::QVariant,    6,    7,
    QMetaType::Void, 0x80000000 | 9,   10,

 // properties: name, type, flags
      11, QMetaType::Double, 0x00095103,
      12, QMetaType::Double, 0x00095103,
      13, QMetaType::Double, 0x00095103,
      14, QMetaType::Double, 0x00095103,
      15, QMetaType::Double, 0x00095003,

       0        // eod
};

void Picto::Experiment::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Experiment *_t = static_cast<Experiment *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->taskAdded(); break;
        case 1: _t->changeHostId(); break;
        case 2: _t->updateSignalCoefficients((*reinterpret_cast< Property*(*)>(_a[1])),(*reinterpret_cast< QVariant(*)>(_a[2]))); break;
        case 3: _t->sortTasksIntoList((*reinterpret_cast< QSharedPointer<Asset>(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<int*>(_a[0]) = -1; break;
        case 2:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< Property* >(); break;
            }
            break;
        case 3:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QSharedPointer<Asset> >(); break;
            }
            break;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (Experiment::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&Experiment::taskAdded)) {
                *result = 0;
            }
        }
    }
#ifndef QT_NO_PROPERTIES
    else if (_c == QMetaObject::ReadProperty) {
        Experiment *_t = static_cast<Experiment *>(_o);
        Q_UNUSED(_t)
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< double*>(_v) = _t->getXOffset(); break;
        case 1: *reinterpret_cast< double*>(_v) = _t->getYOffset(); break;
        case 2: *reinterpret_cast< double*>(_v) = _t->getXGain(); break;
        case 3: *reinterpret_cast< double*>(_v) = _t->getYGain(); break;
        case 4: *reinterpret_cast< double*>(_v) = _t->getXYSignalShear(); break;
        default: break;
        }
    } else if (_c == QMetaObject::WriteProperty) {
        Experiment *_t = static_cast<Experiment *>(_o);
        Q_UNUSED(_t)
        void *_v = _a[0];
        switch (_id) {
        case 0: _t->setXOffset(*reinterpret_cast< double*>(_v)); break;
        case 1: _t->setYOffset(*reinterpret_cast< double*>(_v)); break;
        case 2: _t->setXGain(*reinterpret_cast< double*>(_v)); break;
        case 3: _t->setYGain(*reinterpret_cast< double*>(_v)); break;
        case 4: _t->setXYSignalShear(*reinterpret_cast< double*>(_v)); break;
        default: break;
        }
    } else if (_c == QMetaObject::ResetProperty) {
    }
#endif // QT_NO_PROPERTIES
}

const QMetaObject Picto::Experiment::staticMetaObject = {
    { &ScriptableContainer::staticMetaObject, qt_meta_stringdata_Picto__Experiment.data,
      qt_meta_data_Picto__Experiment,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *Picto::Experiment::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Picto::Experiment::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_Picto__Experiment.stringdata0))
        return static_cast<void*>(const_cast< Experiment*>(this));
    if (!strcmp(_clname, "AssociateRootHost"))
        return static_cast< AssociateRootHost*>(const_cast< Experiment*>(this));
    return ScriptableContainer::qt_metacast(_clname);
}

int Picto::Experiment::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = ScriptableContainer::qt_metacall(_c, _id, _a);
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
#ifndef QT_NO_PROPERTIES
   else if (_c == QMetaObject::ReadProperty || _c == QMetaObject::WriteProperty
            || _c == QMetaObject::ResetProperty || _c == QMetaObject::RegisterPropertyMetaType) {
        qt_static_metacall(this, _c, _id, _a);
        _id -= 5;
    } else if (_c == QMetaObject::QueryPropertyDesignable) {
        _id -= 5;
    } else if (_c == QMetaObject::QueryPropertyScriptable) {
        _id -= 5;
    } else if (_c == QMetaObject::QueryPropertyStored) {
        _id -= 5;
    } else if (_c == QMetaObject::QueryPropertyEditable) {
        _id -= 5;
    } else if (_c == QMetaObject::QueryPropertyUser) {
        _id -= 5;
    }
#endif // QT_NO_PROPERTIES
    return _id;
}

// SIGNAL 0
void Picto::Experiment::taskAdded()
{
    QMetaObject::activate(this, &staticMetaObject, 0, Q_NULLPTR);
}
QT_END_MOC_NAMESPACE
