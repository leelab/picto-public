/****************************************************************************
** Meta object code from reading C++ file 'DataViewOrganizer.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.5.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../source/workstation/DataViewer/DataViewOrganizer.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'DataViewOrganizer.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.5.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_DataViewOrganizer_t {
    QByteArrayData data[13];
    char stringdata0[134];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_DataViewOrganizer_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_DataViewOrganizer_t qt_meta_stringdata_DataViewOrganizer = {
    {
QT_MOC_LITERAL(0, 0, 17), // "DataViewOrganizer"
QT_MOC_LITERAL(1, 18, 13), // "widgetDropped"
QT_MOC_LITERAL(2, 32, 0), // ""
QT_MOC_LITERAL(3, 33, 6), // "offset"
QT_MOC_LITERAL(4, 40, 13), // "widgetDragged"
QT_MOC_LITERAL(5, 54, 8), // "QWidget*"
QT_MOC_LITERAL(6, 63, 7), // "pWidget"
QT_MOC_LITERAL(7, 71, 16), // "removeWidgetSlot"
QT_MOC_LITERAL(8, 88, 13), // "addWidgetSlot"
QT_MOC_LITERAL(9, 102, 1), // "x"
QT_MOC_LITERAL(10, 104, 1), // "y"
QT_MOC_LITERAL(11, 106, 22), // "DataViewSize::ViewSize"
QT_MOC_LITERAL(12, 129, 4) // "size"

    },
    "DataViewOrganizer\0widgetDropped\0\0"
    "offset\0widgetDragged\0QWidget*\0pWidget\0"
    "removeWidgetSlot\0addWidgetSlot\0x\0y\0"
    "DataViewSize::ViewSize\0size"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_DataViewOrganizer[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   34,    2, 0x06 /* Public */,
       4,    1,   37,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       7,    1,   40,    2, 0x0a /* Public */,
       8,    4,   43,    2, 0x0a /* Public */,

 // signals: parameters
    QMetaType::Void, QMetaType::QPoint,    3,
    QMetaType::Void, 0x80000000 | 5,    6,

 // slots: parameters
    QMetaType::Void, 0x80000000 | 5,    6,
    QMetaType::Void, 0x80000000 | 5, QMetaType::Int, QMetaType::Int, 0x80000000 | 11,    6,    9,   10,   12,

       0        // eod
};

void DataViewOrganizer::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        DataViewOrganizer *_t = static_cast<DataViewOrganizer *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->widgetDropped((*reinterpret_cast< QPoint(*)>(_a[1]))); break;
        case 1: _t->widgetDragged((*reinterpret_cast< QWidget*(*)>(_a[1]))); break;
        case 2: _t->removeWidgetSlot((*reinterpret_cast< QWidget*(*)>(_a[1]))); break;
        case 3: _t->addWidgetSlot((*reinterpret_cast< QWidget*(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3])),(*reinterpret_cast< DataViewSize::ViewSize(*)>(_a[4]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<int*>(_a[0]) = -1; break;
        case 1:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QWidget* >(); break;
            }
            break;
        case 2:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QWidget* >(); break;
            }
            break;
        case 3:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QWidget* >(); break;
            }
            break;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (DataViewOrganizer::*_t)(QPoint );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&DataViewOrganizer::widgetDropped)) {
                *result = 0;
            }
        }
        {
            typedef void (DataViewOrganizer::*_t)(QWidget * );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&DataViewOrganizer::widgetDragged)) {
                *result = 1;
            }
        }
    }
}

const QMetaObject DataViewOrganizer::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_DataViewOrganizer.data,
      qt_meta_data_DataViewOrganizer,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *DataViewOrganizer::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *DataViewOrganizer::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_DataViewOrganizer.stringdata0))
        return static_cast<void*>(const_cast< DataViewOrganizer*>(this));
    return QWidget::qt_metacast(_clname);
}

int DataViewOrganizer::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
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
    return _id;
}

// SIGNAL 0
void DataViewOrganizer::widgetDropped(QPoint _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void DataViewOrganizer::widgetDragged(QWidget * _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}
QT_END_MOC_NAMESPACE
