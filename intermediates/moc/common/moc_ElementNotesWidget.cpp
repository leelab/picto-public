/****************************************************************************
** Meta object code from reading C++ file 'ElementNotesWidget.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.5.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../source/common/designer/ElementNotesWidget.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#include <QtCore/QSharedPointer>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'ElementNotesWidget.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.5.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_ElementNotesWidget_t {
    QByteArrayData data[11];
    char stringdata0[156];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_ElementNotesWidget_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_ElementNotesWidget_t qt_meta_stringdata_ElementNotesWidget = {
    {
QT_MOC_LITERAL(0, 0, 18), // "ElementNotesWidget"
QT_MOC_LITERAL(1, 19, 20), // "selectedAssetChanged"
QT_MOC_LITERAL(2, 40, 0), // ""
QT_MOC_LITERAL(3, 41, 21), // "QSharedPointer<Asset>"
QT_MOC_LITERAL(4, 63, 5), // "asset"
QT_MOC_LITERAL(5, 69, 15), // "notesWereEdited"
QT_MOC_LITERAL(6, 85, 15), // "searchRequested"
QT_MOC_LITERAL(7, 101, 13), // "SearchRequest"
QT_MOC_LITERAL(8, 115, 13), // "searchRequest"
QT_MOC_LITERAL(9, 129, 14), // "notesLostFocus"
QT_MOC_LITERAL(10, 144, 11) // "clearAssets"

    },
    "ElementNotesWidget\0selectedAssetChanged\0"
    "\0QSharedPointer<Asset>\0asset\0"
    "notesWereEdited\0searchRequested\0"
    "SearchRequest\0searchRequest\0notesLostFocus\0"
    "clearAssets"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_ElementNotesWidget[] = {

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
       1,    1,   39,    2, 0x08 /* Private */,
       5,    0,   42,    2, 0x08 /* Private */,
       6,    1,   43,    2, 0x08 /* Private */,
       9,    0,   46,    2, 0x08 /* Private */,
      10,    0,   47,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void, 0x80000000 | 3,    4,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 7,    8,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void ElementNotesWidget::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        ElementNotesWidget *_t = static_cast<ElementNotesWidget *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->selectedAssetChanged((*reinterpret_cast< QSharedPointer<Asset>(*)>(_a[1]))); break;
        case 1: _t->notesWereEdited(); break;
        case 2: _t->searchRequested((*reinterpret_cast< SearchRequest(*)>(_a[1]))); break;
        case 3: _t->notesLostFocus(); break;
        case 4: _t->clearAssets(); break;
        default: ;
        }
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<int*>(_a[0]) = -1; break;
        case 0:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QSharedPointer<Asset> >(); break;
            }
            break;
        }
    }
}

const QMetaObject ElementNotesWidget::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_ElementNotesWidget.data,
      qt_meta_data_ElementNotesWidget,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *ElementNotesWidget::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *ElementNotesWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_ElementNotesWidget.stringdata0))
        return static_cast<void*>(const_cast< ElementNotesWidget*>(this));
    return QWidget::qt_metacast(_clname);
}

int ElementNotesWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 5)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 5;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 5)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 5;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
