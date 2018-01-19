/****************************************************************************
** Meta object code from reading C++ file 'AnalysisFileOutput.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.5.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../source/common/parameter/AnalysisFileOutput.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'AnalysisFileOutput.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.5.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_Picto__AnalysisFileOutput_t {
    QByteArrayData data[8];
    char stringdata0[81];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Picto__AnalysisFileOutput_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Picto__AnalysisFileOutput_t qt_meta_stringdata_Picto__AnalysisFileOutput = {
    {
QT_MOC_LITERAL(0, 0, 25), // "Picto::AnalysisFileOutput"
QT_MOC_LITERAL(1, 26, 9), // "writeLine"
QT_MOC_LITERAL(2, 36, 0), // ""
QT_MOC_LITERAL(3, 37, 4), // "text"
QT_MOC_LITERAL(4, 42, 9), // "writeText"
QT_MOC_LITERAL(5, 52, 11), // "writeBinary"
QT_MOC_LITERAL(6, 64, 7), // "csvData"
QT_MOC_LITERAL(7, 72, 8) // "csvTypes"

    },
    "Picto::AnalysisFileOutput\0writeLine\0"
    "\0text\0writeText\0writeBinary\0csvData\0"
    "csvTypes"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Picto__AnalysisFileOutput[] = {

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
       4,    1,   32,    2, 0x0a /* Public */,
       5,    2,   35,    2, 0x0a /* Public */,

 // slots: parameters
    QMetaType::Void, QMetaType::QString,    3,
    QMetaType::Void, QMetaType::QString,    3,
    QMetaType::Void, QMetaType::QString, QMetaType::QString,    6,    7,

       0        // eod
};

void Picto::AnalysisFileOutput::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        AnalysisFileOutput *_t = static_cast<AnalysisFileOutput *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->writeLine((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 1: _t->writeText((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 2: _t->writeBinary((*reinterpret_cast< QString(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2]))); break;
        default: ;
        }
    }
}

const QMetaObject Picto::AnalysisFileOutput::staticMetaObject = {
    { &AnalysisOutput::staticMetaObject, qt_meta_stringdata_Picto__AnalysisFileOutput.data,
      qt_meta_data_Picto__AnalysisFileOutput,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *Picto::AnalysisFileOutput::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Picto::AnalysisFileOutput::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_Picto__AnalysisFileOutput.stringdata0))
        return static_cast<void*>(const_cast< AnalysisFileOutput*>(this));
    return AnalysisOutput::qt_metacast(_clname);
}

int Picto::AnalysisFileOutput::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = AnalysisOutput::qt_metacall(_c, _id, _a);
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
