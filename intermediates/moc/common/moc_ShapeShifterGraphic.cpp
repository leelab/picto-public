/****************************************************************************
** Meta object code from reading C++ file 'ShapeShifterGraphic.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.5.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../source/common/stimuli/ShapeShifterGraphic.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'ShapeShifterGraphic.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.5.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_Picto__ShapeShifterGraphic_t {
    QByteArrayData data[10];
    char stringdata0[83];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Picto__ShapeShifterGraphic_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Picto__ShapeShifterGraphic_t qt_meta_stringdata_Picto__ShapeShifterGraphic = {
    {
QT_MOC_LITERAL(0, 0, 26), // "Picto::ShapeShifterGraphic"
QT_MOC_LITERAL(1, 27, 13), // "setDimensions"
QT_MOC_LITERAL(2, 41, 0), // ""
QT_MOC_LITERAL(3, 42, 1), // "w"
QT_MOC_LITERAL(4, 44, 1), // "h"
QT_MOC_LITERAL(5, 46, 8), // "getShape"
QT_MOC_LITERAL(6, 55, 8), // "setShape"
QT_MOC_LITERAL(7, 64, 5), // "shape"
QT_MOC_LITERAL(8, 70, 5), // "width"
QT_MOC_LITERAL(9, 76, 6) // "height"

    },
    "Picto::ShapeShifterGraphic\0setDimensions\0"
    "\0w\0h\0getShape\0setShape\0shape\0width\0"
    "height"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Picto__ShapeShifterGraphic[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       3,   14, // methods
       3,   38, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    2,   29,    2, 0x0a /* Public */,
       5,    0,   34,    2, 0x0a /* Public */,
       6,    1,   35,    2, 0x0a /* Public */,

 // slots: parameters
    QMetaType::Void, QMetaType::Int, QMetaType::Int,    3,    4,
    QMetaType::QString,
    QMetaType::Void, QMetaType::QString,    7,

 // properties: name, type, flags
       8, QMetaType::Int, 0x00095103,
       9, QMetaType::Int, 0x00095103,
       7, QMetaType::QString, 0x00095103,

       0        // eod
};

void Picto::ShapeShifterGraphic::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        ShapeShifterGraphic *_t = static_cast<ShapeShifterGraphic *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->setDimensions((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 1: { QString _r = _t->getShape();
            if (_a[0]) *reinterpret_cast< QString*>(_a[0]) = _r; }  break;
        case 2: _t->setShape((*reinterpret_cast< QString(*)>(_a[1]))); break;
        default: ;
        }
    }
#ifndef QT_NO_PROPERTIES
    else if (_c == QMetaObject::ReadProperty) {
        ShapeShifterGraphic *_t = static_cast<ShapeShifterGraphic *>(_o);
        Q_UNUSED(_t)
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< int*>(_v) = _t->getWidth(); break;
        case 1: *reinterpret_cast< int*>(_v) = _t->getHeight(); break;
        case 2: *reinterpret_cast< QString*>(_v) = _t->getShape(); break;
        default: break;
        }
    } else if (_c == QMetaObject::WriteProperty) {
        ShapeShifterGraphic *_t = static_cast<ShapeShifterGraphic *>(_o);
        Q_UNUSED(_t)
        void *_v = _a[0];
        switch (_id) {
        case 0: _t->setWidth(*reinterpret_cast< int*>(_v)); break;
        case 1: _t->setHeight(*reinterpret_cast< int*>(_v)); break;
        case 2: _t->setShape(*reinterpret_cast< QString*>(_v)); break;
        default: break;
        }
    } else if (_c == QMetaObject::ResetProperty) {
    }
#endif // QT_NO_PROPERTIES
}

const QMetaObject Picto::ShapeShifterGraphic::staticMetaObject = {
    { &VisualElement::staticMetaObject, qt_meta_stringdata_Picto__ShapeShifterGraphic.data,
      qt_meta_data_Picto__ShapeShifterGraphic,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *Picto::ShapeShifterGraphic::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Picto::ShapeShifterGraphic::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_Picto__ShapeShifterGraphic.stringdata0))
        return static_cast<void*>(const_cast< ShapeShifterGraphic*>(this));
    return VisualElement::qt_metacast(_clname);
}

int Picto::ShapeShifterGraphic::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = VisualElement::qt_metacall(_c, _id, _a);
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
#ifndef QT_NO_PROPERTIES
   else if (_c == QMetaObject::ReadProperty || _c == QMetaObject::WriteProperty
            || _c == QMetaObject::ResetProperty || _c == QMetaObject::RegisterPropertyMetaType) {
        qt_static_metacall(this, _c, _id, _a);
        _id -= 3;
    } else if (_c == QMetaObject::QueryPropertyDesignable) {
        _id -= 3;
    } else if (_c == QMetaObject::QueryPropertyScriptable) {
        _id -= 3;
    } else if (_c == QMetaObject::QueryPropertyStored) {
        _id -= 3;
    } else if (_c == QMetaObject::QueryPropertyEditable) {
        _id -= 3;
    } else if (_c == QMetaObject::QueryPropertyUser) {
        _id -= 3;
    }
#endif // QT_NO_PROPERTIES
    return _id;
}
QT_END_MOC_NAMESPACE
