/****************************************************************************
** Meta object code from reading C++ file 'ViewerWindow.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.5.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../source/common/designer/ViewerWindow.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'ViewerWindow.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.5.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_ViewerWindow_t {
    QByteArrayData data[7];
    char stringdata0[66];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_ViewerWindow_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_ViewerWindow_t qt_meta_stringdata_ViewerWindow = {
    {
QT_MOC_LITERAL(0, 0, 12), // "ViewerWindow"
QT_MOC_LITERAL(1, 13, 7), // "setZoom"
QT_MOC_LITERAL(2, 21, 0), // ""
QT_MOC_LITERAL(3, 22, 4), // "zoom"
QT_MOC_LITERAL(4, 27, 15), // "editModeChanged"
QT_MOC_LITERAL(5, 43, 4), // "mode"
QT_MOC_LITERAL(6, 48, 17) // "zoomToFitContents"

    },
    "ViewerWindow\0setZoom\0\0zoom\0editModeChanged\0"
    "mode\0zoomToFitContents"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_ViewerWindow[] = {

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
       1,    1,   29,    2, 0x08 /* Private */,
       4,    1,   32,    2, 0x08 /* Private */,
       6,    0,   35,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void, QMetaType::Double,    3,
    QMetaType::Void, QMetaType::Int,    5,
    QMetaType::Void,

       0        // eod
};

void ViewerWindow::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        ViewerWindow *_t = static_cast<ViewerWindow *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->setZoom((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 1: _t->editModeChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 2: _t->zoomToFitContents(); break;
        default: ;
        }
    }
}

const QMetaObject ViewerWindow::staticMetaObject = {
    { &QGraphicsView::staticMetaObject, qt_meta_stringdata_ViewerWindow.data,
      qt_meta_data_ViewerWindow,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *ViewerWindow::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *ViewerWindow::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_ViewerWindow.stringdata0))
        return static_cast<void*>(const_cast< ViewerWindow*>(this));
    return QGraphicsView::qt_metacast(_clname);
}

int ViewerWindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QGraphicsView::qt_metacall(_c, _id, _a);
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
