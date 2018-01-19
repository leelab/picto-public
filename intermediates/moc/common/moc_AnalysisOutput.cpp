/****************************************************************************
** Meta object code from reading C++ file 'AnalysisOutput.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.5.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../source/common/parameter/AnalysisOutput.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'AnalysisOutput.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.5.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_Picto__AnalysisOutput_t {
    QByteArrayData data[7];
    char stringdata0[80];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Picto__AnalysisOutput_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Picto__AnalysisOutput_t qt_meta_stringdata_Picto__AnalysisOutput = {
    {
QT_MOC_LITERAL(0, 0, 21), // "Picto::AnalysisOutput"
QT_MOC_LITERAL(1, 22, 13), // "widgetIsValid"
QT_MOC_LITERAL(2, 36, 0), // ""
QT_MOC_LITERAL(3, 37, 16), // "widgetIdentifier"
QT_MOC_LITERAL(4, 54, 10), // "runStarted"
QT_MOC_LITERAL(5, 65, 5), // "runId"
QT_MOC_LITERAL(6, 71, 8) // "runEnded"

    },
    "Picto::AnalysisOutput\0widgetIsValid\0"
    "\0widgetIdentifier\0runStarted\0runId\0"
    "runEnded"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Picto__AnalysisOutput[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       3,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   29,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       4,    1,   32,    2, 0x09 /* Protected */,
       6,    0,   35,    2, 0x09 /* Protected */,

 // signals: parameters
    QMetaType::Void, QMetaType::QUuid,    3,

 // slots: parameters
    QMetaType::Void, QMetaType::QUuid,    5,
    QMetaType::Void,

       0        // eod
};

void Picto::AnalysisOutput::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        AnalysisOutput *_t = static_cast<AnalysisOutput *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->widgetIsValid((*reinterpret_cast< QUuid(*)>(_a[1]))); break;
        case 1: _t->runStarted((*reinterpret_cast< QUuid(*)>(_a[1]))); break;
        case 2: _t->runEnded(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (AnalysisOutput::*_t)(QUuid );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&AnalysisOutput::widgetIsValid)) {
                *result = 0;
            }
        }
    }
}

const QMetaObject Picto::AnalysisOutput::staticMetaObject = {
    { &AnalysisVariable::staticMetaObject, qt_meta_stringdata_Picto__AnalysisOutput.data,
      qt_meta_data_Picto__AnalysisOutput,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *Picto::AnalysisOutput::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Picto::AnalysisOutput::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_Picto__AnalysisOutput.stringdata0))
        return static_cast<void*>(const_cast< AnalysisOutput*>(this));
    return AnalysisVariable::qt_metacast(_clname);
}

int Picto::AnalysisOutput::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = AnalysisVariable::qt_metacall(_c, _id, _a);
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

// SIGNAL 0
void Picto::AnalysisOutput::widgetIsValid(QUuid _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_END_MOC_NAMESPACE
