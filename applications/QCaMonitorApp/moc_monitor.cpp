/****************************************************************************
** Meta object code from reading C++ file 'monitor.h'
**
** Created: Thu Sep 17 12:11:45 2009
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "include/monitor.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'monitor.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_monitor[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   12, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // slots: signature, parameters, type, tag, flags
      35,    9,    8,    8, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_monitor[] = {
    "monitor\0\0data,alarmInfo,timeStamp,\0"
    "log(QString,QCaAlarmInfo&,QCaDateTime&,uint)\0"
};

const QMetaObject monitor::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_monitor,
      qt_meta_data_monitor, 0 }
};

const QMetaObject *monitor::metaObject() const
{
    return &staticMetaObject;
}

void *monitor::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_monitor))
        return static_cast<void*>(const_cast< monitor*>(this));
    return QObject::qt_metacast(_clname);
}

int monitor::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: log((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< QCaAlarmInfo(*)>(_a[2])),(*reinterpret_cast< QCaDateTime(*)>(_a[3])),(*reinterpret_cast< const uint(*)>(_a[4]))); break;
        default: ;
        }
        _id -= 1;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
