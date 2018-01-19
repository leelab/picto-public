/****************************************************************************
** Meta object code from reading C++ file 'AudioElement.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.5.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../source/common/stimuli/AudioElement.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'AudioElement.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.5.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_Picto__AudioElement_t {
    QByteArrayData data[14];
    char stringdata0[152];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Picto__AudioElement_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Picto__AudioElement_t qt_meta_stringdata_Picto__AudioElement = {
    {
QT_MOC_LITERAL(0, 0, 19), // "Picto::AudioElement"
QT_MOC_LITERAL(1, 20, 4), // "play"
QT_MOC_LITERAL(2, 25, 0), // ""
QT_MOC_LITERAL(3, 26, 4), // "stop"
QT_MOC_LITERAL(4, 31, 9), // "isPlaying"
QT_MOC_LITERAL(5, 41, 9), // "cacheFile"
QT_MOC_LITERAL(6, 51, 15), // "fileElementName"
QT_MOC_LITERAL(7, 67, 16), // "audioFileChanged"
QT_MOC_LITERAL(8, 84, 9), // "Property*"
QT_MOC_LITERAL(9, 94, 14), // "commandChanged"
QT_MOC_LITERAL(10, 109, 11), // "playOccured"
QT_MOC_LITERAL(11, 121, 13), // "mixSampleName"
QT_MOC_LITERAL(12, 135, 11), // "stopOccured"
QT_MOC_LITERAL(13, 147, 4) // "file"

    },
    "Picto::AudioElement\0play\0\0stop\0isPlaying\0"
    "cacheFile\0fileElementName\0audioFileChanged\0"
    "Property*\0commandChanged\0playOccured\0"
    "mixSampleName\0stopOccured\0file"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Picto__AudioElement[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       8,   14, // methods
       1,   76, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,   54,    2, 0x0a /* Public */,
       3,    0,   55,    2, 0x0a /* Public */,
       4,    0,   56,    2, 0x0a /* Public */,
       5,    1,   57,    2, 0x0a /* Public */,
       7,    2,   60,    2, 0x08 /* Private */,
       9,    2,   65,    2, 0x08 /* Private */,
      10,    1,   70,    2, 0x08 /* Private */,
      12,    1,   73,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Bool,
    QMetaType::Void, QMetaType::QString,    6,
    QMetaType::Void, 0x80000000 | 8, QMetaType::QVariant,    2,    2,
    QMetaType::Void, 0x80000000 | 8, QMetaType::QVariant,    2,    2,
    QMetaType::Void, QMetaType::QString,   11,
    QMetaType::Void, QMetaType::QString,   11,

 // properties: name, type, flags
      13, QMetaType::QString, 0x00095103,

       0        // eod
};

void Picto::AudioElement::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        AudioElement *_t = static_cast<AudioElement *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->play(); break;
        case 1: _t->stop(); break;
        case 2: { bool _r = _t->isPlaying();
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = _r; }  break;
        case 3: _t->cacheFile((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 4: _t->audioFileChanged((*reinterpret_cast< Property*(*)>(_a[1])),(*reinterpret_cast< QVariant(*)>(_a[2]))); break;
        case 5: _t->commandChanged((*reinterpret_cast< Property*(*)>(_a[1])),(*reinterpret_cast< QVariant(*)>(_a[2]))); break;
        case 6: _t->playOccured((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 7: _t->stopOccured((*reinterpret_cast< QString(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<int*>(_a[0]) = -1; break;
        case 4:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< Property* >(); break;
            }
            break;
        case 5:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< Property* >(); break;
            }
            break;
        }
    }
#ifndef QT_NO_PROPERTIES
    else if (_c == QMetaObject::ReadProperty) {
        AudioElement *_t = static_cast<AudioElement *>(_o);
        Q_UNUSED(_t)
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< QString*>(_v) = _t->getFile(); break;
        default: break;
        }
    } else if (_c == QMetaObject::WriteProperty) {
        AudioElement *_t = static_cast<AudioElement *>(_o);
        Q_UNUSED(_t)
        void *_v = _a[0];
        switch (_id) {
        case 0: _t->setFile(*reinterpret_cast< QString*>(_v)); break;
        default: break;
        }
    } else if (_c == QMetaObject::ResetProperty) {
    }
#endif // QT_NO_PROPERTIES
}

const QMetaObject Picto::AudioElement::staticMetaObject = {
    { &OutputElement::staticMetaObject, qt_meta_stringdata_Picto__AudioElement.data,
      qt_meta_data_Picto__AudioElement,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *Picto::AudioElement::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Picto::AudioElement::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_Picto__AudioElement.stringdata0))
        return static_cast<void*>(const_cast< AudioElement*>(this));
    return OutputElement::qt_metacast(_clname);
}

int Picto::AudioElement::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = OutputElement::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 8)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 8;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 8)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 8;
    }
#ifndef QT_NO_PROPERTIES
   else if (_c == QMetaObject::ReadProperty || _c == QMetaObject::WriteProperty
            || _c == QMetaObject::ResetProperty || _c == QMetaObject::RegisterPropertyMetaType) {
        qt_static_metacall(this, _c, _id, _a);
        _id -= 1;
    } else if (_c == QMetaObject::QueryPropertyDesignable) {
        _id -= 1;
    } else if (_c == QMetaObject::QueryPropertyScriptable) {
        _id -= 1;
    } else if (_c == QMetaObject::QueryPropertyStored) {
        _id -= 1;
    } else if (_c == QMetaObject::QueryPropertyEditable) {
        _id -= 1;
    } else if (_c == QMetaObject::QueryPropertyUser) {
        _id -= 1;
    }
#endif // QT_NO_PROPERTIES
    return _id;
}
QT_END_MOC_NAMESPACE
