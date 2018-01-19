/****************************************************************************
** Meta object code from reading C++ file 'AnalysisRunNotesData.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.5.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../source/common/parameter/AnalysisRunNotesData.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'AnalysisRunNotesData.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.5.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_Picto__AnalysisRunNotesData_t {
    QByteArrayData data[7];
    char stringdata0[100];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Picto__AnalysisRunNotesData_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Picto__AnalysisRunNotesData_t qt_meta_stringdata_Picto__AnalysisRunNotesData = {
    {
QT_MOC_LITERAL(0, 0, 27), // "Picto::AnalysisRunNotesData"
QT_MOC_LITERAL(1, 28, 10), // "getRunName"
QT_MOC_LITERAL(2, 39, 0), // ""
QT_MOC_LITERAL(3, 40, 18), // "getGlobalStartTime"
QT_MOC_LITERAL(4, 59, 16), // "getGlobalEndTime"
QT_MOC_LITERAL(5, 76, 11), // "getRunNotes"
QT_MOC_LITERAL(6, 88, 11) // "getRunIndex"

    },
    "Picto::AnalysisRunNotesData\0getRunName\0"
    "\0getGlobalStartTime\0getGlobalEndTime\0"
    "getRunNotes\0getRunIndex"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Picto__AnalysisRunNotesData[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       5,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,   39,    2, 0x0a /* Public */,
       3,    0,   40,    2, 0x0a /* Public */,
       4,    0,   41,    2, 0x0a /* Public */,
       5,    0,   42,    2, 0x0a /* Public */,
       6,    0,   43,    2, 0x0a /* Public */,

 // slots: parameters
    QMetaType::QString,
    QMetaType::Double,
    QMetaType::Double,
    QMetaType::QString,
    QMetaType::Int,

       0        // eod
};

void Picto::AnalysisRunNotesData::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        AnalysisRunNotesData *_t = static_cast<AnalysisRunNotesData *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: { QString _r = _t->getRunName();
            if (_a[0]) *reinterpret_cast< QString*>(_a[0]) = _r; }  break;
        case 1: { double _r = _t->getGlobalStartTime();
            if (_a[0]) *reinterpret_cast< double*>(_a[0]) = _r; }  break;
        case 2: { double _r = _t->getGlobalEndTime();
            if (_a[0]) *reinterpret_cast< double*>(_a[0]) = _r; }  break;
        case 3: { QString _r = _t->getRunNotes();
            if (_a[0]) *reinterpret_cast< QString*>(_a[0]) = _r; }  break;
        case 4: { int _r = _t->getRunIndex();
            if (_a[0]) *reinterpret_cast< int*>(_a[0]) = _r; }  break;
        default: ;
        }
    }
}

const QMetaObject Picto::AnalysisRunNotesData::staticMetaObject = {
    { &AnalysisDataSource::staticMetaObject, qt_meta_stringdata_Picto__AnalysisRunNotesData.data,
      qt_meta_data_Picto__AnalysisRunNotesData,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *Picto::AnalysisRunNotesData::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Picto::AnalysisRunNotesData::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_Picto__AnalysisRunNotesData.stringdata0))
        return static_cast<void*>(const_cast< AnalysisRunNotesData*>(this));
    return AnalysisDataSource::qt_metacast(_clname);
}

int Picto::AnalysisRunNotesData::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = AnalysisDataSource::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 5)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 5;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 5)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 5;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
