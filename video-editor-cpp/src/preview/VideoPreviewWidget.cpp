#include "VideoPreviewWidget.h"
#include "TextOverlay.h"
#include <QPainter>
#include <QMouseEvent>
#include <QPainterPath>
#include <QDebug>
#include <QTimer>
#include <QApplication>

VideoPreviewWidget::VideoPreviewWidget(QWidget* parent)
    : QLabel(parent), m_originalVideoSize(640, 480), m_isMuted(false) {
    setAlignment(Qt::AlignCenter);
    setStyleSheet("font-size: 24px; color: gray; border: 2px dashed #aaa; padding: 40px; background-color: #222;");
    setText("Video Preview");
    setMouseTracking(true);
}

void VideoPreviewWidget::setVideoProperties(const QSize& originalSize) {
    m_originalVideoSize = originalSize;
    update(); // Redraw with correct aspect ratio
}

void VideoPreviewWidget::setBaseFrame(const QPixmap& pixmap) {
    m_basePixmap = pixmap;
    if (m_basePixmap.isNull()) {
        setText("No Video Frame");
    } else {
        setText(""); // Clear placeholder text
    }
    update(); // Trigger a repaint
}

void VideoPreviewWidget::setTextOverlays(const QList<TextOverlay*>& overlays) {
    m_currentTextOverlays = overlays;
    for (TextOverlay* overlay : overlays) {
        if (overlay) {
            qDebug() << "[DBG:setTextOverlays] Overlay text:" << overlay->getText() << "BG color:" << overlay->getBackgroundColor() << "alpha:" << overlay->getBackgroundColor().alpha();
        }
    }
    update(); // Trigger a repaint to show new overlays
}

void VideoPreviewWidget::setMuted(bool muted) {
    m_isMuted = muted;
    // TODO: Add audio muting logic here
}

void VideoPreviewWidget::paintEvent(QPaintEvent* event) {
    QLabel::paintEvent(event); // Draw the label's border and background
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setRenderHint(QPainter::TextAntialiasing);
    painter.setRenderHint(QPainter::SmoothPixmapTransform);

    if (m_basePixmap.isNull()) {
        return; // Nothing to draw
    }

    // 1. Draw the base video frame (scaled to fit)
    QRectF scaledRect = getScaledPixmapRect();
    painter.drawPixmap(scaledRect.toRect(), m_basePixmap);

    // 2. Draw each text overlay on top
    for (TextOverlay* overlay : qAsConst(m_currentTextOverlays)) {
        if (!overlay) continue;

        painter.save();
        
        // --- Setup painter from overlay properties ---
        painter.setFont(overlay->getFont());
        painter.setOpacity(overlay->getOpacity() / 255.0);

        // --- Calculate transformations ---
        QPointF imagePos = overlay->getPosition();
        QPointF widgetPos = imageToWidgetCoords(imagePos);
        painter.translate(widgetPos);
        painter.rotate(overlay->getRotationAngle());
        // --- Calculate text and background rectangles ---
        QFontMetricsF metrics(painter.font());
        QRectF textBoundingBox = metrics.boundingRect(overlay->getText());
        // --- Draw background (draw first) ---
        QColor bg = overlay->getBackgroundColor();
        if (bg != Qt::transparent && bg.alpha() > 0) {
            QRectF bgRect = textBoundingBox.adjusted(-8, -8, 8, 8);
            bgRect.moveCenter(QPointF(0, 0));
            painter.save();
            painter.setBrush(bg);
            painter.setPen(Qt::NoPen);
            painter.drawRect(bgRect);
            painter.restore();
        }
        // --- Prepare Text Path, Centered at the Origin ---
        QPainterPath path;
        QPointF textStartPoint = -textBoundingBox.center();
        path.addText(textStartPoint, painter.font(), overlay->getText());

        // Draw shadow (if enabled)
        if (overlay->isShadowEnabled() && overlay->getShadowColor().alpha() > 0) {
            painter.save();
            painter.translate(4, 4); // Simple shadow offset
            QColor shadowColor = overlay->getShadowColor();
            painter.fillPath(path, shadowColor);
            painter.restore();
        }

        // Draw outline (if width > 0)
        if (overlay->getOutlineWidth() > 0) {
            QPen pen(overlay->getOutlineColor());
            pen.setWidth(overlay->getOutlineWidth());
            painter.strokePath(path, pen);
        }

        // Draw main text
        painter.fillPath(path, overlay->getTextColor());

        painter.restore();
    }
}

void VideoPreviewWidget::mousePressEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {
        QPointF imagePos = widgetToImageCoords(event->pos());
        TextOverlay* clickedOverlay = findTextOverlayAt(imagePos);

        if (clickedOverlay) {
            // Prevent reopening properties if already open for this overlay
            static TextOverlay* lastOpenedOverlay = nullptr;
            static QWidget* lastPanel = nullptr;
            bool panelOpen = (lastPanel && lastPanel->isVisible());
            if (panelOpen && lastOpenedOverlay == clickedOverlay) {
                // Start drag logic only
                m_draggedOverlay = clickedOverlay;
                m_isDragging = true;
                m_dragStartPos = event->pos();
                m_overlayStartPos = clickedOverlay->getPosition();
                setCursor(Qt::ClosedHandCursor);
                event->accept();
                return;
            }
            // Otherwise, open properties and remember
            emit textOverlaySelected(clickedOverlay);
            lastOpenedOverlay = clickedOverlay;
            // Find the panel after it is created (delayed)
            QTimer::singleShot(200, [=]() {
                QWidgetList topLevels = QApplication::topLevelWidgets();
                for (QWidget* w : topLevels) {
                    if (w->windowTitle() == "Text Properties" && w->isVisible()) {
                        lastPanel = w;
                        break;
                    }
                }
            });
            event->accept();
            return;
        }
    }
    QLabel::mousePressEvent(event);
}

void VideoPreviewWidget::mouseMoveEvent(QMouseEvent* event) {
    if (m_isDragging && m_draggedOverlay) {
        QPointF dragVector = event->pos() - m_dragStartPos;

        // Scale the drag vector to match the image coordinate system
        QRectF scaledRect = getScaledPixmapRect();
        if (scaledRect.width() == 0 || scaledRect.height() == 0) return;
        
        double scaleX = m_originalVideoSize.width() / scaledRect.width();
        double scaleY = m_originalVideoSize.height() / scaledRect.height();
        
        QPointF imageOffset(dragVector.x() * scaleX, dragVector.y() * scaleY);
        QPointF newImagePos = m_overlayStartPos + imageOffset;

        m_draggedOverlay->setPosition(newImagePos);
        
        // The paintEvent will handle the visual update, so we just call update()
        update();
        
        emit textOverlayMoved(m_draggedOverlay, newImagePos);
        event->accept();
        return;
    }
    QLabel::mouseMoveEvent(event);
}

void VideoPreviewWidget::mouseReleaseEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton && m_isDragging) {
        m_isDragging = false;
        m_draggedOverlay = nullptr;
        setCursor(Qt::ArrowCursor);
        event->accept();
        return;
    }
    QLabel::mouseReleaseEvent(event);
}

TextOverlay* VideoPreviewWidget::findTextOverlayAt(const QPointF& imagePos) {
    // Iterate backwards to select the top-most overlay first
    for (int i = m_currentTextOverlays.size() - 1; i >= 0; --i) {
        TextOverlay* overlay = m_currentTextOverlays[i];
        if (!overlay) continue;

        // Note: For perfect hit-testing, the bounding rect should be calculated
        // based on the current text, font, and rotation.
        QRectF boundingRect = overlay->getBoundingRect();
        boundingRect.moveCenter(overlay->getPosition()); // Center the rect on the overlay's position

        if (boundingRect.contains(imagePos)) {
            return overlay;
        }
    }
    return nullptr;
}

QRectF VideoPreviewWidget::getScaledPixmapRect() const {
    if (m_basePixmap.isNull()) return QRectF();

    QSize p_size = m_basePixmap.size();
    p_size.scale(size(), Qt::KeepAspectRatio);

    // Calculate top-left corner to center the pixmap
    QPointF offset = QPointF(width() - p_size.width(), height() - p_size.height()) / 2.0;
    
    return QRectF(offset, p_size);
}

QPointF VideoPreviewWidget::widgetToImageCoords(const QPointF& widgetPos) const {
    QRectF scaledRect = getScaledPixmapRect();
    if (m_originalVideoSize.isNull() || scaledRect.width() == 0) {
        return widgetPos; // Cannot convert
    }
    
    // Position relative to the scaled pixmap's top-left corner
    QPointF relativePos = widgetPos - scaledRect.topLeft();

    // Scale up to original video coordinates
    double scaleX = m_originalVideoSize.width() / scaledRect.width();
    double scaleY = m_originalVideoSize.height() / scaledRect.height();

    return QPointF(relativePos.x() * scaleX, relativePos.y() * scaleY);
}

QPointF VideoPreviewWidget::imageToWidgetCoords(const QPointF& imagePos) const {
    QRectF scaledRect = getScaledPixmapRect();
    if (m_originalVideoSize.isNull() || scaledRect.width() == 0) {
        return imagePos; // Cannot convert
    }

    // Scale down to widget coordinates
    double scaleX = scaledRect.width() / m_originalVideoSize.width();
    double scaleY = scaledRect.height() / m_originalVideoSize.height();

    QPointF scaledPos(imagePos.x() * scaleX, imagePos.y() * scaleY);

    // Add the widget's top-left offset
    return scaledPos + scaledRect.topLeft();
}

void VideoPreviewWidget::resizeEvent(QResizeEvent *event) {
    QLabel::resizeEvent(event);
    update(); // Redraw everything correctly on resize
}

void VideoPreviewWidget::setCompositedImage(const QImage& image) {
    setBaseFrame(QPixmap::fromImage(image));
}