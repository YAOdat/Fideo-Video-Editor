/****************************************************************************
** Meta object code from reading C++ file 'ClipWidget.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.12.8)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "timeline/ClipWidget.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'ClipWidget.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.12.8. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_ClipWidget_t {
    QByteArrayData data[14];
    char stringdata0[164];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_ClipWidget_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_ClipWidget_t qt_meta_stringdata_ClipWidget = {
    {
QT_MOC_LITERAL(0, 0, 10), // "ClipWidget"
QT_MOC_LITERAL(1, 11, 13), // "requestRemove"
QT_MOC_LITERAL(2, 25, 0), // ""
QT_MOC_LITERAL(3, 26, 11), // "ClipWidget*"
QT_MOC_LITERAL(4, 38, 4), // "self"
QT_MOC_LITERAL(5, 43, 14), // "splitRequested"
QT_MOC_LITERAL(6, 58, 13), // "resizeStarted"
QT_MOC_LITERAL(7, 72, 14), // "resizeFinished"
QT_MOC_LITERAL(8, 87, 13), // "newStartFrame"
QT_MOC_LITERAL(9, 101, 17), // "newDurationFrames"
QT_MOC_LITERAL(10, 119, 11), // "dragStarted"
QT_MOC_LITERAL(11, 131, 9), // "dragMoved"
QT_MOC_LITERAL(12, 141, 9), // "globalPos"
QT_MOC_LITERAL(13, 151, 12) // "dragFinished"

    },
    "ClipWidget\0requestRemove\0\0ClipWidget*\0"
    "self\0splitRequested\0resizeStarted\0"
    "resizeFinished\0newStartFrame\0"
    "newDurationFrames\0dragStarted\0dragMoved\0"
    "globalPos\0dragFinished"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_ClipWidget[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       7,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       7,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   49,    2, 0x06 /* Public */,
       5,    1,   52,    2, 0x06 /* Public */,
       6,    1,   55,    2, 0x06 /* Public */,
       7,    3,   58,    2, 0x06 /* Public */,
      10,    1,   65,    2, 0x06 /* Public */,
      11,    2,   68,    2, 0x06 /* Public */,
      13,    2,   73,    2, 0x06 /* Public */,

 // signals: parameters
    QMetaType::Void, 0x80000000 | 3,    4,
    QMetaType::Void, 0x80000000 | 3,    4,
    QMetaType::Void, 0x80000000 | 3,    4,
    QMetaType::Void, 0x80000000 | 3, QMetaType::Int, QMetaType::Int,    4,    8,    9,
    QMetaType::Void, 0x80000000 | 3,    4,
    QMetaType::Void, 0x80000000 | 3, QMetaType::QPoint,    4,   12,
    QMetaType::Void, 0x80000000 | 3, QMetaType::QPoint,    4,   12,

       0        // eod
};

void ClipWidget::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<ClipWidget *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->requestRemove((*reinterpret_cast< ClipWidget*(*)>(_a[1]))); break;
        case 1: _t->splitRequested((*reinterpret_cast< ClipWidget*(*)>(_a[1]))); break;
        case 2: _t->resizeStarted((*reinterpret_cast< ClipWidget*(*)>(_a[1]))); break;
        case 3: _t->resizeFinished((*reinterpret_cast< ClipWidget*(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3]))); break;
        case 4: _t->dragStarted((*reinterpret_cast< ClipWidget*(*)>(_a[1]))); break;
        case 5: _t->dragMoved((*reinterpret_cast< ClipWidget*(*)>(_a[1])),(*reinterpret_cast< const QPoint(*)>(_a[2]))); break;
        case 6: _t->dragFinished((*reinterpret_cast< ClipWidget*(*)>(_a[1])),(*reinterpret_cast< const QPoint(*)>(_a[2]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<int*>(_a[0]) = -1; break;
        case 0:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< ClipWidget* >(); break;
            }
            break;
        case 1:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< ClipWidget* >(); break;
            }
            break;
        case 2:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< ClipWidget* >(); break;
            }
            break;
        case 3:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< ClipWidget* >(); break;
            }
            break;
        case 4:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< ClipWidget* >(); break;
            }
            break;
        case 5:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< ClipWidget* >(); break;
            }
            break;
        case 6:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< ClipWidget* >(); break;
            }
            break;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (ClipWidget::*)(ClipWidget * );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ClipWidget::requestRemove)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (ClipWidget::*)(ClipWidget * );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ClipWidget::splitRequested)) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (ClipWidget::*)(ClipWidget * );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ClipWidget::resizeStarted)) {
                *result = 2;
                return;
            }
        }
        {
            using _t = void (ClipWidget::*)(ClipWidget * , int , int );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ClipWidget::resizeFinished)) {
                *result = 3;
                return;
            }
        }
        {
            using _t = void (ClipWidget::*)(ClipWidget * );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ClipWidget::dragStarted)) {
                *result = 4;
                return;
            }
        }
        {
            using _t = void (ClipWidget::*)(ClipWidget * , const QPoint & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ClipWidget::dragMoved)) {
                *result = 5;
                return;
            }
        }
        {
            using _t = void (ClipWidget::*)(ClipWidget * , const QPoint & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ClipWidget::dragFinished)) {
                *result = 6;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject ClipWidget::staticMetaObject = { {
    &QWidget::staticMetaObject,
    qt_meta_stringdata_ClipWidget.data,
    qt_meta_data_ClipWidget,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *ClipWidget::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *ClipWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_ClipWidget.stringdata0))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int ClipWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 7)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 7;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 7)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 7;
    }
    return _id;
}

// SIGNAL 0
void ClipWidget::requestRemove(ClipWidget * _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void ClipWidget::splitRequested(ClipWidget * _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void ClipWidget::resizeStarted(ClipWidget * _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void ClipWidget::resizeFinished(ClipWidget * _t1, int _t2, int _t3)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}

// SIGNAL 4
void ClipWidget::dragStarted(ClipWidget * _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 4, _a);
}

// SIGNAL 5
void ClipWidget::dragMoved(ClipWidget * _t1, const QPoint & _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 5, _a);
}

// SIGNAL 6
void ClipWidget::dragFinished(ClipWidget * _t1, const QPoint & _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 6, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
