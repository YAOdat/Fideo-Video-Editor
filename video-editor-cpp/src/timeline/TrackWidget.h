#pragma once
#include <QWidget>
#include <QHBoxLayout>
#include <QList>
#include <QPointer>
#include <Mlt.h>
#include <QMenu>

class ClipWidget;

class TrackWidget : public QWidget {
    Q_OBJECT
public:
    TrackWidget(Mlt::Playlist* playlist, int trackNumber, QWidget* parent = nullptr);
    ~TrackWidget();
    void addClip(const QString& filePath, int insertIndex = -1);
    void removeClip(int index);
    int getClipCount() const;
    Mlt::Playlist* getPlaylist();
    QList<int> getClipEdgeFrames() const;
    void updateClipStartFrames();
    void updateClipWidths(int frameToPixel);
    bool splitClipAt(int frame);
    int getFrameToPixelScale() const;
    bool hasClipAtFrame(int frame) const;
    void refreshUI();
protected:
    void paintEvent(QPaintEvent* event) override;
    void dragEnterEvent(QDragEnterEvent* event) override;
    void dragMoveEvent(QDragMoveEvent* event) override;
    void dragLeaveEvent(QDragLeaveEvent* event) override;
    void dropEvent(QDropEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
private:
    // --- Enhanced features ---
    bool snapToGrid = true;
    int snapTolerance = 10; // pixels
    QList<int> snapPoints;
    bool magneticSnap = true;
    QColor snapLineColor = QColor(255, 255, 0, 180);
    QColor trackSeparatorColor = QColor(60, 60, 60);
    bool isLocked = false;
    bool isMuted = false;
    bool isSolo = false;
    bool rippleEditEnabled = false;
    QList<ClipWidget*> selectedClips;
    bool isMultiSelecting = false;
    QPoint selectionStart;
    QRect selectionRect;
    // --- End enhanced features ---
    // No layout; manual positioning
    QList<QPointer<ClipWidget>> clipWidgets;
    Mlt::Playlist* playlist;
    int trackNumber;
    int dropIndicatorX = -1;
    void rebuildPlaylistFromWidgets();
    void handleClipResize(ClipWidget* clip, int newStart, int newDuration);
    int calculateInsertIndex(const QPoint& pos) const;
    // Enhanced helpers
    void createTrackHeader();
    void createTrackControls();
    void updateSnapPoints();
    int snapToNearestPoint(int x) const;
    void clearSelection();
    void selectClipRange(ClipWidget* endClip);
    void showContextMenu(const QPoint& pos, ClipWidget* clip);
public slots:
    void toggleLock();
    void toggleMute();
    void toggleSolo();
    void enableRippleEdit(bool enabled);
    void setSnapEnabled(bool enabled);
signals:
    void trackStateChanged(int trackNumber, bool locked, bool muted, bool solo);
    void clipSelectionChanged(const QList<ClipWidget*>& selected);
    void timelineChanged();
    void clipResized();
    void clipDoubleClicked(int clipIndex);
}; 