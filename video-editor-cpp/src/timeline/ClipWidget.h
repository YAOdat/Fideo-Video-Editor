#pragma once
#include <QWidget>
#include <QLabel>
#include <QPixmap>
#include <QPoint>
#include <QRect>
#include <Mlt.h>

class ClipWidget : public QWidget {
    Q_OBJECT
    
public:
    enum ResizeMode {
        ResizeNone,
        ResizeLeft,
        ResizeRight
    };
    
    ClipWidget(Mlt::Producer* producer, const QPixmap& thumb, QWidget* parent = nullptr);
    ~ClipWidget();
    
    Mlt::Producer* getProducer();
    void setSelected(bool selected);
    bool isSelected() const;
    int getDuration() const;
    int getStartFrame() const;
    void setStartFrame(int frame);
    bool containsFrame(int frame) const;
    void setLabelText(const QString& text);
    
    bool isResizing() const { return resizing; }
    bool isDraggingClip() const { return isDragging; }
    
    // Optionally, add a helper to get the index in parent track
    int getIndexInTrack() const;
    
protected:
    void paintEvent(QPaintEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void contextMenuEvent(QContextMenuEvent* event) override;
    void enterEvent(QEvent* event) override;
    void leaveEvent(QEvent* event) override;

private:
    // UI Components
    QLabel* thumbLabel;
    QLabel* label;
    QPixmap thumbnail;
    
    // MLT Producer
    Mlt::Producer* producer;
    
    // State
    bool selected = false;
    bool resizing = false;
    bool isDragging = false;
    
    // Resize state
    ResizeMode resizeMode = ResizeNone;
    QPoint resizeStartPos;
    QRect originalGeometry;
    
    // Drag state
    QPoint dragStartPos;
    QPoint dragStartGlobalPos;
    
    // Properties
    int minWidth = 60;
    int startFrame = 0;
    int originalDuration = 0;
    
    // Helper methods
    void startResize(ResizeMode mode, const QPoint& globalPos);
    void handleResize(const QPoint& globalPos);
    void finishResize();
    int getFrameToPixelScale() const;
    
signals:
    void requestRemove(ClipWidget* self);
    void splitRequested(ClipWidget* self);
    void resizeStarted(ClipWidget* self);
    void resizeFinished(ClipWidget* self, int newStartFrame, int newDurationFrames);
    void dragStarted(ClipWidget* self);
    void dragMoved(ClipWidget* self, const QPoint& globalPos);
    void dragFinished(ClipWidget* self, const QPoint& globalPos);
};