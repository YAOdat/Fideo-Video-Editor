#include "ClipWidget.h"
#include <QPainter>
#include <QMouseEvent>
#include <QHBoxLayout>
#include <QLabel>
#include <QFileInfo>
#include <QMenu>
#include <QStyleOption>
#include <QApplication>
#include <QDebug>
#include <QDrag>
#include <QMimeData>
#include "TimelineConstants.h"

ClipWidget::ClipWidget(Mlt::Producer* producer, const QPixmap& thumb, QWidget* parent)
    : QWidget(parent), producer(producer), thumbnail(thumb) {
    thumbLabel = new QLabel(this);
    thumbLabel->setPixmap(thumbnail.scaled(40, 40, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    thumbLabel->setFixedSize(44, 44);
    label = new QLabel(QFileInfo(QString::fromUtf8(producer->get("resource"))).fileName(), this);
    
    QHBoxLayout* layout = new QHBoxLayout(this);
    layout->setContentsMargins(2,2,2,2);
    layout->setSpacing(4);
    layout->addWidget(thumbLabel);
    layout->addWidget(label);
    setLayout(layout);
    
    setMinimumHeight(44);
    setMinimumWidth(minWidth);
    setCursor(Qt::ArrowCursor);
    
    // Store original duration for reference
    originalDuration = getDuration();
}

ClipWidget::~ClipWidget() {}

Mlt::Producer* ClipWidget::getProducer() { return producer; }

void ClipWidget::setSelected(bool sel) {
    selected = sel;
    update();
}

bool ClipWidget::isSelected() const { return selected; }

int ClipWidget::getDuration() const {
    if (!producer) return 0;
    return producer->get_length();
}

int ClipWidget::getStartFrame() const { return startFrame; }

void ClipWidget::setStartFrame(int frame) { 
    startFrame = frame; 
    update();
}

bool ClipWidget::containsFrame(int frame) const {
    int start = getStartFrame();
    int end = start + getDuration();
    return frame >= start && frame <= end;
}

void ClipWidget::paintEvent(QPaintEvent* event) {
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
    
    // Draw background
    QColor bgColor = selected ? QColor(60, 60, 100) : QColor(45, 45, 45);
    p.fillRect(rect(), bgColor);
    
    // Draw selection border
    if (selected) {
        p.setPen(QPen(QColor(100, 150, 255), 2));
        p.drawRect(rect().adjusted(1, 1, -2, -2));
    }
    
    // Draw duration text
    int dur = getDuration();
    if (dur > 0) {
        QString durStr = QString::number(dur) + "f";
        p.setPen(Qt::white);
        p.setFont(QFont("Arial", 9));
        p.drawText(rect().adjusted(48, 0, -8, -2), Qt::AlignRight | Qt::AlignBottom, durStr);
    }
    
    // Draw resize handles
    int handleWidth = 8;
    QColor handleColor = QColor(120, 120, 120);
    
    // Left handle
    QRect leftHandle(0, height()/4, handleWidth, height()/2);
    p.fillRect(leftHandle, handleColor);
    
    // Right handle  
    QRect rightHandle(width() - handleWidth, height()/4, handleWidth, height()/2);
    p.fillRect(rightHandle, handleColor);
    
    // Visual feedback during resize
    if (resizing) {
        p.setPen(QPen(QColor(255, 255, 0), 2, Qt::DashLine));
        p.drawRect(rect().adjusted(2, 2, -2, -2));
    }
    
    // Visual feedback during drag
    if (isDragging) {
        p.setPen(QPen(QColor(0, 255, 0), 2, Qt::DashLine));
        p.drawRect(rect().adjusted(2, 2, -2, -2));
    }
}

void ClipWidget::mousePressEvent(QMouseEvent* event) {
    if (event->button() != Qt::LeftButton) {
        QWidget::mousePressEvent(event);
        return;
    }
    
    int edgeMargin = 10;
    QPoint pos = event->pos();
    
    // Check for resize handles
    if (pos.x() <= edgeMargin) {
        // Left edge resize
        startResize(ResizeLeft, event->globalPos());
        event->accept();
        return;
    } else if (pos.x() >= width() - edgeMargin) {
        // Right edge resize
        startResize(ResizeRight, event->globalPos());
        event->accept();
        return;
    }
    
    // Regular click - start potential drag
    setSelected(true);
    isDragging = false;
    dragStartPos = event->pos();
    dragStartGlobalPos = event->globalPos();
    
    event->accept();
}

void ClipWidget::mouseMoveEvent(QMouseEvent* event) {
    if (resizing) {
        handleResize(event->globalPos());
        return;
    }
    if (!(event->buttons() & Qt::LeftButton)) {
        int edgeMargin = 10;
        if (event->pos().x() <= edgeMargin || event->pos().x() >= width() - edgeMargin) {
            setCursor(Qt::SizeHorCursor);
        } else {
            setCursor(Qt::ArrowCursor);
        }
        return;
    }
    if ((event->pos() - dragStartPos).manhattanLength() < QApplication::startDragDistance()) {
        return;
    }
    // Start QDrag with pointer-based MIME type
    QDrag* drag = new QDrag(this);
    QMimeData* mimeData = new QMimeData;
    QByteArray data;
    QDataStream stream(&data, QIODevice::WriteOnly);
    stream << (qulonglong)this;
    mimeData->setData("application/x-dnd-clip-widget", data);
    drag->setMimeData(mimeData);
    drag->setPixmap(this->grab());
    drag->setHotSpot(event->pos());
    this->hide();
    if (drag->exec(Qt::MoveAction) == Qt::IgnoreAction) {
        this->show();
    }
}

void ClipWidget::mouseReleaseEvent(QMouseEvent* event) {
    if (event->button() != Qt::LeftButton) {
        QWidget::mouseReleaseEvent(event);
        return;
    }
    
    if (resizing) {
        finishResize();
        event->accept();
        return;
    }
    
    if (isDragging) {
        isDragging = false;
        emit dragFinished(this, event->globalPos());
        update();
        event->accept();
        return;
    }
    
    QWidget::mouseReleaseEvent(event);
}

void ClipWidget::startResize(ResizeMode mode, const QPoint& globalPos) {
    resizing = true;
    resizeMode = mode;
    resizeStartPos = globalPos;
    originalGeometry = geometry();
    
    setCursor(Qt::SizeHorCursor);
    update();
    
    emit resizeStarted(this);
}

void ClipWidget::handleResize(const QPoint& globalPos) {
    if (!resizing) return;
    
    int deltaX = globalPos.x() - resizeStartPos.x();
    int frameToPixel = getFrameToPixelScale();
    
    QRect newGeometry = originalGeometry;
    
    if (resizeMode == ResizeLeft) {
        // Resize from left edge
        int newX = qMax(60, originalGeometry.x() + deltaX); // Don't go past track label
        int newWidth = originalGeometry.right() - newX;
        newWidth = qMax(minWidth, newWidth);
        
        // Snap to grid
        int grid = frameToPixel;
        newWidth = ((newWidth + grid/2) / grid) * grid;
        newX = originalGeometry.right() - newWidth;
        
        newGeometry.setLeft(newX);
        newGeometry.setWidth(newWidth);
        
    } else if (resizeMode == ResizeRight) {
        // Resize from right edge
        int newWidth = qMax(minWidth, originalGeometry.width() + deltaX);
        
        // Snap to grid
        int grid = frameToPixel;
        newWidth = ((newWidth + grid/2) / grid) * grid;
        
        newGeometry.setWidth(newWidth);
    }
    
    setGeometry(newGeometry);
    update();
}

void ClipWidget::finishResize() {
    if (!resizing) return;
    
    resizing = false;
    setCursor(Qt::ArrowCursor);
    
    int frameToPixel = getFrameToPixelScale();
    if (frameToPixel <= 0) frameToPixel = 5; // fallback
    
    // Calculate new timing parameters
    int newWidth = width();
    int newDuration = qMax(1, newWidth / frameToPixel);
    int newStartFrame = qMax(0, (x() - kTrackLabelOffset) / frameToPixel);
    
    qDebug() << "Resize finished - Width:" << newWidth << "Duration:" << newDuration << "Start:" << newStartFrame;
    
    update();
    emit resizeFinished(this, newStartFrame, newDuration);
}

int ClipWidget::getFrameToPixelScale() const {
    QWidget* p = parentWidget();
    while (p && !p->inherits("TimelineWidget")) {
        p = p->parentWidget();
    }
    if (p && p->inherits("TimelineWidget")) {
        return p->property("frameToPixelScale").toInt();
    }
    return 5; // fallback
}

void ClipWidget::contextMenuEvent(QContextMenuEvent* event) {
    QMenu menu(this);
    QAction* splitAct = menu.addAction("Split");
    QAction* removeAct = menu.addAction("Remove");
    
    QAction* chosen = menu.exec(event->globalPos());
    if (chosen == splitAct) {
        emit splitRequested(this);
    } else if (chosen == removeAct) {
        emit requestRemove(this);
    }
}

void ClipWidget::enterEvent(QEvent* event) {
    if (!resizing && !isDragging) {
        int edgeMargin = 10;
        QPoint pos = mapFromGlobal(QCursor::pos());
        if (pos.x() <= edgeMargin || pos.x() >= width() - edgeMargin) {
            setCursor(Qt::SizeHorCursor);
        } else {
            setCursor(Qt::ArrowCursor);
        }
    }
    QWidget::enterEvent(event);
}

void ClipWidget::leaveEvent(QEvent* event) {
    if (!resizing && !isDragging) {
        setCursor(Qt::ArrowCursor);
    }
    QWidget::leaveEvent(event);
}

int ClipWidget::getIndexInTrack() const {
    auto* parentTrack = qobject_cast<QWidget*>(parentWidget());
    if (!parentTrack) return -1;
    QList<ClipWidget*> children = parentTrack->findChildren<ClipWidget*>(QString(), Qt::FindDirectChildrenOnly);
    for (int i = 0; i < children.size(); ++i) {
        if (children[i] == this) return i;
    }
    return -1;
}

void ClipWidget::setLabelText(const QString& text) {
    if (label) label->setText(text);
}