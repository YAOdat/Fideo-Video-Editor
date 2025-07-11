/****************************************************************************
** Meta object code from reading C++ file 'TrackWidget.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.12.8)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "timeline/TrackWidget.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#include <QtCore/QList>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'TrackWidget.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.12.8. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_TrackWidget_t {
    QByteArrayData data[20];
    char stringdata0[239];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_TrackWidget_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_TrackWidget_t qt_meta_stringdata_TrackWidget = {
    {
QT_MOC_LITERAL(0, 0, 11), // "TrackWidget"
QT_MOC_LITERAL(1, 12, 17), // "trackStateChanged"
QT_MOC_LITERAL(2, 30, 0), // ""
QT_MOC_LITERAL(3, 31, 11), // "trackNumber"
QT_MOC_LITERAL(4, 43, 6), // "locked"
QT_MOC_LITERAL(5, 50, 5), // "muted"
QT_MOC_LITERAL(6, 56, 4), // "solo"
QT_MOC_LITERAL(7, 61, 20), // "clipSelectionChanged"
QT_MOC_LITERAL(8, 82, 18), // "QList<ClipWidget*>"
QT_MOC_LITERAL(9, 101, 8), // "selected"
QT_MOC_LITERAL(10, 110, 15), // "timelineChanged"
QT_MOC_LITERAL(11, 126, 11), // "clipResized"
QT_MOC_LITERAL(12, 138, 17), // "clipDoubleClicked"
QT_MOC_LITERAL(13, 156, 9), // "clipIndex"
QT_MOC_LITERAL(14, 166, 10), // "toggleLock"
QT_MOC_LITERAL(15, 177, 10), // "toggleMute"
QT_MOC_LITERAL(16, 188, 10), // "toggleSolo"
QT_MOC_LITERAL(17, 199, 16), // "enableRippleEdit"
QT_MOC_LITERAL(18, 216, 7), // "enabled"
QT_MOC_LITERAL(19, 224, 14) // "setSnapEnabled"

    },
    "TrackWidget\0trackStateChanged\0\0"
    "trackNumber\0locked\0muted\0solo\0"
    "clipSelectionChanged\0QList<ClipWidget*>\0"
    "selected\0timelineChanged\0clipResized\0"
    "clipDoubleClicked\0clipIndex\0toggleLock\0"
    "toggleMute\0toggleSolo\0enableRippleEdit\0"
    "enabled\0setSnapEnabled"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_TrackWidget[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
      10,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       5,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    4,   64,    2, 0x06 /* Public */,
       7,    1,   73,    2, 0x06 /* Public */,
      10,    0,   76,    2, 0x06 /* Public */,
      11,    0,   77,    2, 0x06 /* Public */,
      12,    1,   78,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
      14,    0,   81,    2, 0x0a /* Public */,
      15,    0,   82,    2, 0x0a /* Public */,
      16,    0,   83,    2, 0x0a /* Public */,
      17,    1,   84,    2, 0x0a /* Public */,
      19,    1,   87,    2, 0x0a /* Public */,

 // signals: parameters
    QMetaType::Void, QMetaType::Int, QMetaType::Bool, QMetaType::Bool, QMetaType::Bool,    3,    4,    5,    6,
    QMetaType::Void, 0x80000000 | 8,    9,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,   13,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Bool,   18,
    QMetaType::Void, QMetaType::Bool,   18,

       0        // eod
};

void TrackWidget::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<TrackWidget *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->trackStateChanged((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< bool(*)>(_a[2])),(*reinterpret_cast< bool(*)>(_a[3])),(*reinterpret_cast< bool(*)>(_a[4]))); break;
        case 1: _t->clipSelectionChanged((*reinterpret_cast< const QList<ClipWidget*>(*)>(_a[1]))); break;
        case 2: _t->timelineChanged(); break;
        case 3: _t->clipResized(); break;
        case 4: _t->clipDoubleClicked((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 5: _t->toggleLock(); break;
        case 6: _t->toggleMute(); break;
        case 7: _t->toggleSolo(); break;
        case 8: _t->enableRippleEdit((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 9: _t->setSnapEnabled((*reinterpret_cast< bool(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (TrackWidget::*)(int , bool , bool , bool );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&TrackWidget::trackStateChanged)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (TrackWidget::*)(const QList<ClipWidget*> & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&TrackWidget::clipSelectionChanged)) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (TrackWidget::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&TrackWidget::timelineChanged)) {
                *result = 2;
                return;
            }
        }
        {
            using _t = void (TrackWidget::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&TrackWidget::clipResized)) {
                *result = 3;
                return;
            }
        }
        {
            using _t = void (TrackWidget::*)(int );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&TrackWidget::clipDoubleClicked)) {
                *result = 4;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject TrackWidget::staticMetaObject = { {
    &QWidget::staticMetaObject,
    qt_meta_stringdata_TrackWidget.data,
    qt_meta_data_TrackWidget,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *TrackWidget::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *TrackWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_TrackWidget.stringdata0))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int TrackWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 10)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 10;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 10)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 10;
    }
    return _id;
}

// SIGNAL 0
void TrackWidget::trackStateChanged(int _t1, bool _t2, bool _t3, bool _t4)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)), const_cast<void*>(reinterpret_cast<const void*>(&_t4)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void TrackWidget::clipSelectionChanged(const QList<ClipWidget*> & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void TrackWidget::timelineChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 2, nullptr);
}

// SIGNAL 3
void TrackWidget::clipResized()
{
    QMetaObject::activate(this, &staticMetaObject, 3, nullptr);
}

// SIGNAL 4
void TrackWidget::clipDoubleClicked(int _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 4, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
