/****************************************************************************
** Meta object code from reading C++ file 'TextGraphic.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.5.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../source/common/stimuli/TextGraphic.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'TextGraphic.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.5.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_Picto__TextGraphic_t {
    QByteArrayData data[11];
    char stringdata0[79];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Picto__TextGraphic_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Picto__TextGraphic_t qt_meta_stringdata_Picto__TextGraphic = {
    {
QT_MOC_LITERAL(0, 0, 18), // "Picto::TextGraphic"
QT_MOC_LITERAL(1, 19, 13), // "setDimensions"
QT_MOC_LITERAL(2, 33, 0), // ""
QT_MOC_LITERAL(3, 34, 1), // "w"
QT_MOC_LITERAL(4, 36, 1), // "h"
QT_MOC_LITERAL(5, 38, 11), // "setFontSize"
QT_MOC_LITERAL(6, 50, 1), // "s"
QT_MOC_LITERAL(7, 52, 5), // "width"
QT_MOC_LITERAL(8, 58, 6), // "height"
QT_MOC_LITERAL(9, 65, 4), // "text"
QT_MOC_LITERAL(10, 70, 8) // "FontSize"

    },
    "Picto::TextGraphic\0setDimensions\0\0w\0"
    "h\0setFontSize\0s\0width\0height\0text\0"
    "FontSize"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Picto__TextGraphic[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       2,   14, // methods
       4,   32, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    2,   24,    2, 0x0a /* Public */,
       5,    1,   29,    2, 0x0a /* Public */,

 // slots: parameters
    QMetaType::Void, QMetaType::Int, QMetaType::Int,    3,    4,
    QMetaType::Void, QMetaType::Int,    6,

 // properties: name, type, flags
       7, QMetaType::Int, 0x00095103,
       8, QMetaType::Int, 0x00095103,
       9, QMetaType::QString, 0x00095103,
      10, QMetaType::Int, 0x00095003,

       0        // eod
};

void Picto::TextGraphic::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        TextGraphic *_t = static_cast<TextGraphic *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->setDimensions((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 1: _t->setFontSize((*reinterpret_cast< int(*)>(_a[1]))); break;
        default: ;
        }
    }
#ifndef QT_NO_PROPERTIES
    else if (_c == QMetaObject::ReadProperty) {
        TextGraphic *_t = static_cast<TextGraphic *>(_o);
        Q_UNUSED(_t)
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< int*>(_v) = _t->getWidth(); break;
        case 1: *reinterpret_cast< int*>(_v) = _t->getHeight(); break;
        case 2: *reinterpret_cast< QString*>(_v) = _t->getText(); break;
        case 3: *reinterpret_cast< int*>(_v) = _t->getSize(); break;
        default: break;
        }
    } else if (_c == QMetaObject::WriteProperty) {
        TextGraphic *_t = static_cast<TextGraphic *>(_o);
        Q_UNUSED(_t)
        void *_v = _a[0];
        switch (_id) {
        case 0: _t->setWidth(*reinterpret_cast< int*>(_v)); break;
        case 1: _t->setHeight(*reinterpret_cast< int*>(_v)); break;
        case 2: _t->setText(*reinterpret_cast< QString*>(_v)); break;
        case 3: _t->setSize(*reinterpret_cast< int*>(_v)); break;
        default: break;
        }
    } else if (_c == QMetaObject::ResetProperty) {
    }
#endif // QT_NO_PROPERTIES
}

const QMetaObject Picto::TextGraphic::staticMetaObject = {
    { &VisualElement::staticMetaObject, qt_meta_stringdata_Picto__TextGraphic.data,
      qt_meta_data_Picto__TextGraphic,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *Picto::TextGraphic::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Picto::TextGraphic::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_Picto__TextGraphic.stringdata0))
        return static_cast<void*>(const_cast< TextGraphic*>(this));
    return VisualElement::qt_metacast(_clname);
}

int Picto::TextGraphic::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = VisualElement::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 2)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 2;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 2)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 2;
    }
#ifndef QT_NO_PROPERTIES
   else if (_c == QMetaObject::ReadProperty || _c == QMetaObject::WriteProperty
            || _c == QMetaObject::ResetProperty || _c == QMetaObject::RegisterPropertyMetaType) {
        qt_static_metacall(this, _c, _id, _a);
        _id -= 4;
    } else if (_c == QMetaObject::QueryPropertyDesignable) {
        _id -= 4;
    } else if (_c == QMetaObject::QueryPropertyScriptable) {
        _id -= 4;
    } else if (_c == QMetaObject::QueryPropertyStored) {
        _id -= 4;
    } else if (_c == QMetaObject::QueryPropertyEditable) {
        _id -= 4;
    } else if (_c == QMetaObject::QueryPropertyUser) {
        _id -= 4;
    }
#endif // QT_NO_PROPERTIES
    return _id;
}
QT_END_MOC_NAMESPACE
