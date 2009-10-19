/****************************************************************************
** Meta object code from reading C++ file 'QCaString.h'
**
** Created: Thu Sep 17 12:11:45 2009
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../ca_framework/data/include/QCaString.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'QCaString.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_QCaString[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
       3,   12, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // signals: signature, parameters, type, tag, flags
      51,   11,   10,   10, 0x05,

 // slots: signature, parameters, type, tag, flags
     111,  106,   10,   10, 0x0a,
     158,  132,   10,   10, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_QCaString[] = {
    "QCaString\0\0value,alarmInfo,timeStamp,variableIndex\0"
    "stringChanged(QString,QCaAlarmInfo&,QCaDateTime&,uint)\0"
    "data\0writeString(QString)\0"
    "value,alarmInfo,timeStamp\0"
    "convertVariant(QVariant,QCaAlarmInfo&,QCaDateTime&)\0"
};

const QMetaObject QCaString::staticMetaObject = {
    { &qcaobject::QCaObject::staticMetaObject, qt_meta_stringdata_QCaString,
      qt_meta_data_QCaString, 0 }
};

const QMetaObject *QCaString::metaObject() const
{
    return &staticMetaObject;
}

void *QCaString::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_QCaString))
        return static_cast<void*>(const_cast< QCaString*>(this));
    typedef qcaobject::QCaObject QMocSuperClass;
    return QMocSuperClass::qt_metacast(_clname);
}

int QCaString::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    typedef qcaobject::QCaObject QMocSuperClass;
    _id = QMocSuperClass::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: stringChanged((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< QCaAlarmInfo(*)>(_a[2])),(*reinterpret_cast< QCaDateTime(*)>(_a[3])),(*reinterpret_cast< const uint(*)>(_a[4]))); break;
        case 1: writeString((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 2: convertVariant((*reinterpret_cast< const QVariant(*)>(_a[1])),(*reinterpret_cast< QCaAlarmInfo(*)>(_a[2])),(*reinterpret_cast< QCaDateTime(*)>(_a[3]))); break;
        default: ;
        }
        _id -= 3;
    }
    return _id;
}

// SIGNAL 0
void QCaString::stringChanged(const QString & _t1, QCaAlarmInfo & _t2, QCaDateTime & _t3, const unsigned int & _t4)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)), const_cast<void*>(reinterpret_cast<const void*>(&_t4)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_END_MOC_NAMESPACE
