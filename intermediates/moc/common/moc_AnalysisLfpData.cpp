/****************************************************************************
** Meta object code from reading C++ file 'AnalysisLfpData.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.5.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../source/common/parameter/AnalysisLfpData.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'AnalysisLfpData.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.5.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_Picto__AnalysisLfpData_t {
    QByteArrayData data[23];
    char stringdata0[302];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Picto__AnalysisLfpData_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Picto__AnalysisLfpData_t qt_meta_stringdata_Picto__AnalysisLfpData = {
    {
QT_MOC_LITERAL(0, 0, 22), // "Picto::AnalysisLfpData"
QT_MOC_LITERAL(1, 23, 15), // "zeroLatestFrame"
QT_MOC_LITERAL(2, 39, 0), // ""
QT_MOC_LITERAL(3, 40, 11), // "getZeroTime"
QT_MOC_LITERAL(4, 52, 11), // "getChannels"
QT_MOC_LITERAL(5, 64, 15), // "getSamplePeriod"
QT_MOC_LITERAL(6, 80, 13), // "getLatestTime"
QT_MOC_LITERAL(7, 94, 14), // "getLatestValue"
QT_MOC_LITERAL(8, 109, 7), // "channel"
QT_MOC_LITERAL(9, 117, 11), // "getNextTime"
QT_MOC_LITERAL(10, 129, 12), // "getNextValue"
QT_MOC_LITERAL(11, 142, 12), // "getPrevTimes"
QT_MOC_LITERAL(12, 155, 13), // "secsPreceding"
QT_MOC_LITERAL(13, 169, 12), // "getNextTimes"
QT_MOC_LITERAL(14, 182, 13), // "secsFollowing"
QT_MOC_LITERAL(15, 196, 13), // "getTimesSince"
QT_MOC_LITERAL(16, 210, 10), // "beyondTime"
QT_MOC_LITERAL(17, 221, 13), // "getTimesUntil"
QT_MOC_LITERAL(18, 235, 8), // "upToTime"
QT_MOC_LITERAL(19, 244, 13), // "getPrevValues"
QT_MOC_LITERAL(20, 258, 13), // "getNextValues"
QT_MOC_LITERAL(21, 272, 14), // "getValuesSince"
QT_MOC_LITERAL(22, 287, 14) // "getValuesUntil"

    },
    "Picto::AnalysisLfpData\0zeroLatestFrame\0"
    "\0getZeroTime\0getChannels\0getSamplePeriod\0"
    "getLatestTime\0getLatestValue\0channel\0"
    "getNextTime\0getNextValue\0getPrevTimes\0"
    "secsPreceding\0getNextTimes\0secsFollowing\0"
    "getTimesSince\0beyondTime\0getTimesUntil\0"
    "upToTime\0getPrevValues\0getNextValues\0"
    "getValuesSince\0getValuesUntil"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Picto__AnalysisLfpData[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
      22,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,  124,    2, 0x0a /* Public */,
       3,    0,  125,    2, 0x0a /* Public */,
       4,    0,  126,    2, 0x0a /* Public */,
       5,    0,  127,    2, 0x0a /* Public */,
       6,    0,  128,    2, 0x0a /* Public */,
       7,    1,  129,    2, 0x0a /* Public */,
       7,    0,  132,    2, 0x0a /* Public */,
       9,    0,  133,    2, 0x0a /* Public */,
      10,    1,  134,    2, 0x0a /* Public */,
      10,    0,  137,    2, 0x0a /* Public */,
      11,    1,  138,    2, 0x0a /* Public */,
      13,    1,  141,    2, 0x0a /* Public */,
      15,    1,  144,    2, 0x0a /* Public */,
      17,    1,  147,    2, 0x0a /* Public */,
      19,    2,  150,    2, 0x0a /* Public */,
      20,    2,  155,    2, 0x0a /* Public */,
      19,    1,  160,    2, 0x0a /* Public */,
      20,    1,  163,    2, 0x0a /* Public */,
      21,    2,  166,    2, 0x0a /* Public */,
      22,    2,  171,    2, 0x0a /* Public */,
      21,    1,  176,    2, 0x0a /* Public */,
      22,    1,  179,    2, 0x0a /* Public */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Double,
    QMetaType::QVariantList,
    QMetaType::Double,
    QMetaType::Double,
    QMetaType::Double, QMetaType::Int,    8,
    QMetaType::QVariantList,
    QMetaType::Double,
    QMetaType::Double, QMetaType::Int,    8,
    QMetaType::QVariantList,
    QMetaType::QVariantList, QMetaType::Double,   12,
    QMetaType::QVariantList, QMetaType::Double,   14,
    QMetaType::QVariantList, QMetaType::Double,   16,
    QMetaType::QVariantList, QMetaType::Double,   18,
    QMetaType::QVariantList, QMetaType::Int, QMetaType::Double,    8,   12,
    QMetaType::QVariantList, QMetaType::Int, QMetaType::Double,    8,   14,
    QMetaType::QVariantList, QMetaType::Double,   12,
    QMetaType::QVariantList, QMetaType::Double,   14,
    QMetaType::QVariantList, QMetaType::Int, QMetaType::Double,    8,   16,
    QMetaType::QVariantList, QMetaType::Int, QMetaType::Double,    8,   18,
    QMetaType::QVariantList, QMetaType::Double,   16,
    QMetaType::QVariantList, QMetaType::Double,   18,

       0        // eod
};

void Picto::AnalysisLfpData::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        AnalysisLfpData *_t = static_cast<AnalysisLfpData *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->zeroLatestFrame(); break;
        case 1: { double _r = _t->getZeroTime();
            if (_a[0]) *reinterpret_cast< double*>(_a[0]) = _r; }  break;
        case 2: { QVariantList _r = _t->getChannels();
            if (_a[0]) *reinterpret_cast< QVariantList*>(_a[0]) = _r; }  break;
        case 3: { double _r = _t->getSamplePeriod();
            if (_a[0]) *reinterpret_cast< double*>(_a[0]) = _r; }  break;
        case 4: { double _r = _t->getLatestTime();
            if (_a[0]) *reinterpret_cast< double*>(_a[0]) = _r; }  break;
        case 5: { double _r = _t->getLatestValue((*reinterpret_cast< int(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< double*>(_a[0]) = _r; }  break;
        case 6: { QVariantList _r = _t->getLatestValue();
            if (_a[0]) *reinterpret_cast< QVariantList*>(_a[0]) = _r; }  break;
        case 7: { double _r = _t->getNextTime();
            if (_a[0]) *reinterpret_cast< double*>(_a[0]) = _r; }  break;
        case 8: { double _r = _t->getNextValue((*reinterpret_cast< int(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< double*>(_a[0]) = _r; }  break;
        case 9: { QVariantList _r = _t->getNextValue();
            if (_a[0]) *reinterpret_cast< QVariantList*>(_a[0]) = _r; }  break;
        case 10: { QVariantList _r = _t->getPrevTimes((*reinterpret_cast< double(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< QVariantList*>(_a[0]) = _r; }  break;
        case 11: { QVariantList _r = _t->getNextTimes((*reinterpret_cast< double(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< QVariantList*>(_a[0]) = _r; }  break;
        case 12: { QVariantList _r = _t->getTimesSince((*reinterpret_cast< double(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< QVariantList*>(_a[0]) = _r; }  break;
        case 13: { QVariantList _r = _t->getTimesUntil((*reinterpret_cast< double(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< QVariantList*>(_a[0]) = _r; }  break;
        case 14: { QVariantList _r = _t->getPrevValues((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< double(*)>(_a[2])));
            if (_a[0]) *reinterpret_cast< QVariantList*>(_a[0]) = _r; }  break;
        case 15: { QVariantList _r = _t->getNextValues((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< double(*)>(_a[2])));
            if (_a[0]) *reinterpret_cast< QVariantList*>(_a[0]) = _r; }  break;
        case 16: { QVariantList _r = _t->getPrevValues((*reinterpret_cast< double(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< QVariantList*>(_a[0]) = _r; }  break;
        case 17: { QVariantList _r = _t->getNextValues((*reinterpret_cast< double(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< QVariantList*>(_a[0]) = _r; }  break;
        case 18: { QVariantList _r = _t->getValuesSince((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< double(*)>(_a[2])));
            if (_a[0]) *reinterpret_cast< QVariantList*>(_a[0]) = _r; }  break;
        case 19: { QVariantList _r = _t->getValuesUntil((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< double(*)>(_a[2])));
            if (_a[0]) *reinterpret_cast< QVariantList*>(_a[0]) = _r; }  break;
        case 20: { QVariantList _r = _t->getValuesSince((*reinterpret_cast< double(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< QVariantList*>(_a[0]) = _r; }  break;
        case 21: { QVariantList _r = _t->getValuesUntil((*reinterpret_cast< double(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< QVariantList*>(_a[0]) = _r; }  break;
        default: ;
        }
    }
}

const QMetaObject Picto::AnalysisLfpData::staticMetaObject = {
    { &AnalysisDataSource::staticMetaObject, qt_meta_stringdata_Picto__AnalysisLfpData.data,
      qt_meta_data_Picto__AnalysisLfpData,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *Picto::AnalysisLfpData::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Picto::AnalysisLfpData::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_Picto__AnalysisLfpData.stringdata0))
        return static_cast<void*>(const_cast< AnalysisLfpData*>(this));
    return AnalysisDataSource::qt_metacast(_clname);
}

int Picto::AnalysisLfpData::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = AnalysisDataSource::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 22)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 22;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 22)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 22;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
