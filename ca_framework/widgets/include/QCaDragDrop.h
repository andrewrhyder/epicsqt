#ifndef QCADRAGDROP_H
#define QCADRAGDROP_H

#include <QDropEvent>
#include <QDragEnterEvent>
#include <QMouseEvent>

class QCaDragDrop
{
public:
    QCaDragDrop( QWidget* ){}
    void qcaDragEnterEvent( QDragEnterEvent* ){}
    void qcaDropEvent( QDropEvent* ){}
    void qcaMousePressEvent( QMouseEvent* ){}
};

#endif // QCADRAGDROP_H
