#include "TimelineWidget.h"
#include "TrackWidget.h"
#include <QPushButton>
#include <QVBoxLayout>
#include <QLabel>
#include <mlt/framework/mlt_multitrack.h>
#include <QTimer>
#include <QInputDialog>
#include <QDebug>
#include <QMouseEvent>
#include <QPaintEvent>
#include <QPainter>
#include <QPen>
#include <QColor>
#include <QToolTip>
#include <QFontMetrics>
#include <algorithm>
#include <QKeyEvent>
#include <QGraphicsDropShadowEffect>
#include "TimelineConstants.h"
#include <limits>

TimelineWidget::TimelineWidget(QWidget* parent)
    : QWidget(parent), profile(nullptr), multitrack(nullptr), tractor(nullptr) {
    setFocusPolicy(Qt::StrongFocus);
    setProperty("frameToPixelScale", frameToPixelScale);
    try {
        // Initialize MLT profile with a profile string
        profile = new Mlt::Profile("width=1920:height=1080:progressive=1:sample_aspect_num=1:sample_aspect_den=1:frame_rate_num=25:frame_rate_den=1:display_aspect_num=16:display_aspect_den=9:colorspace=709:format=yuv420p");
        qDebug() << "MLT Profile valid?" << (profile && profile->is_valid());
        if (!profile || !profile->is_valid()) {
            qDebug() << "Failed to create MLT profile";
            return;
        }
        
        // Initialize multitrack
        mlt_multitrack c_multitrack = mlt_multitrack_init();
        if (!c_multitrack) {
            qDebug() << "Failed to initialize MLT multitrack";
            return;
        }
        
        multitrack = new Mlt::Multitrack(c_multitrack);
        qDebug() << "MLT Multitrack valid?" << (multitrack && multitrack->is_valid());
        if (!multitrack || !multitrack->is_valid()) {
            qDebug() << "Failed to create MLT multitrack";
            return;
        }
        
        // Initialize tractor
        tractor = new Mlt::Tractor(*profile);
        qDebug() << "MLT Tractor valid?" << (tractor && tractor->is_valid());
        if (!tractor || !tractor->is_valid()) {
            qDebug() << "Failed to create MLT tractor";
            return;
        }
        
        tractor->insert_track(*multitrack, 0);
        
        // Initialize UI
        mainLayout = new QVBoxLayout(this);
        setLayout(mainLayout);
        playbackTimer = new QTimer(this);
        playbackTimer->setTimerType(Qt::PreciseTimer); // More precise timing
        connect(playbackTimer, &QTimer::timeout, this, &TimelineWidget::requestFrame);
        refreshUI();
        setProperty("playhead", playhead);
        
        // Initialize playhead animation
        playheadAnimation = new QPropertyAnimation(this, "playhead", this);
        playheadAnimation->setDuration(100);
        playheadAnimation->setEasingCurve(QEasingCurve::OutQuad);
        
    } catch (const std::exception& e) {
        qDebug() << "Exception during MLT initialization:" << e.what();
    }
}

TimelineWidget::~TimelineWidget() {
    if (playheadAnimation) {
        playheadAnimation->stop();
        delete playheadAnimation;
    }
    if (tractor) delete tractor;
    if (multitrack) delete multitrack;
    if (profile) delete profile;
}

void TimelineWidget::addTrack(int trackNumber) {
    if (!profile || !profile->is_valid() || !multitrack || !multitrack->is_valid()) {
        qDebug() << "Cannot add track: MLT not properly initialized. Profile valid?" << (profile && profile->is_valid());
        return;
    }
    try {
        auto* playlist = new Mlt::Playlist(*profile);
        qDebug() << "MLT Playlist valid?" << (playlist && playlist->is_valid());
        if (!playlist || !playlist->is_valid()) {
            qDebug() << "Failed to create playlist";
            delete playlist;
            return;
        }
        multitrack->insert(*playlist, multitrack->count());
        int n = (trackNumber == -1) ? trackWidgets.size() : trackNumber;
        auto* trackWidget = new TrackWidget(playlist, n, this);
        trackWidgets.append(trackWidget);
        // Connect signals for sync
        connect(trackWidget, &TrackWidget::timelineChanged, this, [this]{
            rebuildMultitrackFromTracks();
            updateFrameAtCurrentPosition();
        });
        connect(trackWidget, &TrackWidget::trackStateChanged, this, [this](int, bool, bool, bool){
            rebuildMultitrackFromTracks();
            updateFrameAtCurrentPosition();
        });
        connect(trackWidget, &QWidget::windowTitleChanged, this, [this]{
            rebuildMultitrackFromTracks();
            updateFrameAtCurrentPosition();
        });
        connect(trackWidget, &TrackWidget::clipResized, this, [this]{
            rebuildMultitrackFromTracks();
            updateFrameAtCurrentPosition();
        });
        connect(trackWidget, &TrackWidget::clipDoubleClicked, this, [this, n](int clipIndex) {
            emit clipDoubleClicked(n, clipIndex);
        });
        refreshUI();
    } catch (const std::exception& e) {
        qDebug() << "Exception adding track:" << e.what();
    }
}

void TimelineWidget::removeTrack(int index) {
    if (index < 0 || index >= trackWidgets.size()) return;
    if (!multitrack || !multitrack->is_valid()) return;
    
    try {
        multitrack->disconnect(index);
        delete trackWidgets.takeAt(index);
        refreshUI();
    } catch (const std::exception& e) {
        qDebug() << "Exception removing track:" << e.what();
    }
}

void TimelineWidget::addClip(int trackIndex, const QString& filePath) {
    if (trackWidgets.isEmpty()) {
        qDebug() << "No tracks available to add a clip.";
        return;
    }
    if (trackIndex < 0 || trackIndex >= trackWidgets.size()) return;
    trackWidgets[trackIndex]->addClip(filePath);
    refreshUI();
    rebuildMultitrackFromTracks();
    setPlayhead(getFirstClipStartFrame());
    updateFrameAtCurrentPosition();
}

void TimelineWidget::removeClip(int trackIndex, int clipIndex) {
    if (trackIndex < 0 || trackIndex >= trackWidgets.size()) return;
    trackWidgets[trackIndex]->removeClip(clipIndex);
    refreshUI();
    rebuildMultitrackFromTracks();
    updateFrameAtCurrentPosition();
}

void TimelineWidget::setPlayhead(int frame) {
    int clampedFrame = std::clamp(frame, 0, getTimelineDuration());
    
    // Only update if the frame actually changed
    if (playhead == clampedFrame) {
        return;
    }
    
    playhead = clampedFrame;
    // setProperty("playhead", playhead); // REMOVE THIS LINE TO PREVENT INFINITE RECURSION
    
    // CRITICAL: Update tractor position for sync
    if (tractor && tractor->is_valid()) {
        tractor->seek(playhead);
    }
    
    // Update UI immediately
    update();
    
    // Emit signals
    emit playheadChanged(playhead);
    
    // Force frame update unless we're in the middle of playback
    if (!m_isPlaying || isScrubbing) {
        updateFrameAtCurrentPosition();
    }
}

// New method to ensure frame updates are always in sync
void TimelineWidget::updateFrameAtCurrentPosition() {
    if (tractor && tractor->is_valid()) {
        qDebug() << "[Preview] updateFrameAtCurrentPosition: playhead=" << playhead << ", tractor position=" << tractor->position();
        tractor->seek(playhead);
        QImage frame = getCurrentFrame();
        qDebug() << "[Preview] Emitting frameReady. Frame valid?" << !frame.isNull();
        emit frameReady(frame);
    }
}

int TimelineWidget::getPlayhead() const { return playhead; }
int TimelineWidget::getTrackCount() const { return trackWidgets.size(); }
int TimelineWidget::getClipCount(int trackIndex) const {
    if (trackIndex < 0 || trackIndex >= trackWidgets.size()) return 0;
    return trackWidgets[trackIndex]->getClipCount();
}
Mlt::Multitrack* TimelineWidget::getMultitrack() { return multitrack; }
Mlt::Tractor* TimelineWidget::getTractor() { return tractor; }

void TimelineWidget::refreshUI() {
    // Remove all widgets from the layout, but do not delete them
    QLayoutItem* item;
    while ((item = mainLayout->takeAt(0)) != nullptr) {
        if (item->widget()) {
            item->widget()->setParent(nullptr); // Detach from layout, do not delete
        }
        delete item;
    }
    // Add track widgets
    for (int i = 0; i < trackWidgets.size(); ++i) {
        mainLayout->addWidget(trackWidgets[i]);
    }
}

void TimelineWidget::play() {
    if (m_isPlaying) return;
    
    m_isPlaying = true;
    
    // Start from current position, not zero
    if (tractor && tractor->is_valid()) {
        tractor->seek(playhead);
    }
    
    updateFrameAtCurrentPosition();
    playbackTimer->start(40); // ~25fps
    
    qDebug() << "Started playback at frame:" << playhead;
}

void TimelineWidget::pause() {
    if (!m_isPlaying) return;
    
    m_isPlaying = false;
    playbackTimer->stop();
    
    // Ensure final frame is displayed
    updateFrameAtCurrentPosition();
    
    qDebug() << "Paused playback at frame:" << playhead;
}

void TimelineWidget::stop() {
    bool wasPlaying = m_isPlaying;
    m_isPlaying = false;
    playbackTimer->stop();
    
    // Smooth animation to beginning
    if (wasPlaying && playheadAnimation) {
        playheadAnimation->stop();
        playheadAnimation->setStartValue(playhead);
        playheadAnimation->setEndValue(0);
        playheadAnimation->start();
    } else {
        setPlayhead(0);
    }
    
    qDebug() << "Stopped playback";
}

void TimelineWidget::requestFrame() {
    if (!m_isPlaying || !tractor || !tractor->is_valid()) {
        return;
    }
    
    // Check if we have content at current position
    bool hasVisibleClip = false;
    for (TrackWidget* track : trackWidgets) {
        if (track && track->hasClipAtFrame(playhead)) {
            hasVisibleClip = true;
            break;
        }
    }
    
    if (!hasVisibleClip) {
        QImage blank(640, 360, QImage::Format_RGB32);
        blank.fill(Qt::darkGray);
        emit frameReady(blank);
    } else {
        // Ensure tractor is at correct position before getting frame
        tractor->seek(playhead);
        emit frameReady(getCurrentFrame());
    }
    
    // Advance playhead
    int nextFrame = playhead + 1;
    int maxFrame = getTimelineDuration();
    
    if (nextFrame >= maxFrame) {
        // End of timeline reached
        pause();
        return;
    }
    
    setPlayhead(nextFrame);
}

QImage TimelineWidget::getCurrentFrame() {
    if (!tractor || !tractor->is_valid()) {
        return QImage();
    }
    try {
        if (tractor->position() != playhead) {
            tractor->seek(playhead);
        }
        qDebug() << "[Preview] getCurrentFrame: playhead=" << playhead << ", tractor position=" << tractor->position();
        Mlt::Frame* frame = tractor->get_frame();
        if (!frame || !frame->is_valid()) {
            qDebug() << "[Preview] Invalid frame at playhead=" << playhead;
            delete frame;
            return QImage();
        }
        int w = 0, h = 0;
        mlt_image_format format = mlt_image_rgb24;
        uint8_t* image_data = frame->get_image(format, w, h, 0);
        QImage img;
        if (w > 0 && h > 0 && image_data) {
            img = QImage(image_data, w, h, QImage::Format_RGB888).convertToFormat(QImage::Format_RGB32).copy();
        }
        delete frame;
        qDebug() << "[Preview] Frame extracted: valid=" << !img.isNull() << ", size=" << w << "x" << h;
        return img;
    } catch (...) {
        return QImage();
    }
} 

void TimelineWidget::paintEvent(QPaintEvent* event) {
    QWidget::paintEvent(event);
    
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);
    
    // Enhanced playhead visualization
    int rulerHeight = 30;
    int playheadX = frameToPixel(playhead);
    
    // Draw playhead line with gradient
    QLinearGradient gradient(0, rulerHeight, 0, height());
    gradient.setColorAt(0, QColor(255, 100, 100, 255));
    gradient.setColorAt(1, QColor(255, 100, 100, 180));
    
    p.setPen(QPen(QBrush(gradient), 3));
    p.drawLine(playheadX, rulerHeight, playheadX, height());
    
    // Draw playhead handle (triangle at top)
    QPolygon triangle;
    triangle << QPoint(playheadX - 8, rulerHeight - 5)
             << QPoint(playheadX + 8, rulerHeight - 5)
             << QPoint(playheadX, rulerHeight + 10);
    
    p.setPen(QPen(QColor(200, 60, 60), 2));
    p.setBrush(QColor(255, 100, 100));
    p.drawPolygon(triangle);
    
    // Enhanced timecode display
    QString timecode = formatTimecode(playhead);
    QFont font = p.font();
    font.setBold(true);
    font.setPointSize(10);
    p.setFont(font);
    
    QFontMetrics fm(font);
    int textWidth = fm.horizontalAdvance(timecode) + 12;
    int textHeight = fm.height() + 6;
    
    // Position timecode box above playhead
    QRect labelRect(playheadX - textWidth/2, rulerHeight - textHeight - 8, textWidth, textHeight);
    
    // Ensure timecode stays within widget bounds
    if (labelRect.left() < 0) {
        labelRect.moveLeft(5);
    } else if (labelRect.right() > width()) {
        labelRect.moveRight(width() - 5);
    }
    
    // Draw timecode background with rounded corners
    p.setPen(Qt::NoPen);
    p.setBrush(QColor(40, 40, 40, 240));
    p.drawRoundedRect(labelRect, 4, 4);
    
    // Draw timecode text
    p.setPen(QColor(255, 255, 255));
    p.drawText(labelRect, Qt::AlignCenter, timecode);
    
    // Draw frame indicator for precise positioning
    if (isScrubbing) {
        p.setPen(QPen(QColor(255, 200, 0), 1));
        p.drawText(labelRect.adjusted(0, textHeight + 2, 0, textHeight + 2), 
                   Qt::AlignCenter, QString("Frame: %1").arg(playhead));
    }
}

QString TimelineWidget::formatTimecode(int frame) const {
    if (!profile || !profile->is_valid()) {
        return QString("%1").arg(frame);
    }
    
    double fps = profile->fps();
    int totalSeconds = frame / fps;
    int minutes = totalSeconds / 60;
    int seconds = totalSeconds % 60;
    int frameNumber = frame % (int)fps;
    
    return QString("%1:%2:%3")
        .arg(minutes, 2, 10, QChar('0'))
        .arg(seconds, 2, 10, QChar('0'))
        .arg(frameNumber, 2, 10, QChar('0'));
}

// Helper: snap frame to nearest clip edge if within threshold
static int snapToEdges(int frame, const QList<int>& edges, int threshold = 3) {
    for (int edge : edges) {
        if (std::abs(frame - edge) <= threshold) return edge;
    }
    return frame;
}

void TimelineWidget::setFrameToPixel(int v) {
    frameToPixelScale = std::clamp(v, kMinFrameToPixel, kMaxFrameToPixel);
    setProperty("frameToPixelScale", frameToPixelScale);
    // Update all track UIs
    for (auto* track : trackWidgets) {
        if (track) track->refreshUI();
    }
    update();
}

void TimelineWidget::wheelEvent(QWheelEvent* event) {
    if (event->modifiers() & Qt::ControlModifier) {
        int delta = event->angleDelta().y() > 0 ? 1 : -1;
        setFrameToPixel(frameToPixelScale + delta);
        event->accept();
    } else {
        QWidget::wheelEvent(event);
    }
}

int TimelineWidget::frameToPixel(int frame) const {
    return frame * frameToPixelScale + kTrackLabelOffset;
}

int TimelineWidget::pixelToFrame(int x) const {
    return (x - kTrackLabelOffset) / frameToPixelScale;
}

void TimelineWidget::mousePressEvent(QMouseEvent* event) {
    setFocus();
    if (event->button() == Qt::LeftButton) {
        // If click is in ruler area (top 30px), move playhead instantly
        if (event->y() < 30) {
            int newFrame = pixelToFrame(event->x());
            // Snap and clamp
            QList<int> edges = getAllClipEdgeFrames();
            newFrame = snapToEdges(newFrame, edges);
            setPlayhead(newFrame);
            return;
        }
        
        // Start scrubbing
        isScrubbing = true;
        scrubStartX = event->x();
        scrubStartFrame = playhead;
        
        int newFrame = pixelToFrame(event->x());
        QList<int> edges = getAllClipEdgeFrames();
        newFrame = snapToEdges(newFrame, edges);
        setPlayhead(newFrame);
    }
    QWidget::mousePressEvent(event);
}

void TimelineWidget::mouseMoveEvent(QMouseEvent* event) {
    if (isScrubbing && (event->buttons() & Qt::LeftButton)) {
        int newFrame = pixelToFrame(event->x());
        QList<int> edges = getAllClipEdgeFrames();
        newFrame = snapToEdges(newFrame, edges);
        setPlayhead(newFrame);
    }
    QWidget::mouseMoveEvent(event);
}

void TimelineWidget::mouseReleaseEvent(QMouseEvent* event) {
    if (isScrubbing && event->button() == Qt::LeftButton) {
        isScrubbing = false;
        int newFrame = pixelToFrame(event->x());
        QList<int> edges = getAllClipEdgeFrames();
        newFrame = snapToEdges(newFrame, edges);
        setPlayhead(newFrame);
        
        // Final frame update to ensure sync
        updateFrameAtCurrentPosition();
    }
    QWidget::mouseReleaseEvent(event);
} 

QList<int> TimelineWidget::getAllClipEdgeFrames() const {
    QList<int> edges;
    for (const auto* track : trackWidgets) {
        if (track) edges.append(track->getClipEdgeFrames());
    }
    std::sort(edges.begin(), edges.end());
    edges.erase(std::unique(edges.begin(), edges.end()), edges.end());
    return edges;
}

int TimelineWidget::getTimelineDuration() const {
    int maxEnd = 0;
    for (const auto* track : trackWidgets) {
        if (!track) continue;
        QList<int> edges = track->getClipEdgeFrames();
        for (int i = 1; i < edges.size(); i += 2) { // end frames
            if (edges[i] > maxEnd) maxEnd = edges[i];
        }
    }
    return maxEnd;
} 

int TimelineWidget::getFirstClipStartFrame() const {
    int minStart = std::numeric_limits<int>::max();
    for (const auto* track : trackWidgets) {
        if (!track) continue;
        QList<int> edges = track->getClipEdgeFrames();
        if (edges.size() >= 2) {
            if (edges[0] < minStart) minStart = edges[0];
        }
    }
    return (minStart == std::numeric_limits<int>::max()) ? 0 : minStart;
}

void TimelineWidget::keyPressEvent(QKeyEvent* event) {
    if (event->key() == Qt::Key_Space) {
        // Toggle play/pause
        if (m_isPlaying) {
            pause();
        } else {
            play();
        }
        event->accept();
    } else if (event->key() == Qt::Key_S) {
        int frame = playhead;
        for (auto* track : trackWidgets) {
            if (track && track->splitClipAt(frame)) {
                updateFrameAtCurrentPosition();
                break; // Only split the first matching clip
            }
        }
        event->accept();
    } else if (event->key() == Qt::Key_Left) {
        // Frame backward
        int step = (event->modifiers() & Qt::ShiftModifier) ? 10 : 1;
        setPlayhead(playhead - step);
        event->accept();
    } else if (event->key() == Qt::Key_Right) {
        // Frame forward
        int step = (event->modifiers() & Qt::ShiftModifier) ? 10 : 1;
        setPlayhead(playhead + step);
        event->accept();
    } else if (event->key() == Qt::Key_Home) {
        setPlayhead(getFirstClipStartFrame());
        event->accept();
    } else if (event->key() == Qt::Key_End) {
        // Go to end
        setPlayhead(getTimelineDuration());
        event->accept();
    } else {
        QWidget::keyPressEvent(event);
    }
} 

void TimelineWidget::rebuildMultitrackFromTracks() {
    if (!multitrack || !multitrack->is_valid()) return;
    
    // Store current playhead position
    int currentFrame = playhead;
    
    // Remove all tracks from multitrack
    while (multitrack->count() > 0) {
        multitrack->disconnect(0);
    }
    
    // Add each track's playlist in order
    for (int i = 0; i < trackWidgets.size(); ++i) {
        TrackWidget* track = trackWidgets[i];
        if (track && track->getPlaylist()) {
            multitrack->insert(*track->getPlaylist(), i);
        }
    }
    
    // Rebuild tractor
    if (tractor) delete tractor;
    tractor = new Mlt::Tractor(*profile);
    tractor->insert_track(*multitrack, 0);
    
    // Restore playhead position and sync
    tractor->seek(currentFrame);
    qDebug() << "Rebuilt multitrack, restored playhead to frame:" << currentFrame;
}