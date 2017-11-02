/****************************************************************************
** Meta object code from reading C++ file 'AnalysisFrameData.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.5.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../source/common/parameter/AnalysisFrameData.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'AnalysisFrameData.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.5.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_Picto__AnalysisFrameData_t {
    QByteArrayData data[15];
    char stringdata0[194];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Picto__AnalysisFrameData_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Picto__AnalysisFrameData_t qt_meta_stringdata_Picto__AnalysisFrameData = {
    {
QT_MOC_LITERAL(0, 0, 24), // "Picto::AnalysisFrameData"
QT_MOC_LITERAL(1, 25, 15), // "zeroLatestFrame"
QT_MOC_LITERAL(2, 41, 0), // ""
QT_MOC_LITERAL(3, 42, 11), // "getZeroTime"
QT_MOC_LITERAL(4, 54, 13), // "getLatestTime"
QT_MOC_LITERAL(5, 68, 11), // "getPrevTime"
QT_MOC_LITERAL(6, 80, 11), // "getNextTime"
QT_MOC_LITERAL(7, 92, 12), // "getPrevTimes"
QT_MOC_LITERAL(8, 105, 13), // "secsPreceding"
QT_MOC_LITERAL(9, 119, 12), // "getNextTimes"
QT_MOC_LITERAL(10, 132, 13), // "secsFollowing"
QT_MOC_LITERAL(11, 146, 13), // "getTimesSince"
QT_MOC_LITERAL(12, 160, 10), // "beyondTime"
QT_MOC_LITERAL(13, 171, 13), // "getTimesUntil"
QT_MOC_LITERAL(14, 185, 8) // "upToTime"

    },
    "Picto::AnalysisFrameData\0zeroLatestFrame\0"
    "\0getZeroTime\0getLatestTime\0getPrevTime\0"
    "getNextTime\0getPrevTimes\0secsPreceding\0"
    "getNextTimes\0secsFollowing\0getTimesSince\0"
    "beyondTime\0getTimesUntil\0upToTime"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Picto__AnalysisFrameData[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       9,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,   59,    2, 0x0a /* Public */,
       3,    0,   60,    2, 0x0a /* Public */,
       4,    0,   61,    2, 0x0a /* Public */,
       5,    0,   62,    2, 0x0a /* Public */,
       6,    0,   63,    2, 0x0a /* Public */,
       7,    1,   64,    2, 0x0a /* Public */,
       9,    1,   67,    2, 0x0a /* Public */,
      11,    1,   70,    2, 0x0a /* Public */,
      13,    1,   73,    2, 0x0a /* Public */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Double,
    QMetaType::Double,
    QMetaType::Double,
    QMetaType::Double,
    QMetaType::QVariantList, QMetaType::Double,    8,
    QMetaType::QVariantList, QMetaType::Double,   10,
    QMetaType::QVariantList, QMetaType::Double,   12,
    QMetaType::QVariantList, QMetaType::Double,   14,

       0        // eod
};

void Picto::AnalysisFrameData::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        AnalysisFrameData *_t = static_cast<AnalysisFrameData *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->zeroLatestFrame(); break;
        case 1: { double _r = _t->getZeroTime();
            if (_a[0]) *reinterpret_cast< double*>(_a[0]) = _r; }  break;
        case 2: { double _r = _t->getLatestTime();
            if (_a[0]) *reinterpret_cast< double*>(_a[0]) = _r; }  break;
        case 3: { double _r = _t->getPrevTime();
            if (_a[0]) *reinterpret_cast< double*>(_a[0]) = _r; }  break;
        case 4: { double _r = _t->getNextTime();
            if (_a[0]) *reinterpret_cast< double*>(_a[0]) = _r; }  break;
        case 5: { QVariantList _r = _t->getPrevTimes((*reinterpret_cast< double(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< QVariantList*>(_a[0]) = _r; }  break;
        case 6: { QVariantList _r = _t->getNextTimes((*reinterpret_cast< double(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< QVariantList*>(_a[0]) = _r; }  break;
        case 7: { QVariantList _r = _t->getTimesSince((*reinterpret_cast< double(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< QVariantList*>(_a[0]) = _r; }  break;
        case 8: { QVariantList _r = _t->getTimesUntil((*reinterpret_cast< double(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< QVariantList*>(_a[0]) = _r; }  break;
        default: ;
        }
    }
}

const QMetaObject Picto::AnalysisFrameData::staticMetaObject = {
    { &AnalysisDataSource::staticMetaObject, qt_meta_stringdata_Picto__AnalysisFrameData.data,
      qt_meta_data_Picto__AnalysisFrameData,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *Picto::AnalysisFrameData::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Picto::AnalysisFrameData::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_Picto__AnalysisFrameData.stringdata0))
        return static_cast<void*>(const_cast< AnalysisFrameData*>(this));
    return AnalysisDataSource::qt_metacast(_clname);
}

int Picto::AnalysisFrameData::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = AnalysisDataSource::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 9)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 9;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 9)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 9;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
