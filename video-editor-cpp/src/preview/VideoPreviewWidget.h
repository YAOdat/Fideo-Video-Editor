#pragma once

#include <QLabel>
#include <QList>
#include <QPixmap>

class TextOverlay;

class VideoPreviewWidget : public QLabel {
    Q_OBJECT

public:
    explicit VideoPreviewWidget(QWidget* parent = nullptr);

    void setVideoProperties(const QSize& originalSize);

public slots:
    void setBaseFrame(const QPixmap& pixmap);
    void setTextOverlays(const QList<TextOverlay*>& overlays);
    void setMuted(bool muted);
    void setCompositedImage(const QImage& image);

signals:
    void textOverlayMoved(TextOverlay* overlay, const QPointF& newImagePosition);
    void textOverlaySelected(TextOverlay* overlay); // UX: Signal when an overlay is clicked

protected:
    void paintEvent(QPaintEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;

private:
    // Coordinate conversion helpers
    QPointF widgetToImageCoords(const QPointF& widgetPos) const;
    QPointF imageToWidgetCoords(const QPointF& imagePos) const;
    QRectF getScaledPixmapRect() const;

    TextOverlay* findTextOverlayAt(const QPointF& imagePos);

    QPixmap m_basePixmap;
    QSize m_originalVideoSize;
    QList<TextOverlay*> m_currentTextOverlays;

    // Dragging state
    TextOverlay* m_draggedOverlay = nullptr;
    QPointF m_dragStartPos;      // Position where the drag started (widget coords)
    QPointF m_overlayStartPos;   // Original position of the overlay (image coords)
    bool m_isDragging = false;
    
    bool m_isMuted;
};