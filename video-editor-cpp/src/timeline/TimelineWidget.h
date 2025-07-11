#pragma once
#include <QWidget>
#include <QVBoxLayout>
#include <QList>
#include <QPropertyAnimation>
#include <Mlt.h>
#include "TrackWidget.h"
#include "TimelineConstants.h"

class TimelineWidget : public QWidget {
    Q_OBJECT
    Q_PROPERTY(int playhead READ getPlayhead WRITE setPlayhead NOTIFY playheadChanged)
    
public:
    TimelineWidget(QWidget* parent = nullptr);
    ~TimelineWidget();
    
    void addTrack(int trackNumber = -1);
    void removeTrack(int index);
    void addClip(int trackIndex, const QString& filePath);
    void removeClip(int trackIndex, int clipIndex);
    
    void setPlayhead(int frame);
    int getPlayhead() const;
    int getTrackCount() const;
    int getClipCount(int trackIndex) const;
    
    Mlt::Multitrack* getMultitrack();
    Mlt::Tractor* getTractor();
    
    bool isPlaying() const { return m_isPlaying; }
    int getFrameToPixel() const { return frameToPixelScale; }
    void setFrameToPixel(int v);

public slots:
    void play();
    void pause();
    void stop();
    void requestFrame();
    QImage getCurrentFrame();

signals:
    void playheadChanged(int frame);
    void frameReady(const QImage& image);
    void clipDoubleClicked(int trackIndex, int clipIndex);

protected:
    void paintEvent(QPaintEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;
    void wheelEvent(QWheelEvent* event) override;

private:
    // UI Components
    QVBoxLayout* mainLayout;
    QList<TrackWidget*> trackWidgets;
    
    // MLT Components
    Mlt::Profile* profile;
    Mlt::Multitrack* multitrack;
    Mlt::Tractor* tractor;
    
    // Playback State
    int playhead = 0;
    QTimer* playbackTimer = nullptr;
    bool m_isPlaying = false;
    
    // Interaction State
    bool isScrubbing = false;
    int scrubStartX = 0;
    int scrubStartFrame = 0;
    
    // Animation
    QPropertyAnimation* playheadAnimation = nullptr;
    
    // Scaling
    int frameToPixelScale = kDefaultFrameToPixel;
    
    // Private Methods
    void refreshUI();
    void updateFrameAtCurrentPosition();
    QString formatTimecode(int frame) const;
    
    // Coordinate conversion
    int frameToPixel(int frame) const;
    int pixelToFrame(int x) const;
    
    // Timeline analysis
    QList<int> getAllClipEdgeFrames() const;
    int getTimelineDuration() const;
    int getFirstClipStartFrame() const;
    
    // MLT management
    void rebuildMultitrackFromTracks();
};