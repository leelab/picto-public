/****************************************************************************
** Meta object code from reading C++ file 'AnalysisRewardData.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.5.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../source/common/parameter/AnalysisRewardData.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'AnalysisRewardData.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.5.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_Picto__AnalysisRewardData_t {
    QByteArrayData data[20];
    char stringdata0[287];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Picto__AnalysisRewardData_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Picto__AnalysisRewardData_t qt_meta_stringdata_Picto__AnalysisRewardData = {
    {
QT_MOC_LITERAL(0, 0, 25), // "Picto::AnalysisRewardData"
QT_MOC_LITERAL(1, 26, 15), // "zeroLatestFrame"
QT_MOC_LITERAL(2, 42, 0), // ""
QT_MOC_LITERAL(3, 43, 11), // "getZeroTime"
QT_MOC_LITERAL(4, 55, 13), // "getLatestTime"
QT_MOC_LITERAL(5, 69, 11), // "getNextTime"
QT_MOC_LITERAL(6, 81, 17), // "getLatestDuration"
QT_MOC_LITERAL(7, 99, 15), // "getNextDuration"
QT_MOC_LITERAL(8, 115, 12), // "getPrevTimes"
QT_MOC_LITERAL(9, 128, 13), // "secsPreceding"
QT_MOC_LITERAL(10, 142, 12), // "getNextTimes"
QT_MOC_LITERAL(11, 155, 13), // "secsFollowing"
QT_MOC_LITERAL(12, 169, 13), // "getTimesSince"
QT_MOC_LITERAL(13, 183, 10), // "beyondTime"
QT_MOC_LITERAL(14, 194, 13), // "getTimesUntil"
QT_MOC_LITERAL(15, 208, 8), // "upToTime"
QT_MOC_LITERAL(16, 217, 16), // "getPrevDurations"
QT_MOC_LITERAL(17, 234, 16), // "getNextDurations"
QT_MOC_LITERAL(18, 251, 17), // "getDurationsSince"
QT_MOC_LITERAL(19, 269, 17) // "getDurationsUntil"

    },
    "Picto::AnalysisRewardData\0zeroLatestFrame\0"
    "\0getZeroTime\0getLatestTime\0getNextTime\0"
    "getLatestDuration\0getNextDuration\0"
    "getPrevTimes\0secsPreceding\0getNextTimes\0"
    "secsFollowing\0getTimesSince\0beyondTime\0"
    "getTimesUntil\0upToTime\0getPrevDurations\0"
    "getNextDurations\0getDurationsSince\0"
    "getDurationsUntil"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Picto__AnalysisRewardData[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
      14,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,   84,    2, 0x0a /* Public */,
       3,    0,   85,    2, 0x0a /* Public */,
       4,    0,   86,    2, 0x0a /* Public */,
       5,    0,   87,    2, 0x0a /* Public */,
       6,    0,   88,    2, 0x0a /* Public */,
       7,    0,   89,    2, 0x0a /* Public */,
       8,    1,   90,    2, 0x0a /* Public */,
      10,    1,   93,    2, 0x0a /* Public */,
      12,    1,   96,    2, 0x0a /* Public */,
      14,    1,   99,    2, 0x0a /* Public */,
      16,    1,  102,    2, 0x0a /* Public */,
      17,    1,  105,    2, 0x0a /* Public */,
      18,    1,  108,    2, 0x0a /* Public */,
      19,    1,  111,    2, 0x0a /* Public */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Double,
    QMetaType::Double,
    QMetaType::Double,
    QMetaType::Int,
    QMetaType::Int,
    QMetaType::QVariantList, QMetaType::Double,    9,
    QMetaType::QVariantList, QMetaType::Double,   11,
    QMetaType::QVariantList, QMetaType::Double,   13,
    QMetaType::QVariantList, QMetaType::Double,   15,
    QMetaType::QVariantList, QMetaType::Double,    9,
    QMetaType::QVariantList, QMetaType::Double,   11,
    QMetaType::QVariantList, QMetaType::Double,   13,
    QMetaType::QVariantList, QMetaType::Double,   15,

       0        // eod
};

void Picto::AnalysisRewardData::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        AnalysisRewardData *_t = static_cast<AnalysisRewardData *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->zeroLatestFrame(); break;
        case 1: { double _r = _t->getZeroTime();
            if (_a[0]) *reinterpret_cast< double*>(_a[0]) = _r; }  break;
        case 2: { double _r = _t->getLatestTime();
            if (_a[0]) *reinterpret_cast< double*>(_a[0]) = _r; }  break;
        case 3: { double _r = _t->getNextTime();
            if (_a[0]) *reinterpret_cast< double*>(_a[0]) = _r; }  break;
        case 4: { int _r = _t->getLatestDuration();
            if (_a[0]) *reinterpret_cast< int*>(_a[0]) = _r; }  break;
        case 5: { int _r = _t->getNextDuration();
            if (_a[0]) *reinterpret_cast< int*>(_a[0]) = _r; }  break;
        case 6: { QVariantList _r = _t->getPrevTimes((*reinterpret_cast< double(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< QVariantList*>(_a[0]) = _r; }  break;
        case 7: { QVariantList _r = _t->getNextTimes((*reinterpret_cast< double(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< QVariantList*>(_a[0]) = _r; }  break;
        case 8: { QVariantList _r = _t->getTimesSince((*reinterpret_cast< double(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< QVariantList*>(_a[0]) = _r; }  break;
        case 9: { QVariantList _r = _t->getTimesUntil((*reinterpret_cast< double(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< QVariantList*>(_a[0]) = _r; }  break;
        case 10: { QVariantList _r = _t->getPrevDurations((*reinterpret_cast< double(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< QVariantList*>(_a[0]) = _r; }  break;
        case 11: { QVariantList _r = _t->getNextDurations((*reinterpret_cast< double(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< QVariantList*>(_a[0]) = _r; }  break;
        case 12: { QVariantList _r = _t->getDurationsSince((*reinterpret_cast< double(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< QVariantList*>(_a[0]) = _r; }  break;
        case 13: { QVariantList _r = _t->getDurationsUntil((*reinterpret_cast< double(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< QVariantList*>(_a[0]) = _r; }  break;
        default: ;
        }
    }
}

const QMetaObject Picto::AnalysisRewardData::staticMetaObject = {
    { &AnalysisDataSource::staticMetaObject, qt_meta_stringdata_Picto__AnalysisRewardData.data,
      qt_meta_data_Picto__AnalysisRewardData,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *Picto::AnalysisRewardData::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Picto::AnalysisRewardData::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_Picto__AnalysisRewardData.stringdata0))
        return static_cast<void*>(const_cast< AnalysisRewardData*>(this));
    return AnalysisDataSource::qt_metacast(_clname);
}

int Picto::AnalysisRewardData::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = AnalysisDataSource::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 14)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 14;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 14)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 14;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
