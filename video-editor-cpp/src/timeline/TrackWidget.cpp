#include "TrackWidget.h"
#include "ClipWidget.h"
#include <QPushButton>
#include <QHBoxLayout>
#include <QLabel>
#include <QFileDialog>
#include <QDebug>
#include <QPainter>
#include <QMimeData>
#include <QDropEvent>
#include <QDragEnterEvent>
#include <QFileInfo>
#include "../utils/FileUtils.h"
#include <QKeyEvent>
#include <QMouseEvent>
#include <QPointer>
#include "TimelineConstants.h"
#include <QEvent>

// Utility function to check if a file is an audio file
static bool isAudioFile(const QString& filePath) {
    QString ext = QFileInfo(filePath).suffix().toLower();
    return ext == "mp3" || ext == "wav" || ext == "ogg" || ext == "flac" || ext == "aac" || ext == "m4a";
}

static QPixmap audioIcon() {
    QPixmap pix(40, 40);
    pix.fill(Qt::transparent);
    QPainter p(&pix);
    p.setRenderHint(QPainter::Antialiasing);
    p.setPen(Qt::NoPen);
    p.setBrush(QColor(80, 80, 180));
    p.drawEllipse(5, 5, 30, 30);
    p.setPen(QPen(Qt::white, 3));
    p.drawLine(20, 12, 20, 28);
    p.drawLine(20, 12, 28, 20);
    p.drawLine(20, 28, 28, 20);
    return pix;
}

TrackWidget::TrackWidget(Mlt::Playlist* playlist, int trackNumber, QWidget* parent)
    : QWidget(parent), playlist(playlist), trackNumber(trackNumber) {
    setAcceptDrops(true);
    qDebug() << "TrackWidget created with playlist pointer:" << playlist << "valid?" << (playlist && playlist->is_valid());
    if (!playlist || !playlist->is_valid()) {
        qDebug() << "Invalid playlist provided to TrackWidget";
        return;
    }
    refreshUI();
}

TrackWidget::~TrackWidget() {
    qDebug() << "TrackWidget destroyed, playlist pointer:" << playlist << "valid?" << (playlist && playlist->is_valid());
    for (QPointer<ClipWidget> w : clipWidgets) if (w) delete w.data();
}

void TrackWidget::rebuildPlaylistFromWidgets() {
    if (!playlist || !playlist->is_valid()) return;
    // Remove all existing producers
    while (playlist->count() > 0) {
        playlist->remove(0);
    }
    int currentFrame = 0;
    for (int i = 0; i < clipWidgets.size(); ++i) {
        ClipWidget* clip = clipWidgets[i];
        if (!clip || !clip->getProducer()) continue;
        int clipStart = clip->getStartFrame();
        int gap = clipStart - currentFrame;
        if (gap > 0) {
            // Insert a blank (color:black) producer for the gap
            Mlt::Producer* blank = new Mlt::Producer(*playlist->profile(), "color:black");
            blank->set("length", gap);
            playlist->append(*blank);
            // Note: blank is owned by playlist after append
        }
        playlist->append(*clip->getProducer());
        currentFrame = clipStart + clip->getDuration();
    }
    // If there is a gap at the end, you may want to add a blank to fill the timeline length (optional)
}

void TrackWidget::addClip(const QString& filePath, int insertIndex) {
    qDebug() << "addClip called, playlist pointer:" << playlist << "valid?" << (playlist && playlist->is_valid());
    if (!playlist || !playlist->is_valid()) {
        qDebug() << "Cannot add clip: invalid playlist";
        return;
    }
    try {
        qDebug() << "Playlist size before append:" << playlist->count();
        auto* producer = new Mlt::Producer(*playlist->profile(), filePath.toUtf8().data());
        qDebug() << "Producer created for" << filePath << ", valid?" << (producer && producer->is_valid()) << ", length:" << (producer ? producer->get_length() : -1);
        if (producer && producer->is_valid()) {
            // Debug: Print producer length for video/audio
            if (isVideoFile(filePath) || isAudioFile(filePath)) {
                qDebug() << "Producer length for" << filePath << ":" << producer->get_length();
            }
            // Set default duration for images
            if (isImageFile(filePath)) {
                int defaultLen = 50; // 2 seconds at 25fps
                if (producer->get_length() > defaultLen * 10) // sanity check
                    producer->set("length", defaultLen);
            }
            int playlistIndex = (insertIndex >= 0 && insertIndex <= playlist->count()) ? insertIndex : playlist->count();
            playlist->insert_at(playlistIndex, *producer, 0);
            qDebug() << "Playlist size after append:" << playlist->count();
            QPixmap thumb;
            QString labelText;
            bool isTextClip = (filePath == "qml:text" || filePath == "qtext:" || filePath.contains(":text"));
            if (isTextClip) {
                // Text element: red-orange background, 'Text' label
                thumb = QPixmap(40, 40);
                thumb.fill(QColor(255, 94, 0)); // Red-orange
                labelText = "Text";
            } else if (isVideoFile(filePath)) {
                thumb = extractVideoFrame(filePath);
                labelText = QFileInfo(filePath).fileName();
            } else if (isImageFile(filePath)) {
                thumb = QPixmap(filePath).scaled(40, 40, Qt::KeepAspectRatio, Qt::SmoothTransformation);
                labelText = QFileInfo(filePath).fileName();
            } else {
                thumb = audioIcon();
                labelText = QFileInfo(filePath).fileName();
            }
            auto* clipWidget = new ClipWidget(producer, thumb, this);
            if (!labelText.isEmpty()) {
                clipWidget->setLabelText(labelText);
            }
            if (isTextClip) {
                clipWidget->setStyleSheet("background: #ff5e00; border: 2px solid #b34700; color: white;");
            }
            int widgetIndex = (insertIndex >= 0 && insertIndex <= clipWidgets.size()) ? insertIndex : clipWidgets.size();
            clipWidgets.insert(widgetIndex, clipWidget);
            qDebug() << "clipWidgets size after append:" << clipWidgets.size();
            // Ensure no overlap: update startFrame for all clips
            updateClipStartFrames();
            rebuildPlaylistFromWidgets();
            refreshUI();
            emit timelineChanged();
        } else {
            qDebug() << "Failed to create valid producer for:" << filePath;
            delete producer;
        }
    } catch (const std::exception& e) {
        qDebug() << "Exception adding clip:" << e.what();
    }
}

void TrackWidget::removeClip(int index) {
    if (index < 0 || index >= clipWidgets.size()) return;
    if (!playlist || !playlist->is_valid()) return;
    try {
        playlist->remove(index);
        auto w = clipWidgets.takeAt(index);
        if (w) w->deleteLater();
        refreshUI();
        updateClipStartFrames();
        rebuildPlaylistFromWidgets();
        refreshUI();
        emit timelineChanged();
    } catch (const std::exception& e) {
        qDebug() << "Exception removing clip:" << e.what();
    }
}

int TrackWidget::getClipCount() const { return clipWidgets.size(); }
Mlt::Playlist* TrackWidget::getPlaylist() { return playlist; }

int TrackWidget::getFrameToPixelScale() const {
    QWidget* p = parentWidget();
    while (p && !p->inherits("TimelineWidget")) p = p->parentWidget();
    if (p && p->inherits("TimelineWidget")) {
        return p->property("frameToPixelScale").toInt();
    }
    return 5; // fallback default
}

// --- Enhanced refreshUI ---
void TrackWidget::refreshUI() {
    qDebug() << "refreshUI: clipWidgets.size()=" << clipWidgets.size();
    // Clean up null pointers
    for (int i = clipWidgets.size() - 1; i >= 0; --i) {
        if (!clipWidgets[i]) {
            qDebug() << "Removing null clipWidget at index" << i;
            clipWidgets.removeAt(i);
        }
    }
    // Create enhanced track header with controls
    createTrackHeader();
    // Position clips with snap support
    int labelOffset = kTrackLabelOffset;
    int frameToPixel = getFrameToPixelScale();
    qDebug() << "refreshUI: frameToPixel=" << frameToPixel;
    // Update snap points for magnetic snapping
    updateSnapPoints();
    for (int i = 0; i < clipWidgets.size(); ++i) {
        ClipWidget* clip = clipWidgets[i];
        if (!clip) continue;
        // Calculate position and size
        int x = labelOffset + clip->getStartFrame() * frameToPixel;
        int w = qMax(60, clip->getDuration() * frameToPixel);
        qDebug() << "Clip" << i << ": startFrame=" << clip->getStartFrame() << ", duration=" << clip->getDuration() << ", width=" << w;
        // Apply snapping if enabled
        if (snapToGrid && !clip->isResizing()) {
            x = snapToNearestPoint(x);
        }
        // Don't interfere with resize operations
        if (!clip->isResizing()) {
            clip->setGeometry(x, 0, w, height());
        }
        // Visual feedback for locked tracks
        if (isLocked) {
            clip->setEnabled(false);
            clip->setStyleSheet(clip->styleSheet() + " border: 2px dashed #666;");
        } else {
            clip->setEnabled(true);
        }
        clip->setParent(this);
        clip->show();
    }
    update();
}

void TrackWidget::createTrackHeader() {
    // Remove any existing header widget/layout
    QWidget* oldHeader = findChild<QWidget*>(QString("trackHeader%1").arg(trackNumber));
    if (oldHeader) {
        delete oldHeader;
    }
    // Create header widget
    QWidget* header = new QWidget(this);
    header->setObjectName(QString("trackHeader%1").arg(trackNumber));
    header->setGeometry(0, 0, kTrackLabelOffset, height());
    QHBoxLayout* layout = new QHBoxLayout(header);
    layout->setContentsMargins(2, 2, 2, 2);
    layout->setSpacing(4);
    // Track label
    QLabel* label = new QLabel(QString("V%1").arg(trackNumber + 1), header);
    label->setAlignment(Qt::AlignCenter);
    label->setFixedWidth(32);
    label->setStyleSheet("font-weight: bold; color: white; background: #222; border-radius: 4px; padding: 2px 4px;");
    layout->addWidget(label);
    // Lock button
    QPushButton* lockBtn = new QPushButton(header);
    lockBtn->setText(isLocked ? "🔒" : "🔓");
    lockBtn->setCheckable(true);
    lockBtn->setChecked(isLocked);
    lockBtn->setFixedSize(22, 22);
    lockBtn->setToolTip("Lock Track");
    lockBtn->setStyleSheet("background: #333; color: #fff; border: 1px solid #555; border-radius: 4px;");
    connect(lockBtn, &QPushButton::toggled, this, &TrackWidget::toggleLock);
    layout->addWidget(lockBtn);
    // Mute button
    QPushButton* muteBtn = new QPushButton(header);
    muteBtn->setText(isMuted ? "🔇" : "🔊");
    muteBtn->setCheckable(true);
    muteBtn->setChecked(isMuted);
    muteBtn->setFixedSize(22, 22);
    muteBtn->setToolTip("Mute Track");
    muteBtn->setStyleSheet(isMuted ? "background: #602020; color: #fff; border: 1px solid #555; border-radius: 4px;" : "background: #333; color: #fff; border: 1px solid #555; border-radius: 4px;");
    connect(muteBtn, &QPushButton::toggled, this, &TrackWidget::toggleMute);
    layout->addWidget(muteBtn);
    // Solo button
    QPushButton* soloBtn = new QPushButton(header);
    soloBtn->setText("S");
    soloBtn->setCheckable(true);
    soloBtn->setChecked(isSolo);
    soloBtn->setFixedSize(22, 22);
    soloBtn->setToolTip("Solo Track");
    soloBtn->setStyleSheet(isSolo ? "background: #206020; color: #fff; font-weight: bold; border: 1px solid #555; border-radius: 4px;" : "background: #333; color: #fff; border: 1px solid #555; border-radius: 4px;");
    connect(soloBtn, &QPushButton::toggled, this, &TrackWidget::toggleSolo);
    layout->addWidget(soloBtn);
    layout->addStretch();
    header->setLayout(layout);
    header->show();
}

void TrackWidget::createTrackControls() {
    // Lock button
    QPushButton* lockBtn = findChild<QPushButton*>(QString("lockBtn%1").arg(trackNumber));
    if (!lockBtn) {
        lockBtn = new QPushButton(this);
        lockBtn->setObjectName(QString("lockBtn%1").arg(trackNumber));
        lockBtn->setFixedSize(20, 20);
        lockBtn->setCheckable(true);
        lockBtn->setToolTip("Lock Track");
        connect(lockBtn, &QPushButton::toggled, this, &TrackWidget::toggleLock);
    }
    lockBtn->setChecked(isLocked);
    lockBtn->setText(isLocked ? "🔒" : "🔓");
    lockBtn->setGeometry(37, 2, 20, 20);
    // Mute button
    QPushButton* muteBtn = findChild<QPushButton*>(QString("muteBtn%1").arg(trackNumber));
    if (!muteBtn) {
        muteBtn = new QPushButton(this);
        muteBtn->setObjectName(QString("muteBtn%1").arg(trackNumber));
        muteBtn->setFixedSize(20, 20);
        muteBtn->setCheckable(true);
        muteBtn->setToolTip("Mute Track");
        connect(muteBtn, &QPushButton::toggled, this, &TrackWidget::toggleMute);
    }
    muteBtn->setChecked(isMuted);
    muteBtn->setText(isMuted ? "🔇" : "🔊");
    muteBtn->setStyleSheet(isMuted ? "background: #602020;" : "");
    muteBtn->setGeometry(59, 2, 20, 20);
    // Solo button
    QPushButton* soloBtn = findChild<QPushButton*>(QString("soloBtn%1").arg(trackNumber));
    if (!soloBtn) {
        soloBtn = new QPushButton(this);
        soloBtn->setObjectName(QString("soloBtn%1").arg(trackNumber));
        soloBtn->setFixedSize(18, 18);
        soloBtn->setCheckable(true);
        soloBtn->setToolTip("Solo Track");
        connect(soloBtn, &QPushButton::toggled, this, &TrackWidget::toggleSolo);
    }
    soloBtn->setChecked(isSolo);
    soloBtn->setText("S");
    soloBtn->setStyleSheet(isSolo ? "background: #206020; color: white; font-weight: bold;" : "font-size: 10px;");
    soloBtn->setGeometry(81, 4, 18, 18);
}

void TrackWidget::updateSnapPoints() {
    snapPoints.clear();
    // Add clip edges
    for (const ClipWidget* clip : clipWidgets) {
        if (clip) {
            snapPoints.append(clip->x());
            snapPoints.append(clip->x() + clip->width());
        }
    }
    // Add grid points (every 10 pixels for fine control)
    for (int x = 100; x < width(); x += 10) {
        snapPoints.append(x);
    }
    // Sort for efficient searching
    std::sort(snapPoints.begin(), snapPoints.end());
}

int TrackWidget::snapToNearestPoint(int x) const {
    if (!snapToGrid || snapPoints.isEmpty()) return x;
    int closest = snapPoints.first();
    int minDistance = qAbs(x - closest);
    for (int point : snapPoints) {
        int distance = qAbs(x - point);
        if (distance < minDistance && distance <= snapTolerance) {
            minDistance = distance;
            closest = point;
        }
    }
    return (minDistance <= snapTolerance) ? closest : x;
}

void TrackWidget::dragEnterEvent(QDragEnterEvent* event) {
    if (event->mimeData()->hasUrls() || event->mimeData()->hasFormat("application/x-dnd-clip-widget")) {
        event->acceptProposedAction();
    } else {
        event->ignore();
    }
}

void TrackWidget::dragMoveEvent(QDragMoveEvent* event) {
    if (isLocked) {
        event->ignore();
        return;
    }
    int insertIndex = calculateInsertIndex(event->pos());
    int targetX;
    if (insertIndex < clipWidgets.size()) {
        targetX = clipWidgets[insertIndex]->x();
    } else {
        targetX = clipWidgets.isEmpty() ? 100 : clipWidgets.last()->geometry().right();
    }
    // Apply snapping to drop indicator
    if (snapToGrid) {
        targetX = snapToNearestPoint(targetX);
    }
    dropIndicatorX = targetX;
    update();
    event->acceptProposedAction();
}

void TrackWidget::dragLeaveEvent(QDragLeaveEvent* event) {
    dropIndicatorX = -1;
    update();
    event->accept();
}

void TrackWidget::dropEvent(QDropEvent* event) {
    if (event->mimeData()->hasFormat("application/x-dnd-clip-widget")) {
        QByteArray data = event->mimeData()->data("application/x-dnd-clip-widget");
        QDataStream stream(&data, QIODevice::ReadOnly);
        qulonglong ptr;
        stream >> ptr;
        ClipWidget* sourceClip = (ClipWidget*)ptr;
        if (sourceClip && sourceClip->parentWidget() == this) {
            int sourceIndex = clipWidgets.indexOf(sourceClip);
            int targetIndex = calculateInsertIndex(event->pos());
            if (sourceIndex != -1 && sourceIndex < targetIndex) {
                targetIndex--;
            }
            if (sourceIndex != targetIndex && sourceIndex != -1) {
                clipWidgets.move(sourceIndex, targetIndex);
                rebuildPlaylistFromWidgets();
                updateClipStartFrames();
                refreshUI();
                emit timelineChanged();
            }
            sourceClip->show();
        }
        event->acceptProposedAction();
    } else if (event->mimeData()->hasUrls()) {
        int insertIndex = calculateInsertIndex(event->pos());
        for (const QUrl& url : event->mimeData()->urls()) {
            addClip(url.toLocalFile(), insertIndex++);
        }
        event->acceptProposedAction();
    }
    dropIndicatorX = -1;
    update();
}

int TrackWidget::calculateInsertIndex(const QPoint& pos) const {
    int targetX = pos.x();
    int labelOffset = kTrackLabelOffset;
    // If before any clips, insert at beginning
    if (clipWidgets.isEmpty() || targetX < labelOffset) {
        return 0;
    }
    // Find the first clip whose left edge is after the drop x
    for (int i = 0; i < clipWidgets.size(); ++i) {
        const ClipWidget* clip = clipWidgets[i];
        if (!clip) continue;
        if (targetX < clip->x()) {
            return i;
        }
    }
    // If after all clips, insert at end
    return clipWidgets.size();
}

void TrackWidget::paintEvent(QPaintEvent* event) {
    QPainter p(this);
    // Track background with subtle gradient
    QLinearGradient bg(0, 0, 0, height());
    QColor baseColor = (trackNumber % 2 == 0) ? QColor(30, 30, 30) : QColor(40, 40, 40);
    bg.setColorAt(0, baseColor.lighter(110));
    bg.setColorAt(1, baseColor.darker(110));
    p.fillRect(rect(), bg);
    // Track separator line
    p.setPen(QPen(trackSeparatorColor, 1));
    p.drawLine(0, height() - 1, width(), height() - 1);
    // Drop indicator with glow effect
    if (dropIndicatorX != -1) {
        p.setPen(QPen(QColor(255, 200, 0), 3));
        p.drawLine(dropIndicatorX, 0, dropIndicatorX, height());
        // Glow effect
        p.setPen(QPen(QColor(255, 200, 0, 100), 6));
        p.drawLine(dropIndicatorX, 0, dropIndicatorX, height());
    }
    // Snap guides
    if (snapToGrid && magneticSnap) {
        p.setPen(QPen(snapLineColor, 1, Qt::DashLine));
        for (int point : snapPoints) {
            if (point > 100 && point < width() - 10) {
                p.drawLine(point, 0, point, height());
            }
        }
    }
    // Selection rectangle for multi-select
    if (isMultiSelecting) {
        p.setPen(QPen(QColor(100, 150, 255), 2));
        p.setBrush(QColor(100, 150, 255, 50));
        p.drawRect(selectionRect);
    }
    // Lock overlay
    if (isLocked) {
        p.fillRect(rect(), QColor(0, 0, 0, 30));
        p.setPen(QPen(QColor(255, 255, 255, 100), 2, Qt::DashLine));
        p.drawRect(rect());
    }
    QWidget::paintEvent(event);
}

void TrackWidget::keyPressEvent(QKeyEvent* event) {
    if (isLocked) {
        event->ignore();
        return;
    }
    if (event->key() == Qt::Key_Delete || event->key() == Qt::Key_Backspace) {
        // Delete all selected clips
        for (auto clip : selectedClips) {
            int index = clipWidgets.indexOf(clip);
            if (index != -1) {
                removeClip(index);
            }
        }
        selectedClips.clear();
    } else if (event->key() == Qt::Key_A && event->modifiers() & Qt::ControlModifier) {
        // Select all clips
        selectedClips.clear();
        for (auto clip : clipWidgets) {
            if (clip) {
                selectedClips.append(clip);
                clip->setSelected(true);
            }
        }
        emit clipSelectionChanged(selectedClips);
    } else if (event->key() == Qt::Key_G && event->modifiers() & Qt::ControlModifier) {
        // Toggle snap to grid
        snapToGrid = !snapToGrid;
        update();
    } else {
        QWidget::keyPressEvent(event);
    }
}

void TrackWidget::updateClipWidths(int frameToPixel) {
    for (int i = 0; i < clipWidgets.size(); ++i) {
        if (clipWidgets[i]) {
            int width = qMax(60, clipWidgets[i]->getDuration() * frameToPixel);
            clipWidgets[i]->setMinimumWidth(width);
            clipWidgets[i]->setMaximumWidth(width);
            clipWidgets[i]->resize(width, clipWidgets[i]->height());
        }
    }
}

void TrackWidget::handleClipResize(ClipWidget* clip, int newStart, int newDuration) {
    if (!clip || !clip->getProducer()) return;
    
    qDebug() << "Handling clip resize - Start:" << newStart << "Duration:" << newDuration;
    
    auto* producer = clip->getProducer();
    QString resource = QString::fromUtf8(producer->get("resource"));
    
    // Update producer duration/timing
    if (isImageFile(resource)) {
        // For images, just set the length
        producer->set("length", newDuration);
    } else {
        // For video/audio, adjust in/out points
        int oldIn = producer->get_in();
        producer->set_in_and_out(oldIn, oldIn + newDuration - 1);
        // Also set length for consistency
        producer->set("length", newDuration);
    }
    
    // Update clip start frame
    clip->setStartFrame(newStart);
    
    // Recalculate all clip positions
    updateClipStartFrames();
    
    // Rebuild playlist to reflect changes
    rebuildPlaylistFromWidgets();
    
    // Debug: Print playlist and clip info after resize
    qDebug() << "[Resize] Clip start:" << clip->getStartFrame() << ", duration:" << clip->getDuration()
             << ", producer in:" << producer->get_in() << ", out:" << producer->get_out() << ", length:" << producer->get_length();
    qDebug() << "[Resize] Playlist now has" << playlist->count() << "clips.";
    for (int i = 0; i < clipWidgets.size(); ++i) {
        if (clipWidgets[i])
            qDebug() << "  Clip" << i << ": start=" << clipWidgets[i]->getStartFrame() << ", duration=" << clipWidgets[i]->getDuration();
    }
    refreshUI();
    emit timelineChanged();
    emit clipResized();
}

void TrackWidget::updateClipStartFrames() {
    int currentFrame = 0;
    for (int i = 0; i < clipWidgets.size(); ++i) {
        if (clipWidgets[i]) {
            clipWidgets[i]->setStartFrame(currentFrame);
            currentFrame += clipWidgets[i]->getDuration();
        }
    }
    // No need to call updateClipWidths here; geometry is handled in refreshUI
}

QList<int> TrackWidget::getClipEdgeFrames() const {
    QList<int> edges;
    int currentFrame = 0;
    for (int i = 0; i < clipWidgets.size(); ++i) {
        if (clipWidgets[i]) {
            edges.append(currentFrame); // start
            currentFrame += clipWidgets[i]->getDuration();
            edges.append(currentFrame); // end
        }
    }
    return edges;
}

bool TrackWidget::splitClipAt(int frame) {
    int currentFrame = 0;
    for (int i = 0; i < clipWidgets.size(); ++i) {
        ClipWidget* clip = clipWidgets[i];
        if (!clip) continue;
        int start = currentFrame;
        int end = start + clip->getDuration();
        if (frame > start && frame < end) {
            // Split here
            int firstLen = frame - start;
            int secondLen = end - frame;
            Mlt::Producer* prod = clip->getProducer();
            if (!prod) return false;
            QString resource = QString::fromUtf8(prod->get("resource"));
            // Create two new producers with correct in/out
            Mlt::Producer* prod1 = new Mlt::Producer(*prod->profile(), resource.toUtf8().data());
            prod1->set_in_and_out(prod->get_in(), prod->get_in() + firstLen - 1);
            Mlt::Producer* prod2 = new Mlt::Producer(*prod->profile(), resource.toUtf8().data());
            prod2->set_in_and_out(prod->get_in() + firstLen, prod->get_out());
            // If image, set length explicitly
            if (isImageFile(resource)) {
                prod1->set("length", firstLen);
                prod2->set("length", secondLen);
            }
            // Remove original
            playlist->remove(i);
            clipWidgets.takeAt(i)->deleteLater();
            // Insert new
            playlist->insert_at(i, *prod2, 0);
            playlist->insert_at(i, *prod1, 0);
            // Add new widgets
            QPixmap thumb;
            if (isVideoFile(resource)) {
                thumb = extractVideoFrame(resource);
            } else if (isImageFile(resource)) {
                thumb = QPixmap(resource).scaled(40, 40, Qt::KeepAspectRatio, Qt::SmoothTransformation);
            } else {
                thumb = audioIcon();
            }
            auto* clip1 = new ClipWidget(prod1, thumb, this);
            auto* clip2 = new ClipWidget(prod2, thumb, this);
            clipWidgets.insert(i, clip2);
            clipWidgets.insert(i, clip1);
            refreshUI();
            updateClipStartFrames();
            rebuildPlaylistFromWidgets();
            refreshUI();
            emit timelineChanged();
            return true;
        }
        currentFrame = end;
    }
    return false;
} 

void TrackWidget::mousePressEvent(QMouseEvent* event) {
    if (isLocked) {
        event->ignore();
        return;
    }
    QPoint pos = event->pos();
    bool onClip = false;
    ClipWidget* clickedClip = nullptr;
    int clickedIndex = -1;
    for (int i = 0; i < clipWidgets.size(); ++i) {
        auto clip = clipWidgets[i];
        if (clip && clip->geometry().contains(pos)) {
            onClip = true;
            clickedClip = clip;
            clickedIndex = i;
            break;
        }
    }
    if (event->type() == QEvent::MouseButtonDblClick && onClip) {
        emit clipDoubleClicked(clickedIndex);
        event->accept();
        return;
    }
    if (event->button() == Qt::LeftButton) {
        if (onClip) {
            // Handle clip selection
            if (event->modifiers() & Qt::ControlModifier) {
                // Multi-select
                if (selectedClips.contains(clickedClip)) {
                    selectedClips.removeOne(clickedClip);
                    clickedClip->setSelected(false);
                } else {
                    selectedClips.append(clickedClip);
                    clickedClip->setSelected(true);
                }
            } else if (event->modifiers() & Qt::ShiftModifier) {
                // Range select
                selectClipRange(clickedClip);
            } else {
                // Single select
                clearSelection();
                selectedClips.append(clickedClip);
                clickedClip->setSelected(true);
            }
        } else {
            // Start multi-selection rectangle
            if (event->modifiers() & Qt::ControlModifier) {
                isMultiSelecting = true;
                selectionStart = pos;
                selectionRect = QRect(pos, pos);
            } else {
                clearSelection();
            }
        }
    } else if (event->button() == Qt::RightButton) {
        // Context menu for clips or track
        showContextMenu(pos, clickedClip);
    }
    emit clipSelectionChanged(selectedClips);
    update();
    QWidget::mousePressEvent(event);
}

void TrackWidget::mouseMoveEvent(QMouseEvent* event) {
    if (isMultiSelecting) {
        selectionRect = QRect(selectionStart, event->pos()).normalized();
        // Update selection based on rectangle
        for (auto clip : clipWidgets) {
            if (clip && selectionRect.intersects(clip->geometry())) {
                if (!selectedClips.contains(clip)) {
                    selectedClips.append(clip);
                    clip->setSelected(true);
                }
            } else if (selectedClips.contains(clip)) {
                selectedClips.removeOne(clip);
                clip->setSelected(false);
            }
        }
        update();
    }
    QWidget::mouseMoveEvent(event);
}

void TrackWidget::mouseReleaseEvent(QMouseEvent* event) {
    if (isMultiSelecting) {
        isMultiSelecting = false;
        emit clipSelectionChanged(selectedClips);
        update();
    }
    QWidget::mouseReleaseEvent(event);
}

void TrackWidget::clearSelection() {
    for (auto clip : selectedClips) {
        if (clip) {
            clip->setSelected(false);
        }
    }
    selectedClips.clear();
}

void TrackWidget::selectClipRange(ClipWidget* endClip) {
    if (selectedClips.isEmpty()) {
        selectedClips.append(endClip);
        endClip->setSelected(true);
        return;
    }
    int startIndex = clipWidgets.indexOf(selectedClips.last());
    int endIndex = clipWidgets.indexOf(endClip);
    if (startIndex == -1 || endIndex == -1) return;
    if (startIndex > endIndex) {
        std::swap(startIndex, endIndex);
    }
    clearSelection();
    for (int i = startIndex; i <= endIndex; ++i) {
        if (i < clipWidgets.size() && clipWidgets[i]) {
            selectedClips.append(clipWidgets[i]);
            clipWidgets[i]->setSelected(true);
        }
    }
}

void TrackWidget::showContextMenu(const QPoint& pos, ClipWidget* clip) {
    QMenu contextMenu(this);
    if (clip) {
        contextMenu.addAction("Cut", [this, clip]() {
            // Implement cut functionality
        });
        contextMenu.addAction("Copy", [this, clip]() {
            // Implement copy functionality
        });
        contextMenu.addAction("Delete", [this, clip]() {
            int index = clipWidgets.indexOf(clip);
            if (index != -1) removeClip(index);
        });
        contextMenu.addSeparator();
        contextMenu.addAction("Split at Playhead", [this, clip]() {
            // Implement split at playhead
        });
        contextMenu.addAction("Fade In", [this, clip]() {
            // Implement fade in
        });
        contextMenu.addAction("Fade Out", [this, clip]() {
            // Implement fade out
        });
    } else {
        contextMenu.addAction("Paste", [this, pos]() {
            // Implement paste functionality
        });
        contextMenu.addSeparator();
        contextMenu.addAction("Lock Track", this, &TrackWidget::toggleLock);
        contextMenu.addAction("Mute Track", this, &TrackWidget::toggleMute);
        contextMenu.addAction("Solo Track", this, &TrackWidget::toggleSolo);
        contextMenu.addSeparator();
        contextMenu.addAction("Delete Track", [this]() {
            // Emit signal to parent to delete this track
        });
    }
    contextMenu.exec(mapToGlobal(pos));
}

// Slot implementations
void TrackWidget::toggleLock() {
    isLocked = !isLocked;
    emit trackStateChanged(trackNumber, isLocked, isMuted, isSolo);
    refreshUI();
}

void TrackWidget::toggleMute() {
    isMuted = !isMuted;
    emit trackStateChanged(trackNumber, isLocked, isMuted, isSolo);
    refreshUI();
}

void TrackWidget::toggleSolo() {
    isSolo = !isSolo;
    emit trackStateChanged(trackNumber, isLocked, isMuted, isSolo);
    refreshUI();
}

void TrackWidget::enableRippleEdit(bool enabled) {
    rippleEditEnabled = enabled;
    // Implement ripple edit logic in clip movement operations
}

void TrackWidget::setSnapEnabled(bool enabled) {
    snapToGrid = enabled;
    update();
} 

bool TrackWidget::hasClipAtFrame(int frame) const {
    for (const QPointer<ClipWidget>& clip : clipWidgets) {
        if (!clip) continue;
        int start = clip->getStartFrame();
        int end = start + clip->getDuration();
        if (frame >= start && frame < end) {
            return true;
        }
    }
    return false;
} 