/****************************************************************************
** Meta object code from reading C++ file 'RasterBasePlotHandler.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.5.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../source/common/operator/RasterBasePlotHandler.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#include <QtCore/QVector>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'RasterBasePlotHandler.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.5.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_Picto__RasterBasePlotHandler_t {
    QByteArrayData data[20];
    char stringdata0[240];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Picto__RasterBasePlotHandler_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Picto__RasterBasePlotHandler_t qt_meta_stringdata_Picto__RasterBasePlotHandler = {
    {
QT_MOC_LITERAL(0, 0, 28), // "Picto::RasterBasePlotHandler"
QT_MOC_LITERAL(1, 29, 14), // "initializePlot"
QT_MOC_LITERAL(2, 44, 0), // ""
QT_MOC_LITERAL(3, 45, 14), // "bDisplayLegend"
QT_MOC_LITERAL(4, 60, 8), // "barColor"
QT_MOC_LITERAL(5, 69, 11), // "canvasColor"
QT_MOC_LITERAL(6, 81, 16), // "updateProperties"
QT_MOC_LITERAL(7, 98, 7), // "setName"
QT_MOC_LITERAL(8, 106, 5), // "color"
QT_MOC_LITERAL(9, 112, 9), // "setPoints"
QT_MOC_LITERAL(10, 122, 16), // "QVector<QPointF>"
QT_MOC_LITERAL(11, 139, 8), // "qvPoints"
QT_MOC_LITERAL(12, 148, 9), // "scaleAxis"
QT_MOC_LITERAL(13, 158, 8), // "dBinSize"
QT_MOC_LITERAL(14, 167, 13), // "handleXLabels"
QT_MOC_LITERAL(15, 181, 10), // "callReplot"
QT_MOC_LITERAL(16, 192, 13), // "exportClicked"
QT_MOC_LITERAL(17, 206, 11), // "rowsClicked"
QT_MOC_LITERAL(18, 218, 11), // "newNbOfRows"
QT_MOC_LITERAL(19, 230, 9) // "resetSlot"

    },
    "Picto::RasterBasePlotHandler\0"
    "initializePlot\0\0bDisplayLegend\0barColor\0"
    "canvasColor\0updateProperties\0setName\0"
    "color\0setPoints\0QVector<QPointF>\0"
    "qvPoints\0scaleAxis\0dBinSize\0handleXLabels\0"
    "callReplot\0exportClicked\0rowsClicked\0"
    "newNbOfRows\0resetSlot"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Picto__RasterBasePlotHandler[] = {

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
       1,    3,   59,    2, 0x0a /* Public */,
       6,    2,   66,    2, 0x0a /* Public */,
       9,    2,   71,    2, 0x0a /* Public */,
      12,    1,   76,    2, 0x0a /* Public */,
      14,    2,   79,    2, 0x0a /* Public */,
      15,    1,   84,    2, 0x0a /* Public */,
      16,    0,   87,    2, 0x0a /* Public */,
      17,    1,   88,    2, 0x0a /* Public */,
      19,    0,   91,    2, 0x0a /* Public */,

 // slots: parameters
    QMetaType::Void, QMetaType::Bool, QMetaType::QColor, QMetaType::QColor,    3,    4,    5,
    QMetaType::Void, QMetaType::QString, QMetaType::QColor,    7,    8,
    QMetaType::Void, QMetaType::QString, 0x80000000 | 10,    7,   11,
    QMetaType::Void, QMetaType::Double,   13,
    QMetaType::Void, QMetaType::Long, QMetaType::Long,    2,    2,
    QMetaType::Void, QMetaType::QString,    7,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,   18,
    QMetaType::Void,

       0        // eod
};

void Picto::RasterBasePlotHandler::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        RasterBasePlotHandler *_t = static_cast<RasterBasePlotHandler *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->initializePlot((*reinterpret_cast< bool(*)>(_a[1])),(*reinterpret_cast< const QColor(*)>(_a[2])),(*reinterpret_cast< const QColor(*)>(_a[3]))); break;
        case 1: _t->updateProperties((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< const QColor(*)>(_a[2]))); break;
        case 2: _t->setPoints((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< const QVector<QPointF>(*)>(_a[2]))); break;
        case 3: _t->scaleAxis((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 4: _t->handleXLabels((*reinterpret_cast< long(*)>(_a[1])),(*reinterpret_cast< long(*)>(_a[2]))); break;
        case 5: _t->callReplot((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 6: _t->exportClicked(); break;
        case 7: _t->rowsClicked((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 8: _t->resetSlot(); break;
        default: ;
        }
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<int*>(_a[0]) = -1; break;
        case 2:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 1:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QVector<QPointF> >(); break;
            }
            break;
        }
    }
}

const QMetaObject Picto::RasterBasePlotHandler::staticMetaObject = {
    { &OperatorPlotHandler::staticMetaObject, qt_meta_stringdata_Picto__RasterBasePlotHandler.data,
      qt_meta_data_Picto__RasterBasePlotHandler,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *Picto::RasterBasePlotHandler::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Picto::RasterBasePlotHandler::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_Picto__RasterBasePlotHandler.stringdata0))
        return static_cast<void*>(const_cast< RasterBasePlotHandler*>(this));
    return OperatorPlotHandler::qt_metacast(_clname);
}

int Picto::RasterBasePlotHandler::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = OperatorPlotHandler::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 9)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 9;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 9)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 9;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
