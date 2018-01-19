/****************************************************************************
** Meta object code from reading C++ file 'PlotViewWidget.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.5.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../source/workstation/DataViewer/PlotViewWidget.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'PlotViewWidget.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.5.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_PlotViewWidget_t {
    QByteArrayData data[9];
    char stringdata0[90];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_PlotViewWidget_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_PlotViewWidget_t qt_meta_stringdata_PlotViewWidget = {
    {
QT_MOC_LITERAL(0, 0, 14), // "PlotViewWidget"
QT_MOC_LITERAL(1, 15, 12), // "openInNewTab"
QT_MOC_LITERAL(2, 28, 0), // ""
QT_MOC_LITERAL(3, 29, 8), // "QWidget*"
QT_MOC_LITERAL(4, 38, 7), // "pWidget"
QT_MOC_LITERAL(5, 46, 6), // "newTab"
QT_MOC_LITERAL(6, 53, 21), // "mouseDoubleClickEvent"
QT_MOC_LITERAL(7, 75, 12), // "QMouseEvent*"
QT_MOC_LITERAL(8, 88, 1) // "e"

    },
    "PlotViewWidget\0openInNewTab\0\0QWidget*\0"
    "pWidget\0newTab\0mouseDoubleClickEvent\0"
    "QMouseEvent*\0e"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_PlotViewWidget[] = {

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
       5,    0,   32,    2, 0x0a /* Public */,
       6,    1,   33,    2, 0x0a /* Public */,

 // signals: parameters
    QMetaType::Void, 0x80000000 | 3,    4,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 7,    8,

       0        // eod
};

void PlotViewWidget::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        PlotViewWidget *_t = static_cast<PlotViewWidget *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->openInNewTab((*reinterpret_cast< QWidget*(*)>(_a[1]))); break;
        case 1: _t->newTab(); break;
        case 2: _t->mouseDoubleClickEvent((*reinterpret_cast< QMouseEvent*(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<int*>(_a[0]) = -1; break;
        case 0:
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
            typedef void (PlotViewWidget::*_t)(QWidget * );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&PlotViewWidget::openInNewTab)) {
                *result = 0;
            }
        }
    }
}

const QMetaObject PlotViewWidget::staticMetaObject = {
    { &DataViewWidget::staticMetaObject, qt_meta_stringdata_PlotViewWidget.data,
      qt_meta_data_PlotViewWidget,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *PlotViewWidget::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *PlotViewWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_PlotViewWidget.stringdata0))
        return static_cast<void*>(const_cast< PlotViewWidget*>(this));
    return DataViewWidget::qt_metacast(_clname);
}

int PlotViewWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = DataViewWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 3)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 3;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 3)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 3;
    }
    return _id;
}

// SIGNAL 0
void PlotViewWidget::openInNewTab(QWidget * _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_END_MOC_NAMESPACE
